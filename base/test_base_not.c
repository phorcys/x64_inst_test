#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("NOT instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Test 8-bit NOT
    uint8_t val8 = 0b10101010;
    uint8_t result8;
    __asm__ __volatile__ (
        "mov %[val], %%al\n\t"
        "not %%al\n\t"
        : "=a" (result8)
        : [val] "r" (val8)
    );
    
    if (result8 != 0b01010101) {
        printf("Error: 8-bit NOT expected 0x55, got 0x%x\n", result8);
        errors++;
    } else {
        printf("8-bit NOT: PASS\n");
    }
    
    // Test 16-bit NOT
    uint16_t val16 = 0xAAAA;
    uint16_t result16;
    __asm__ __volatile__ (
        "mov %[val], %%ax\n\t"
        "not %%ax\n\t"
        : "=a" (result16)
        : [val] "r" (val16)
    );
    
    if (result16 != 0x5555) {
        printf("Error: 16-bit NOT expected 0x5555, got 0x%x\n", result16);
        errors++;
    } else {
        printf("16-bit NOT: PASS\n");
    }
    
    // Test 32-bit NOT
    uint32_t val32 = 0xAAAAAAAA;
    uint32_t result32;
    __asm__ __volatile__ (
        "mov %[val], %%eax\n\t"
        "not %%eax\n\t"
        : "=a" (result32)
        : [val] "r" (val32)
    );
    
    if (result32 != 0x55555555) {
        printf("Error: 32-bit NOT expected 0x55555555, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit NOT: PASS\n");
    }
    
    // Test 64-bit NOT
    uint64_t val64 = 0xAAAAAAAAAAAAAAAA;
    uint64_t result64;
    __asm__ __volatile__ (
        "mov %[val], %%rax\n\t"
        "not %%rax\n\t"
        : "=a" (result64)
        : [val] "r" (val64)
    );
    
    if (result64 != 0x5555555555555555) {
        printf("Error: 64-bit NOT expected 0x5555555555555555, got 0x%lx\n", result64);
        errors++;
    } else {
        printf("64-bit NOT: PASS\n");
    }
    
    // Test memory operand
    uint16_t mem_val = 0x1234;
    uint16_t expected = ~0x1234;
    __asm__ __volatile__ (
        "notw %[mem]"
        : [mem] "+m" (mem_val)
    );
    
    if (mem_val != expected) {
        printf("Error: Memory NOT expected 0x%x, got 0x%x\n", expected, mem_val);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test zero case
    val32 = 0;
    __asm__ __volatile__ (
        "mov %[val], %%eax\n\t"
        "not %%eax\n\t"
        : "=a" (result32)
        : [val] "r" (val32)
    );
    
    if (result32 != 0xFFFFFFFF) {
        printf("Error: 32-bit NOT (zero) expected 0xFFFFFFFF, got 0x%x\n", result32);
        errors++;
    } else {
        printf("Zero case: PASS\n");
    }
    
    // Test all ones case
    val64 = 0xFFFFFFFFFFFFFFFF;
    __asm__ __volatile__ (
        "mov %[val], %%rax\n\t"
        "not %%rax\n\t"
        : "=a" (result64)
        : [val] "r" (val64)
    );
    
    if (result64 != 0) {
        printf("Error: 64-bit NOT (all ones) expected 0, got 0x%lx\n", result64);
        errors++;
    } else {
        printf("All ones case: PASS\n");
    }
    
    printf("\nNOT test completed\n");
    printf("==================\n");
    printf("Total tests: 7\n");
    printf("Passed: %d\n", 7 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
