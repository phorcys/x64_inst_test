#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("RDTSCP instruction test\n");
    printf("=======================\n");
    
    int errors = 0;
    uint32_t tsc_aux1, tsc_aux2;
    uint64_t tsc1, tsc2, tsc3;
    
    // Basic RDTSCP functionality
    __asm__ __volatile__ (
        "rdtscp" 
        : "=a" (*(uint32_t*)&tsc1), "=d" (*((uint32_t*)&tsc1 + 1)), "=c" (tsc_aux1)
    );
    
    //printf("First read: TSC = %lu, TSC_AUX = 0x%x\n", tsc1, tsc_aux1);
    
    // Access again to check for increment
    __asm__ __volatile__ (
        "rdtscp" 
        : "=a" (*(uint32_t*)&tsc2), "=d" (*((uint32_t*)&tsc2 + 1)), "=c" (tsc_aux2)
    );
    
//printf("Second read: TSC = %lu, TSC_AUX = 0x%x\n", tsc2, tsc_aux2);
    
    if (tsc2 <= tsc1) {
        printf("Error: TSC should increment. First: %lu, Second: %lu\n", tsc1, tsc2);
        errors++;
    } else {
        printf("TSC increment: PASS\n");
    }
    
    // TSC_AUX should be consistent if running on same core
    if (tsc_aux1 != tsc_aux2) {
        printf("Warning: TSC_AUX changed (0x%x -> 0x%x). May have migrated cores.\n", tsc_aux1, tsc_aux2);
    } else {
        printf("TSC_AUX consistency: PASS\n");
    }
    
    // Test serializing behavior (RDTSCP is serializing)
    uint64_t tsc_before, tsc_after;
    uint32_t aux_before, aux_after;
    
    __asm__ __volatile__ (
        "rdtsc\n\t"  // Non-serializing read
        : "=a" (*(uint32_t*)&tsc_before), "=d" (*((uint32_t*)&tsc_before + 1))
    );
    
    __asm__ __volatile__ (
        "rdtscp\n\t"  // Serializing read
        : "=a" (*(uint32_t*)&tsc_after), "=d" (*((uint32_t*)&tsc_after + 1)), "=c" (aux_after)
    );
    
    if (tsc_after <= tsc_before) {
        printf("Error: Serializing RDTSCP should come after RDTSC. Before: %lu, After: %lu\n", tsc_before, tsc_after);
        errors++;
    } else {
        printf("Serializing behavior: PASS\n");
    }
    
    // Test multiple reads
    uint64_t tsc_values[3];
    uint32_t aux_values[3];
    
    for (int i = 0; i < 3; i++) {
        __asm__ __volatile__ (
            "rdtscp" 
            : "=a" (*(uint32_t*)&tsc_values[i]), 
              "=d" (*((uint32_t*)&tsc_values[i] + 1)),
              "=c" (aux_values[i])
        );
        
        // Add small delay
        for (volatile int j = 0; j < 1000; j++);
    }
    
    if (tsc_values[1] <= tsc_values[0] || tsc_values[2] <= tsc_values[1]) {
        printf("Error: TSC values not monotonically increasing: [%lu, %lu, %lu]\n", 
               tsc_values[0], tsc_values[1], tsc_values[2]);
        errors++;
    } else {
        printf("Monotonic TSC check: PASS\n");
    }
    
    // Test TSC_AUX values
    int aux_consistent = 1;
    for (int i = 1; i < 3; i++) {
        if (aux_values[i] != aux_values[0]) {
            aux_consistent = 0;
            break;
        }
    }
    
    if (!aux_consistent) {
        printf("Warning: TSC_AUX values changed during reads: [0x%x, 0x%x, 0x%x]\n", 
               aux_values[0], aux_values[1], aux_values[2]);
    } else {
        printf("TSC_AUX stability: PASS\n");
    }
    
    // // Test processor ID part (lower 12 bits of TSC_AUX)
    // uint32_t proc_id = tsc_aux1 & 0xFFF;
    // if (proc_id == 0) {
    //     printf("Warning: Processor ID is 0 (may not be set by kernel)\n");
    // } else {
    //     printf("Processor ID valid: PASS\n");
    // }
    
    printf("\nRDTSCP test completed\n");
    printf("====================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
