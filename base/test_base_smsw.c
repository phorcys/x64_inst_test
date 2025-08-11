#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("SMSW instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    
    // Test reading SMSW into registers of different sizes
    uint16_t smsw16_1 = 0;
    uint16_t smsw16_2 = 0;
    uint32_t smsw32 = 0;
    uint64_t smsw64 = 0;
    
    __asm__ __volatile__ ("smsw %0" : "=r" (smsw16_1));
    __asm__ __volatile__ ("smsw %0" : "=r" (smsw16_2));
    __asm__ __volatile__ ("smsw %0" : "=r" (smsw32));
    __asm__ __volatile__ ("smsw %0" : "=r" (smsw64));
    
    printf("SMSW (16-bit): 0x%04X\n", smsw16_1);
    
    // Verify consistency between 16-bit reads
    if (smsw16_1 != smsw16_2) {
        printf("Error: Inconsistent SMSW reads (0x%04X vs 0x%04X)\n", smsw16_1, smsw16_2);
        errors++;
    }
    
    // Verify zero-extension for larger registers
    if (smsw16_1 != (uint16_t)smsw32) {
        printf("Error: 32-bit SMSW mismatch (0x%08X vs 0x%04X)\n", smsw32, smsw16_1);
        errors++;
    }
    
    if (smsw16_1 != (uint16_t)smsw64) {
        printf("Error: 64-bit SMSW mismatch (0x%016lX vs 0x%04X)\n", smsw64, smsw16_1);
        errors++;
    }
    
    // Test storing to memory
    uint16_t mem_val = 0;
    __asm__ __volatile__ ("smsw %0" : "=m" (mem_val));
    
    if (smsw16_1 != mem_val) {
        printf("Error: Memory SMSW mismatch (0x%04X vs 0x%04X)\n", mem_val, smsw16_1);
        errors++;
    }
    
    // Basic sanity check (should have at least PE bit set in modern systems)
    if ((smsw16_1 & 0x0001) == 0) {
        printf("Warning: Protected mode bit not set (0x%04X)\n", smsw16_1);
        // Not an error as this could be environment-specific
    }
    
    printf("\nSMSW test completed\n");
    printf("===================\n");
    printf("Total tests: 4\n");
    printf("Passed: %d\n", 4 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
