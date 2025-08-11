#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

static void print_flags(uint64_t flags) {
    printf("[");
    if (flags & X_CF) printf("CF ");
    if (flags & X_PF) printf("PF ");
    if (flags & X_AF) printf("AF ");
    if (flags & X_ZF) printf("ZF ");
    if (flags & X_SF) printf("SF ");
    if (flags & X_OF) printf("OF");
    printf("]");
}

static void print_expected_flags(uint64_t flags) {
    printf("[");
    if (flags & X_CF) printf("CF ");
    if (flags & X_PF) printf("PF ");
    if (flags & X_AF) printf("AF ");
    if (flags & X_ZF) printf("ZF ");
    if (flags & X_SF) printf("SF ");
    if (flags & X_OF) printf("OF");
    printf("]");
}

#define TEST_CASE(title, operation, res_var, expected, flags_expected) \
    do { \
        printf("Test: %s\n", title); \
        CLEAR_FLAGS; \
        operation; \
        uint64_t flags_after = get_eflags(); \
        uint64_t result_value = (uint64_t)(res_var); \
        uint64_t expected_value = (uint64_t)(expected); \
        printf("  Result: 0x%lx, Expected: 0x%lx\n", result_value, expected_value); \
        printf("  Flags: "); print_flags(flags_after); \
        printf("\n"); \
        if (result_value != expected_value) { \
            printf("  ERROR: Result mismatch\n"); \
            errors++; \
        } \
        uint64_t changed_flags = flags_after & (X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF); \
        uint64_t actual_flags = (changed_flags) & (flags_expected); \
        if ((actual_flags) != (flags_expected)) { \
            printf("  ERROR: Flags mismatch (expected "); \
            print_expected_flags(flags_expected); \
            printf(" got "); \
            print_flags(changed_flags); \
            printf(")\n"); \
            errors++; \
        } \
    } while (0)

int main() {
    printf("SUB instruction comprehensive test\n");
    printf("================================\n");
    
    int errors = 0;
    uint8_t  result8  = 0;
    uint16_t result16 = 0;
    uint32_t result32 = 0;
    uint64_t result64 = 0;
    
    // Memory operands
    uint8_t  mem8  = 0;
    uint16_t mem16 = 0;
    uint32_t mem32 = 0;
    uint64_t mem64 = 0;
    
    uint8_t  unaligned8[2]  = {0, 0};
    uint16_t unaligned16[2] = {0, 0};
    uint32_t unaligned32[2] = {0, 0};
    uint64_t unaligned64[2] = {0, 0};
    
    // Test register-register operations
    TEST_CASE("8-bit reg-reg: 0x10 - 0x05", 
        result8 = 0x10; __asm__("subb %1, %0" : "+r"(result8) : "r"((uint8_t)0x05)),
        result8, 0x0B, X_AF);
    
    TEST_CASE("16-bit reg-reg: 0x1000 - 0x2000", 
        result16 = 0x1000; __asm__("subw %1, %0" : "+r"(result16) : "r"((uint16_t)0x2000)),
        result16, 0xF000, X_CF | X_PF | X_SF);
    
    TEST_CASE("32-bit reg-reg: 0x7FFFFFFF - 0x80000000", 
        result32 = 0x7FFFFFFF; __asm__("subl %1, %0" : "+r"(result32) : "r"((uint32_t)0x80000000)),
        result32, 0xFFFFFFFF, X_CF | X_PF | X_SF | X_OF);
    
    TEST_CASE("64-bit reg-reg: 0 - 1", 
        result64 = 0; __asm__("subq %1, %0" : "+r"(result64) : "r"((uint64_t)1)),
        result64, 0xFFFFFFFFFFFFFFFF, X_CF | X_PF | X_SF);
    
    // Test register-memory operations
    mem8 = 0x05;
    TEST_CASE("8-bit reg-mem: AL - [mem8]", 
        __asm__("subb %1, %0" : "=r"(result8) : "m"(mem8), "0"((uint8_t)0x10)),
        result8, 0x0B, X_AF);
    
    mem16 = 0x2000;
    TEST_CASE("16-bit reg-mem: AX - [mem16]", 
        __asm__("subw %1, %0" : "=r"(result16) : "m"(mem16), "0"((uint16_t)0x1000)),
        result16, 0xF000, X_CF | X_PF | X_SF);
    
    mem32 = 0x80000000;
    TEST_CASE("32-bit reg-mem: EAX - [mem32]", 
        __asm__("subl %1, %0" : "=r"(result32) : "m"(mem32), "0"((uint32_t)0x7FFFFFFF)),
        result32, 0xFFFFFFFF, X_CF | X_PF | X_SF | X_OF);
    
    mem64 = 1;
    TEST_CASE("64-bit reg-mem: RAX - [mem64]", 
        __asm__("subq %1, %0" : "=r"(result64) : "m"(mem64), "0"((uint64_t)0)),
        result64, 0xFFFFFFFFFFFFFFFF, X_CF | X_SF | X_AF);
    
    // Test memory-register operations
    mem8 = 0x10;
    result8 = 0x05;
    TEST_CASE("8-bit mem-reg: [mem8] - AL", 
        __asm__("subb %1, %0" : "+m"(mem8) : "r"((uint8_t)0x05)),
        mem8, 0x0B, X_AF);
    
    mem16 = 0x1000;
    result16 = 0x2000;
    TEST_CASE("16-bit mem-reg: [mem16] - AX", 
        __asm__("subw %1, %0" : "+m"(mem16) : "r"((uint16_t)0x2000)),
        mem16, 0xF000, X_CF | X_PF | X_SF);
    
    mem32 = 0x7FFFFFFF;
    result32 = 0x80000000;
    TEST_CASE("32-bit mem-reg: [mem32] - EAX", 
        __asm__("subl %1, %0" : "+m"(mem32) : "r"((uint32_t)0x80000000)),
        mem32, 0xFFFFFFFF, X_CF | X_PF | X_SF | X_OF);
    
    mem64 = 0;
    result64 = 1;
    TEST_CASE("64-bit mem-reg: [mem64] - RAX", 
        __asm__("subq %1, %0" : "+m"(mem64) : "r"((uint64_t)1)),
        mem64, 0xFFFFFFFFFFFFFFFF, X_CF | X_SF | X_AF);
    
    // Test immediate operations
    TEST_CASE("8-bit immediate: AL - 0x05", 
        result8 = 0x0B; __asm__("subb $0x05, %0" : "+r"(result8) : : "cc"),
        result8, 0x06, X_PF);
    
    TEST_CASE("16-bit immediate: AX - 0x1000", 
        result16 = 0xF000; __asm__("subw $0x1000, %0" : "+r"(result16) : : "cc"),
        result16, 0xE000, X_PF | X_SF);
    
    TEST_CASE("32-bit immediate: EAX - 0x7FFFFFFF", 
        result32 = 0xFFFFFFFF; __asm__("subl $0x7FFFFFFF, %0" : "+r"(result32) : : "cc"),
        result32, 0x80000000, X_PF | X_SF);
    
    TEST_CASE("64-bit immediate: RAX - 1", 
        result64 = 0xFFFFFFFFFFFFFFFF; __asm__("subq $1, %0" : "+r"(result64) : : "cc"),
        result64, 0xFFFFFFFFFFFFFFFE, X_SF);
    
    // Test memory-immediate operations
    mem8 = 0x10;
    TEST_CASE("8-bit mem-imm: [mem8] - 0x05", 
        __asm__("subb $0x05, %0" : "+m"(mem8) : : "cc"),
        mem8, 0x0B, X_AF);
    
    mem16 = 0x1000;
    TEST_CASE("16-bit mem-imm: [mem16] - 0x2000", 
        __asm__("subw $0x2000, %0" : "+m"(mem16) : : "cc"),
        mem16, 0xF000, X_CF | X_PF | X_SF);
    
    mem32 = 0x7FFFFFFF;
    TEST_CASE("32-bit mem-imm: [mem32] - 0x80000000", 
        __asm__("subl $0x80000000, %0" : "+m"(mem32) : : "cc"),
        mem32, 0xFFFFFFFF, X_CF | X_PF | X_SF | X_OF);
    
    mem64 = 0;
    TEST_CASE("64-bit mem-imm: [mem64] - 1", 
        __asm__("subq $1, %0" : "+m"(mem64) : : "cc"),
        mem64, 0xFFFFFFFFFFFFFFFF, X_CF | X_SF | X_AF);
    
    // Test unaligned memory access
    uint8_t* unaligned_ptr8 = (uint8_t*)&unaligned8 + 1;
    *unaligned_ptr8 = 0x10;
    TEST_CASE("8-bit unaligned mem-reg", 
        __asm__("subb $0x05, %0" : "+m"(*unaligned_ptr8) : : "cc"),
        *unaligned_ptr8, 0x0B, X_AF);
    
    uint16_t* unaligned_ptr16 = (uint16_t*)((uint8_t*)&unaligned16 + 1);
    *unaligned_ptr16 = 0x1000;
    TEST_CASE("16-bit unaligned mem-reg", 
        __asm__("subw $0x2000, %0" : "+m"(*unaligned_ptr16) : : "cc"),
        *unaligned_ptr16, 0xF000, X_CF | X_PF | X_SF);
    
    uint32_t* unaligned_ptr32 = (uint32_t*)((uint8_t*)&unaligned32 + 1);
    *unaligned_ptr32 = 0x7FFFFFFF;
    TEST_CASE("32-bit unaligned mem-reg", 
        __asm__("subl $0x80000000, %0" : "+m"(*unaligned_ptr32) : : "cc"),
        *unaligned_ptr32, 0xFFFFFFFF, X_CF | X_PF | X_SF | X_OF);
    
    uint64_t* unaligned_ptr64 = (uint64_t*)((uint8_t*)&unaligned64 + 1);
    *unaligned_ptr64 = 0;
    TEST_CASE("64-bit unaligned mem-reg", 
        __asm__("subq $1, %0" : "+m"(*unaligned_ptr64) : : "cc"),
        *unaligned_ptr64, 0xFFFFFFFFFFFFFFFF, X_CF | X_SF | X_AF);
    
    printf("\nSUB test completed\n");
    printf("==================\n");
    printf("Total tests: 20\n");
    printf("Failed tests: %d\n", errors);
    
    return errors ? 1 : 0;
}
