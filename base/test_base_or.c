#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("OR instruction test\n");
    printf("===================\n");
    
    int errors = 0;
    uint8_t flags;
    
    // Test 8-bit OR (register-register)
    uint8_t val1_8 = 0b10100010;
    uint8_t val2_8 = 0b01010001;
    uint8_t result8;
    uint8_t expected8 = 0b11110011;
    
    __asm__ __volatile__ (
        "mov %[v1], %%al\n\t"
        "mov %[v2], %%bl\n\t"
        "or %%bl, %%al\n\t"
        "pushfq\n\t"
        "popq %%rdx\n\t"
        : "=a" (result8), "=d" (flags)
        : [v1] "r" (val1_8), [v2] "r" (val2_8)
        : "bl"
    );
    
    if (result8 != expected8) {
        printf("Error: 8-bit OR expected 0x%x, got 0x%x\n", expected8, result8);
        errors++;
    } else if (flags & (1 << 0)) { // CF should be 0
        printf("Error: 8-bit OR flags expected CF=0, got flags=0x%x\n", flags);
        errors++;
    } else {
        printf("8-bit OR (reg-reg): PASS\n");
    }
    
    // Test 16-bit OR (register-memory)
    uint16_t val1_16 = 0xA5A5;
    uint16_t val2_16 = 0x5A5A;
    uint16_t mem_val = val2_16;
    uint16_t result16;
    uint16_t expected16 = 0xFFFF;
    
    __asm__ __volatile__ (
        "mov %[v1], %%ax\n\t"
        "or %[v2], %%ax\n\t"
        : "=a" (result16)
        : [v1] "r" (val1_16), [v2] "m" (mem_val)
    );
    
    if (result16 != expected16) {
        printf("Error: 16-bit OR expected 0x%x, got 0x%x\n", expected16, result16);
        errors++;
    } else {
        printf("16-bit OR (reg-mem): PASS\n");
    }
    
    // Test 32-bit OR (immediate)
    uint32_t val32 = 0x12345678;
    uint32_t result32;
    uint32_t expected32 = 0x1234FEFA;
    
    __asm__ __volatile__ (
        "mov %[val], %%eax\n\t"
        "or $0xAAAA, %%eax\n\t" // OR with immediate value
        : "=a" (result32)
        : [val] "r" (val32)
    );
    
    if (result32 != expected32) {
        printf("Error: 32-bit OR expected 0x%x, got 0x%x\n", expected32, result32);
        errors++;
    } else {
        printf("32-bit OR (immediate): PASS\n");
    }
    
    // Test 64-bit OR (memory-register)
    uint64_t val1_64 = 0x00000000FFFFFFFF;
    uint64_t val2_64 = 0xFFFF000000000000;
    uint64_t mem_val64 = val2_64;
    uint64_t result64;
    uint64_t expected64 = 0xFFFF0000FFFFFFFF;
    
    __asm__ __volatile__ (
        "mov %[v1], %%rax\n\t"
        "or %[v2], %%rax\n\t"
        : "=a" (result64)
        : [v1] "r" (val1_64), [v2] "m" (mem_val64)
    );
    
    if (result64 != expected64) {
        printf("Error: 64-bit OR expected 0x%lx, got 0x%lx\n", expected64, result64);
        errors++;
    } else {
        printf("64-bit OR (mem-reg): PASS\n");
    }
    
    // Test zero flag
    uint8_t zero_val1 = 0;
    uint8_t zero_val2 = 0;
    uint8_t zero_result;
    
    __asm__ __volatile__ (
        "mov %[v1], %%al\n\t"
        "or %[v2], %%al\n\t"
        "pushfq\n\t"
        "popq %%rdx\n\t"
        : "=a" (zero_result), "=d" (flags)
        : [v1] "r" (zero_val1), [v2] "r" (zero_val2)
    );
    
    if (zero_result != 0) {
        printf("Error: Zero test expected 0, got %d\n", zero_result);
        errors++;
    } else if (!(flags & (1 << 6))) { // Check ZF (should be set)
        printf("Error: Zero test flags expected ZF=1, got flags=0x%x\n", flags);
        errors++;
    } else {
        printf("Zero flag test: PASS\n");
    }
    
    // Test sign flag
    int8_t sign_val1 = -128;
    int8_t sign_val2 = -1;
    int8_t sign_result;
    
    __asm__ __volatile__ (
        "mov %[v1], %%al\n\t"
        "or %[v2], %%al\n\t"
        "pushfq\n\t"
        "popq %%rdx\n\t"
        : "=a" (sign_result), "=d" (flags)
        : [v1] "r" (sign_val1), [v2] "r" (sign_val2)
    );
    
    if (sign_result != -1) {
        printf("Error: Sign test expected -1, got %d\n", sign_result);
        errors++;
    } else if (!(flags & (1 << 7))) { // Check SF (should be set for negative)
        printf("Error: Sign test flags expected SF=1, got flags=0x%x\n", flags);
        errors++;
    } else {
        printf("Sign flag test: PASS\n");
    }
    
    // Test memory to memory using register intermediary
    uint32_t mem_op1 = 0x11111111;
    uint32_t mem_op2 = 0x22222222;
    uint32_t expected_mem = 0x33333333;
    
    __asm__ __volatile__ (
        "mov %[op2], %%eax\n\t"
        "orl %%eax, %[op1]"
        : [op1] "+m" (mem_op1)
        : [op2] "m" (mem_op2)
        : "eax"
    );
    
    if (mem_op1 != expected_mem) {
        printf("Error: Memory-memory OR expected 0x%x, got 0x%x\n", expected_mem, mem_op1);
        errors++;
    } else {
        printf("Memory-memory OR: PASS\n");
    }
    
    printf("\nOR test completed\n");
    printf("================\n");
    printf("Total tests: 7\n");
    printf("Passed: %d\n", 7 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
