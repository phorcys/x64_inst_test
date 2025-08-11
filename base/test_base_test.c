#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to check flags after TEST instruction
static void check_test_flags(uint64_t result, uint8_t width, uint64_t eflags) {
    printf("  Result: 0x%0*lx, Flags: 0x%08lx\n", (width+3)/4, result, eflags);
    
    // Extract flag values
    int sf = (eflags & X_SF) != 0;
    int zf = (eflags & X_ZF) != 0;
    int pf = (eflags & X_PF) != 0;
    int cf = (eflags & X_CF) != 0;
    int of = (eflags & X_OF) != 0;
    
    printf("  Flags: SF=%d ZF=%d PF=%d CF=%d OF=%d\n", sf, zf, pf, cf, of);
    
    // Check flag expectations
    int errors = 0;
    
    // SF should match high bit of result
    uint64_t sign_bit = (width == 8) ? 0x80 : 
                       (width == 16) ? 0x8000 : 
                       (width == 32) ? 0x80000000 : 0x8000000000000000;
    int expected_sf = (result & sign_bit) != 0;
    if (expected_sf != sf) {
        printf("    ERROR: SF expected %d, got %d\n", expected_sf, sf);
        errors++;
    }
    
    // ZF should be set when result is zero
    int expected_zf = (result == 0);
    if (expected_zf != zf) {
        printf("    ERROR: ZF expected %d, got %d\n", expected_zf, zf);
        errors++;
    }
    
    // PF should reflect parity of low 8 bits
    uint8_t low_byte = result & 0xFF;
    int parity = 0;
    for (int i = 0; i < 8; i++) {
        parity ^= (low_byte >> i) & 1;
    }
    int expected_pf = !parity; // PF=1 when even number of bits
    if (expected_pf != pf) {
        printf("    ERROR: PF expected %d, got %d\n", expected_pf, pf);
        errors++;
    }
    
    // CF and OF should always be cleared by TEST
    if (cf) {
        printf("    ERROR: CF should be 0, got 1\n");
        errors++;
    }
    if (of) {
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
    set_eflags(0);
    __asm__ volatile ("testb $0x55, %%al" : "=a"(al_val) : "a"(0xAA) : "cc");
    check_test_flags(0xAA & 0x55, 8, get_eflags());
    
    // 16-bit
    uint16_t ax_val;
    set_eflags(0);
    __asm__ volatile ("testw $0x5555, %%ax" : "=a"(ax_val) : "a"(0xAAAA) : "cc");
    check_test_flags(0xAAAA & 0x5555, 16, get_eflags());
    
    // 32-bit
    uint32_t eax_val;
    set_eflags(0);
    __asm__ volatile ("testl $0x55555555, %%eax" : "=a"(eax_val) : "a"(0xAAAAAAAA) : "cc");
    check_test_flags(0xAAAAAAAA & 0x55555555, 32, get_eflags());
    
    // 64-bit
    uint64_t rax_val;
    set_eflags(0);
    __asm__ volatile ("testq $0x55555555, %%rax" : "=a"(rax_val) : "a"(0xAAAAAAAA00000000) : "cc");
    check_test_flags(0xAAAAAAAA00000000 & 0x55555555, 64, get_eflags());
}

// Test memory-immediate forms
static void test_mem_imm_forms() {
    printf("Testing TEST with memory-immediate forms:\n");
    
    uint8_t mem8 = 0xAA;
    uint16_t mem16 = 0xAAAA;
    uint32_t mem32 = 0xAAAAAAAA;
    uint64_t mem64 = 0xAAAAAAAA00000000;
    
    // 8-bit
    set_eflags(0);
    __asm__ volatile ("testb $0x55, %0" : : "m"(mem8) : "cc");
    check_test_flags(mem8 & 0x55, 8, get_eflags());
    
    // 16-bit
    set_eflags(0);
    __asm__ volatile ("testw $0x5555, %0" : : "m"(mem16) : "cc");
    check_test_flags(mem16 & 0x5555, 16, get_eflags());
    
    // 32-bit
    set_eflags(0);
    __asm__ volatile ("testl $0x55555555, %0" : : "m"(mem32) : "cc");
    check_test_flags(mem32 & 0x55555555, 32, get_eflags());
    
    // 64-bit
    set_eflags(0);
    __asm__ volatile ("testq $0x55555555, %0" : : "m"(mem64) : "cc");
    check_test_flags(mem64 & 0x55555555, 64, get_eflags());
}

// Test register-register forms
static void test_reg_reg_forms() {
    printf("Testing TEST with register-register forms:\n");
    
    // 8-bit
    uint8_t a8 = 0xAA, b8 = 0x55;
    set_eflags(0);
    __asm__ volatile ("testb %1, %0" : : "r"(a8), "r"(b8) : "cc");
    check_test_flags(a8 & b8, 8, get_eflags());
    
    // 16-bit
    uint16_t a16 = 0xAAAA, b16 = 0x5555;
    set_eflags(0);
    __asm__ volatile ("testw %1, %0" : : "r"(a16), "r"(b16) : "cc");
    check_test_flags(a16 & b16, 16, get_eflags());
    
    // 32-bit
    uint32_t a32 = 0xAAAAAAAA, b32 = 0x55555555;
    set_eflags(0);
    __asm__ volatile ("testl %1, %0" : : "r"(a32), "r"(b32) : "cc");
    check_test_flags(a32 & b32, 32, get_eflags());
    
    // 64-bit
    uint64_t a64 = 0xAAAAAAAA00000000, b64 = 0x55555555;
    set_eflags(0);
    __asm__ volatile ("testq %1, %0" : : "r"(a64), "r"(b64) : "cc");
    check_test_flags(a64 & b64, 64, get_eflags());
}

// Test register-memory forms
static void test_reg_mem_forms() {
    printf("Testing TEST with register-memory forms:\n");
    
    uint8_t mem8 = 0xAA;
    uint16_t mem16 = 0xAAAA;
    uint32_t mem32 = 0xAAAAAAAA;
    uint64_t mem64 = 0xAAAAAAAA00000000;
    
    // 8-bit
    uint8_t reg8;
    set_eflags(0);
    __asm__ volatile ("testb %1, %0" : "=r"(reg8) : "m"(mem8) : "cc");
    check_test_flags(reg8 & mem8, 8, get_eflags());
    
    // 16-bit
    uint16_t reg16;
    set_eflags(0);
    __asm__ volatile ("testw %1, %0" : "=r"(reg16) : "m"(mem16) : "cc");
    check_test_flags(reg16 & mem16, 16, get_eflags());
    
    // 32-bit
    uint32_t reg32;
    set_eflags(0);
    __asm__ volatile ("testl %1, %0" : "=r"(reg32) : "m"(mem32) : "cc");
    check_test_flags(reg32 & mem32, 32, get_eflags());
    
    // 64-bit
    uint64_t reg64;
    set_eflags(0);
    __asm__ volatile ("testq %1, %0" : "=r"(reg64) : "m"(mem64) : "cc");
    check_test_flags(reg64 & mem64, 64, get_eflags());
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
