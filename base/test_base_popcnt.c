#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("POPCNT instruction test\n");
    printf("=======================\n");
    
    int errors = 0;
    
    // Test 16-bit POPCNT
    uint16_t val16 = 0xAAAA; // 8 bits set
    uint16_t result16;
    __asm__ __volatile__ (
        "popcntw %[val], %[res]"
        : [res] "=r" (result16)
        : [val] "r" (val16)
    );
    
    if (result16 != 8) {
        printf("Error: 16-bit POPCNT expected 8, got %d\n", result16);
        errors++;
    } else {
        printf("16-bit POPCNT: PASS\n");
    }
    
    // Test 32-bit POPCNT
    uint32_t val32 = 0xFFFFFFFF; // 32 bits set
    uint32_t result32;
    __asm__ __volatile__ (
        "popcntl %[val], %[res]"
        : [res] "=r" (result32)
        : [val] "r" (val32)
    );
    
    if (result32 != 32) {
        printf("Error: 32-bit POPCNT expected 32, got %d\n", result32);
        errors++;
    } else {
        printf("32-bit POPCNT: PASS\n");
    }
    
    // Test 64-bit POPCNT
    uint64_t val64 = 0x5555555555555555; // 32 bits set
    uint64_t result64;
    __asm__ __volatile__ (
        "popcntq %[val], %[res]"
        : [res] "=r" (result64)
        : [val] "r" (val64)
    );
    
    if (result64 != 32) {
        printf("Error: 64-bit POPCNT expected 32, got %lu\n", result64);
        errors++;
    } else {
        printf("64-bit POPCNT: PASS\n");
    }
    
    // Test zero input
    val64 = 0;
    __asm__ __volatile__ (
        "popcntq %[val], %[res]"
        : [res] "=r" (result64)
        : [val] "r" (val64)
    );
    
    if (result64 != 0) {
        printf("Error: Zero POPCNT expected 0, got %lu\n", result64);
        errors++;
    } else {
        printf("Zero input: PASS\n");
    }
    
    // Test all bits set (64-bit version)
    val64 = 0xFFFFFFFFFFFFFFFF; // 64 bits set
    __asm__ __volatile__ (
        "popcntq %[val], %[res]"
        : [res] "=r" (result64)
        : [val] "r" (val64)
    );
    
    if (result64 != 64) {
        printf("Error: All bits set POPCNT expected 64, got %lu\n", result64);
        errors++;
    } else {
        printf("All bits set: PASS\n");
    }
    
    // Test memory operand
    uint64_t mem_val = 0x123456789ABCDEF0;
    uint64_t mem_result;
    // Count bits: 0x1=1, 0x2=1, 0x3=2, 0x4=1, 0x5=2, 0x6=2, 0x7=3, 0x8=1, 0x9=2, etc.
    // Total bits set: 32
    __asm__ __volatile__ (
        "popcntq %[val], %[res]"
        : [res] "=r" (mem_result)
        : [val] "m" (mem_val)
    );
    
    if (mem_result != 32) {
        printf("Error: Memory operand POPCNT expected 32, got %lu\n", mem_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test different register
    uint64_t rbx_val = 0xAAAAAAAAAAAAAAAA; // 32 bits set
    uint64_t rbx_result;
    __asm__ __volatile__ (
        "popcntq %[val], %%rbx"
        : "=b" (rbx_result)
        : [val] "r" (rbx_val)
    );
    
    if (rbx_result != 32) {
        printf("Error: RBX POPCNT expected 32, got %lu\n", rbx_result);
        errors++;
    } else {
        printf("RBX register: PASS\n");
    }
    
    printf("\nPOPCNT test completed\n");
    printf("=====================\n");
    printf("Total tests: 7\n");
    printf("Passed: %d\n", 7 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
