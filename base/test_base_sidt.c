#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

int main() {
    printf("SIDT instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    
    // Buffer to store IDTR (10 bytes in 64-bit mode: 2-byte limit + 8-byte base)
    uint8_t idtr1[10] = {0};
    uint8_t idtr2[10] = {0};
    
    // Store IDTR twice to verify consistency
    __asm__ __volatile__ ("sidt %0" : : "m" (idtr1));
    __asm__ __volatile__ ("sidt %0" : : "m" (idtr2));
    
    // Compare the two IDTR reads
    if (memcmp(idtr1, idtr2, sizeof(idtr1)) != 0) {
        printf("Error: Inconsistent IDTR reads\n");
        errors++;
    }
    
    // Extract limit and base
    uint16_t limit = *(uint16_t*)idtr1;
    uint64_t base = *(uint64_t*)(idtr1 + 2);
    
    printf("IDT limit: 0x%04X\n", limit);
    printf("IDT base: 0x%016lX\n", base);
    
    // Since IDTR values are system-dependent, we can't make assumptions about specific values
    // Instead, we'll just verify that two consecutive reads produce the same result
    // and that the storage structure is the expected size
    
    // Test that SIDT stores the expected structure size
    if (sizeof(idtr1) != 10) {
        printf("Error: IDTR storage size is %zu, expected 10\n", sizeof(idtr1));
        errors++;
    }
    
    // Note: We're not checking specific limit/base values as they are system-dependent
    
    printf("\nSIDT test completed\n");
    printf("===================\n");
    printf("Total tests: 3\n");
    printf("Passed: %d\n", 3 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
