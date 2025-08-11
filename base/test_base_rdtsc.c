#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("RDTSC instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    uint64_t tsc1, tsc2, tsc3;
    
    // Basic RDTSC functionality
    __asm__ __volatile__ (
        "rdtsc" 
        : "=a" (*(uint32_t*)&tsc1), "=d" (*((uint32_t*)&tsc1 + 1))
    );
    
    // Access again to check for increment
    __asm__ __volatile__ (
        "rdtsc" 
        : "=a" (*(uint32_t*)&tsc2), "=d" (*((uint32_t*)&tsc2 + 1))
    );
    
    if (tsc2 <= tsc1) {
        printf("Error: TSC should increment. First: %lu, Second: %lu\n", tsc1, tsc2);
        errors++;
    } else {
        printf("TSC increment: PASS \n");
    }
    
    // Test multiple reads in a loop
    uint64_t prev = tsc2;
    for (int i = 0; i < 5; i++) {
        __asm__ __volatile__ (
            "rdtsc" 
            : "=a" (*(uint32_t*)&tsc3), "=d" (*((uint32_t*)&tsc3 + 1))
        );
        
        if (tsc3 <= prev) {
            printf("Error: TSC not incrementing. Previous: %lu, Current: %lu\n", prev, tsc3);
            errors++;
        }
        prev = tsc3;
    }
    
    // Test serializing instruction (CPUID) before RDTSC
    uint64_t tsc_before, tsc_after;
    uint32_t low_before, high_before, low_after, high_after;

    // First get TSC after serialization
    __asm__ __volatile__ (
        "cpuid\n\t"
        "rdtsc\n\t"
        : "=a" (low_before), "=d" (high_before)
        : : "rbx", "rcx"
    );
    tsc_before = ((uint64_t)high_before << 32) | low_before;

    // Then get TSC before serialization
    __asm__ __volatile__ (
        "rdtsc\n\t"
        : "=a" (low_after), "=d" (high_after)
    );
    tsc_after = ((uint64_t)high_after << 32) | low_after;
    __asm__ __volatile__ (
        "cpuid\n\t"
        : : : "rbx", "rcx"
    );
    
    if (tsc_after <= tsc_before) {
        printf("Error: TSC after CPUID should be greater. Before: %lu, After: %lu\n", tsc_before, tsc_after);
        errors++;
    } else {
        printf("RDTSC with CPUID serialization: PASS\n");
    }
    
    // Test with memory barrier
    uint64_t tsc_barrier1, tsc_barrier2;
    __asm__ __volatile__ (
        "mfence\n\t"
        "rdtsc\n\t"
        : "=a" (*(uint32_t*)&tsc_barrier1), "=d" (*((uint32_t*)&tsc_barrier1 + 1))
    );
    
    __asm__ __volatile__ (
        "rdtsc\n\t"
        "mfence\n\t"
        : "=a" (*(uint32_t*)&tsc_barrier2), "=d" (*((uint32_t*)&tsc_barrier2 + 1))
    );
    
    if (tsc_barrier2 <= tsc_barrier1) {
        printf("Error: TSC after barrier should increment. First: %lu, Second: %lu\n", tsc_barrier1, tsc_barrier2);
        errors++;
    } else {
        printf("RDTSC with memory barrier: PASS\n");
    }
    
    // Test in different contexts
    uint64_t tsc_values[3];
    for (int i = 0; i < 3; i++) {
        __asm__ __volatile__ (
            "rdtsc" 
            : "=a" (*(uint32_t*)&tsc_values[i]), "=d" (*((uint32_t*)&tsc_values[i] + 1))
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
    
    printf("\nRDTSC test completed\n");
    printf("====================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
