#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("SHRD instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    
    // Test 16-bit SHRD with immediate count
    uint16_t dest16 = 0x1234;
    uint16_t src16 = 0x5678;
    uint16_t expected16 = (dest16 >> 8) | (src16 << 8);
    __asm__ __volatile__ (
        "shrdw $8, %1, %0"
        : "+r" (dest16)
        : "r" (src16)
    );
    
    printf("16-bit SHRD (>>8): 0x%04X\n", dest16);
    if (dest16 != expected16) {
        printf("Error: Expected 0x%04X, got 0x%04X\n", expected16, dest16);
        errors++;
    }

    // Test 32-bit SHRD with CL count
    uint32_t dest32 = 0x12345678;
    uint32_t src32 = 0x9ABCDEF0;
    uint32_t expected32 = (dest32 >> 16) | (src32 << 16);
    uint8_t count = 16;
    __asm__ __volatile__ (
        "mov %2, %%cl\n\t"
        "shrdl %%cl, %1, %0"
        : "+r" (dest32)
        : "r" (src32), "r" (count)
        : "cl"
    );
    
    printf("32-bit SHRD (>>16): 0x%08X\n", dest32);
    if (dest32 != expected32) {
        printf("Error: Expected 0x%08X, got 0x%08X\n", expected32, dest32);
        errors++;
    }

    // Test 64-bit SHRD with immediate count
    uint64_t dest64 = 0x0123456789ABCDEF;
    uint64_t src64 = 0xFEDCBA9876543210;
    uint64_t expected64 = (dest64 >> 32) | (src64 << 32);
    __asm__ __volatile__ (
        "shrdq $32, %1, %0"
        : "+r" (dest64)
        : "r" (src64)
    );
    
    printf("64-bit SHRD (>>32): 0x%016lX\n", dest64);
    if (dest64 != expected64) {
        printf("Error: Expected 0x%016lX, got 0x%016lX\n", expected64, dest64);
        errors++;
    }

    // Test memory operand SHRD
    uint16_t mem_dest = 0x8000;
    uint16_t mem_src = 0x0001;
    uint16_t expected_mem = (mem_dest >> 4) | (mem_src << 12);
    __asm__ __volatile__ (
        "shrdw $4, %1, %0"
        : "+m" (mem_dest)
        : "r" (mem_src)
    );
    
    printf("Memory SHRD (>>4): 0x%04X\n", mem_dest);
    if (mem_dest != expected_mem) {
        printf("Error: Expected 0x%04X, got 0x%04X\n", expected_mem, mem_dest);
        errors++;
    }

    // Test carry flag (CF) after SHRD
    uint16_t cf_dest = 0x0001;
    uint16_t cf_src = 0x8000;
    uint8_t cf_flag = 0;
    __asm__ __volatile__ (
        "shrdw $1, %2, %1\n\t"
        "setc %0"
        : "=r" (cf_flag), "+r" (cf_dest)
        : "r" (cf_src)
    );
    
    printf("16-bit SHRD CF test: CF=%d\n", cf_flag);
    if (cf_flag != 1) {
        printf("Error: Expected CF=1, got CF=%d\n", cf_flag);
        errors++;
    }

    // Test masked count (count > operand size)
    uint32_t mask_dest = 0x80000000;
    uint32_t mask_src = 0x00000001;
    uint32_t mask_expected = (mask_dest >> 1) | (mask_src << 31);
    __asm__ __volatile__ (
        "shrdl $33, %1, %0"  // 33 masked to 1 (32-bit: 33 mod 32 = 1)
        : "+r" (mask_dest)
        : "r" (mask_src)
    );
    
    printf("32-bit masked SHRD (>>33 masked to 1): 0x%08X\n", mask_dest);
    if (mask_dest != mask_expected) {
        printf("Error: Expected 0x%08X, got 0x%08X\n", mask_expected, mask_dest);
        errors++;
    }

    printf("\nSHRD test completed\n");
    printf("===================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
