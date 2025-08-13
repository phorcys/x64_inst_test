#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to check flags after TEST instruction
static void check_test_flags(uint64_t result, uint8_t width, uint64_t eflags) {
    printf("  Result: 0x%0*lx\n", (width+3)/4, result);
    
    // Print affected flags
    printf("  Flags: ");
    print_eflags_cond(eflags, X_CF | X_PF | X_ZF | X_SF | X_OF);
    printf("\n");
    
    // Check flag expectations
    int errors = 0;
    
    // SF should match high bit of result
    uint64_t sign_bit = (width == 8) ? 0x80 : 
                       (width == 16) ? 0x8000 : 
                       (width == 32) ? 0x80000000 : 0x8000000000000000;
    int expected_sf = (result & sign_bit) != 0;
    int actual_sf = (eflags & X_SF) != 0;
    if (expected_sf != actual_sf) {
        printf("    ERROR: SF expected %d, got %d\n", expected_sf, actual_sf);
        errors++;
    }
    
    // ZF should be set when result is zero
    int expected_zf = (result == 0);
    int actual_zf = (eflags & X_ZF) != 0;
    if (expected_zf != actual_zf) {
        printf("    ERROR: ZF expected %d, got %d\n", expected_zf, actual_zf);
        errors++;
    }
    
    // PF should reflect parity of low 8 bits
    uint8_t low_byte = result & 0xFF;
    int parity = 0;
    for (int i = 0; i < 8; i++) {
        parity ^= (low_byte >> i) & 1;
    }
    int expected_pf = !parity; // PF=1 when even number of bits
    int actual_pf = (eflags & X_PF) != 0;
    if (expected_pf != actual_pf) {
        printf("    ERROR: PF expected %d, got %d\n", expected_pf, actual_pf);
        errors++;
    }
    
    // CF and OF should always be cleared by TEST
    if (eflags & X_CF) {
        printf("    ERROR: CF should be 0, got 1\n");
        errors++;
    }
    if (eflags & X_OF) {
        printf("    ERROR: OF should be 0, got 1\n");
        errors++;
    }
    
    if (errors == 0) {
        printf("    Flags OK\n");
    }
}

// Test register-immediate forms
static void test_imm_forms() {
    printf("Testing TEST with immediate forms:\n");
    
    // 8-bit
    uint8_t al_val;
    uint64_t flags_after;
    set_eflags(0);
    __asm__ volatile (
        "testb $0x55, %%al\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "=a"(al_val), "=r"(flags_after)
        : "a"(0xAA)
        : "cc"
    );
    check_test_flags(0xAA & 0x55, 8, flags_after);
    
    // 16-bit
    uint16_t ax_val;
    uint64_t flags_after16;
    set_eflags(0);
    __asm__ volatile (
        "testw $0x5555, %%ax\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "=a"(ax_val), "=r"(flags_after16)
        : "a"(0xAAAA)
        : "cc"
    );
    check_test_flags(0xAAAA & 0x5555, 16, flags_after16);
    
    // 32-bit
    uint32_t eax_val;
    uint64_t flags_after32;
    set_eflags(0);
    __asm__ volatile (
        "testl $0x55555555, %%eax\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "=a"(eax_val), "=r"(flags_after32)
        : "a"(0xAAAAAAAA)
        : "cc"
    );
    check_test_flags(0xAAAAAAAA & 0x55555555, 32, flags_after32);
    
    // 64-bit
    uint64_t rax_val;
    uint64_t flags_after64;
    set_eflags(0);
    __asm__ volatile (
        "testq $0x55555555, %%rax\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "=a"(rax_val), "=r"(flags_after64)
        : "a"(0xAAAAAAAA00000000)
        : "cc"
    );
    check_test_flags(0xAAAAAAAA00000000 & 0x55555555, 64, flags_after64);
}

// Test memory-immediate forms
static void test_mem_imm_forms() {
    printf("Testing TEST with memory-immediate forms:\n");
    
    uint8_t mem8 = 0xAA;
    uint16_t mem16 = 0xAAAA;
    uint32_t mem32 = 0xAAAAAAAA;
    uint64_t mem64 = 0xAAAAAAAA00000000;
    
    // 8-bit
    uint64_t flags;
    __asm__ volatile (
        "testb $0x55, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem8)
        : "cc"
    );
    check_test_flags(mem8 & 0x55, 8, flags);
    
    // 16-bit
    __asm__ volatile (
        "testw $0x5555, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem16)
        : "cc"
    );
    check_test_flags(mem16 & 0x5555, 16, flags);
    
    // 32-bit
    __asm__ volatile (
        "testl $0x55555555, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem32)
        : "cc"
    );
    check_test_flags(mem32 & 0x55555555, 32, flags);
    
    // 64-bit
    __asm__ volatile (
        "testq $0x55555555, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem64)
        : "cc"
    );
    check_test_flags(mem64 & 0x55555555, 64, flags);
}

// Test register-register forms
static void test_reg_reg_forms() {
    printf("Testing TEST with register-register forms:\n");
    uint64_t flags;
    // 8-bit
    uint8_t a8 = 0xAA, b8 = 0x55;
    __asm__ volatile (
        "testb %2, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "r" (a8), "r" (b8)
        : "cc"
    );
    check_test_flags(a8 & b8, 8, flags);
    
    // 16-bit
    uint16_t a16 = 0xAAAA, b16 = 0x5555;
    __asm__ volatile (
        "testw %2, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "r" (a16), "r" (b16)
        : "cc"
    );
    check_test_flags(a16 & b16, 16, flags);
    
    // 32-bit
    uint32_t a32 = 0xAAAAAAAA, b32 = 0x55555555;
    __asm__ volatile (
        "testl %2, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "r" (a32), "r" (b32)
        : "cc"
    );
    check_test_flags(a32 & b32, 32, flags);
    
    // 64-bit
    uint64_t a64 = 0xAAAAAAAA00000000, b64 = 0x55555555;
    __asm__ volatile (
        "testq %2, %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "r" (a64), "r" (b64)
        : "cc"
    );
    check_test_flags(a64 & b64, 64, flags);
}

// Test register-memory forms
static void test_reg_mem_forms() {
    printf("Testing TEST with register-memory forms:\n");
    uint64_t flags;
    uint8_t mem8 = 0xAA;
    uint16_t mem16 = 0xAAAA;
    uint32_t mem32 = 0xAAAAAAAA;
    uint64_t mem64 = 0xAAAAAAAA00000000;
    
    // 8-bit
    // Initialize register with known value
    uint8_t reg8_val = 0xAA;
    __asm__ volatile (
        "testb %1, %2\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem8), "r" (reg8_val)
        : "cc"
    );
    check_test_flags(reg8_val & mem8, 8, flags);
    
    // 16-bit
    uint16_t reg16_val = 0xAAAA;
    __asm__ volatile (
        "testw %1, %2\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem16), "r" (reg16_val)
        : "cc"
    );
    check_test_flags(reg16_val & mem16, 16, flags);
    
    // 32-bit
    uint32_t reg32_val = 0xAAAAAAAA;
    __asm__ volatile (
        "testl %1, %2\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem32), "r" (reg32_val)
        : "cc"
    );
    check_test_flags(reg32_val & mem32, 32, flags);
    
    // 64-bit
    uint64_t reg64_val = 0xAAAAAAAA00000000;
    __asm__ volatile (
        "testq %1, %2\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags)
        : "m" (mem64), "r" (reg64_val)
        : "cc"
    );
    check_test_flags(reg64_val & mem64, 64, flags);
}

int main() {
    printf("TEST instruction test\n");
    printf("====================\n");
    
    test_imm_forms();
    test_mem_imm_forms();
    test_reg_reg_forms();
    test_reg_mem_forms();
    
    printf("\nTEST test completed\n");
    printf("==================\n");
    
    return 0;
}
