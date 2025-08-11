#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("STC instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Clear flags
    __asm__ __volatile__ ("pushf\n\t"
                          "pop %%rax\n\t"
                          "and $0xFFFE, %%ax\n\t"  // Clear CF
                          "push %%rax\n\t"
                          "popf\n\t"
                          : : : "rax", "cc");
    
    // Test STC sets carry flag
    uint8_t cf_before = 0;
    uint8_t cf_after = 0;
    
    // Get CF before STC
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "and $1, %%al\n\t"
        "mov %%al, %0\n\t"
        : "=r" (cf_before)
        : 
        : "rax", "cc"
    );
    
    // Execute STC
    __asm__ __volatile__ ("stc");
    
    // Get CF after STC
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "and $1, %%al\n\t"
        "mov %%al, %0\n\t"
        : "=r" (cf_after)
        : 
        : "rax", "cc"
    );
    
    printf("CF before STC: %d\n", cf_before);
    printf("CF after STC: %d\n", cf_after);
    
    if (cf_before != 0) {
        printf("Error: CF was set before STC\n");
        errors++;
    }
    
    if (cf_after != 1) {
        printf("Error: CF not set after STC\n");
        errors++;
    }
    
    // Test STC doesn't affect other flags
    uint64_t flags_before = 0;
    uint64_t flags_after = 0;
    
    // Get flags before STC
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (flags_before)
        : 
        : "rax", "cc"
    );
    
    // Execute STC
    __asm__ __volatile__ ("stc");
    
    // Get flags after STC
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (flags_after)
        : 
        : "rax", "cc"
    );
    
    // Mask out CF bit (bit 0) for comparison
    uint64_t flags_mask = ~(1ULL);
    uint64_t flags_before_masked = flags_before & flags_mask;
    uint64_t flags_after_masked = flags_after & flags_mask;
    
    if (flags_before_masked != flags_after_masked) {
        printf("Error: Other flags changed by STC\n");
        printf("Before: 0x%016lX\n", flags_before_masked);
        printf("After:  0x%016lX\n", flags_after_masked);
        errors++;
    }
    
    printf("\nSTC test completed\n");
    printf("==================\n");
    printf("Total tests: 3\n");
    printf("Passed: %d\n", 3 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
