#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("STD instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Clear direction flag
    __asm__ __volatile__ ("cld");
    
    // Test STD sets direction flag
    uint8_t df_before = 0;
    uint8_t df_after = 0;
    
    // Get DF before STD
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "and $0x400, %%eax\n\t"  // DF is bit 10 (0x400)
        "shr $10, %%eax\n\t"     // Shift right to get 0 or 1
        "mov %%al, %0\n\t"
        : "=r" (df_before)
        : 
        : "rax", "cc"
    );
    
    // Execute STD
    __asm__ __volatile__ ("std");
    
    // Get DF after STD
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "and $0x400, %%eax\n\t"
        "shr $10, %%eax\n\t"
        "mov %%al, %0\n\t"
        : "=r" (df_after)
        : 
        : "rax", "cc"
    );
    
    printf("DF before STD: %d\n", df_before);
    printf("DF after STD: %d\n", df_after);
    
    if (df_before != 0) {
        printf("Error: DF was set before STD\n");
        errors++;
    }
    
    if (df_after != 1) {
        printf("Error: DF not set after STD\n");
        errors++;
    }
    
    // Test STD doesn't affect other flags
    uint64_t flags_before = 0;
    uint64_t flags_after = 0;
    
    // Get flags before STD
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (flags_before)
        : 
        : "rax", "cc"
    );
    
    // Execute STD
    __asm__ __volatile__ ("std");
    
    // Get flags after STD
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (flags_after)
        : 
        : "rax", "cc"
    );
    
    // Mask out DF bit (bit 10) for comparison
    uint64_t flags_mask = ~(1ULL << 10);
    uint64_t flags_before_masked = flags_before & flags_mask;
    uint64_t flags_after_masked = flags_after & flags_mask;
    
    if (flags_before_masked != flags_after_masked) {
        printf("Error: Other flags changed by STD\n");
        printf("Before: 0x%016lX\n", flags_before_masked);
        printf("After:  0x%016lX\n", flags_after_masked);
        errors++;
    }
    
    // Clear direction flag for any subsequent code
    __asm__ __volatile__ ("cld");
    
    printf("\nSTD test completed\n");
    printf("==================\n");
    printf("Total tests: 3\n");
    printf("Passed: %d\n", 3 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
