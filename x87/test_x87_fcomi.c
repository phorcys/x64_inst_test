#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test FCOMI instruction
void test_fcomi() {
    uint64_t eflags = 0;
    
    printf("=== Testing FCOMI ===\n");
    
    // Test cases
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {1.0L, 1.0L, "Equal values"},
        {1.0L, 2.0L, "First less than second"}, 
        {2.0L, 1.0L, "First greater than second"},
        {0.0L, -0.0L, "Positive and negative zero"},
        {POS_INF, POS_INF, "Positive infinities"},
        {POS_INF, NEG_INF, "Positive vs negative infinity"},
        {POS_INF, 1.0L, "Infinity vs finite"},
        {POS_NAN, POS_NAN, "NaN values"},
        {POS_NAN, 1.0L, "NaN vs finite"},
        {POS_DENORM, POS_DENORM, "Denormal values"},
        {PI, E, "PI vs E"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        asm volatile (
            "fldt %1\n\t"      // Load first value
            "fldt %2\n\t"      // Load second value
            "fcomi %%st(1)\n\t" // Compare st(0) and st(1)
            "pushf\n\t"        // Store EFLAGS
            "popq %0\n\t"      // Pop to eflags (64-bit)
            "fstp %%st(0)\n\t" // Pop stack
            "fstp %%st(0)\n\t" // Pop stack
            : "=r" (eflags)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "cc"
        );
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Values: %Lf vs %Lf\n", tests[i].a, tests[i].b);
        printf("EFLAGS: 0x%08lx\n", eflags);
        printf("ZF: %ld\n", (eflags >> 6) & 1);
        printf("PF: %ld\n", (eflags >> 2) & 1);
        printf("CF: %ld\n", (eflags >> 0) & 1);
        print_x87_status();
    }
}

int main() {
    test_fcomi();
    return 0;
}
