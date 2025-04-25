#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test FCOMP instruction
void test_fcomp() {
    uint16_t status = 0;
    int stack_depth = 0;
    
    printf("=== Testing FCOMP ===\n");
    
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
        // Get initial stack depth
        asm volatile ("fnstsw %0" : "=m" (status));
        stack_depth = (status >> 11) & 0x7;
        
        asm volatile (
            "fldt %1\n\t"      // Load first value
            "fldt %2\n\t"      // Load second value
            "fcomp %%st(1)\n\t" // Compare and pop
            "fstsw %0\n\t"     // Store status word
            : "=m" (status)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "cc"
        );
        
        // Get new stack depth
        uint16_t new_status = 0;
        asm volatile ("fnstsw %0" : "=m" (new_status));
        int new_stack_depth = (new_status >> 11) & 0x7;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Values: %Lf vs %Lf\n", tests[i].a, tests[i].b);
        printf("Stack depth before: %d, after: %d\n", stack_depth, new_stack_depth);
        printf("Status Word: 0x%04x\n", status);
        printf("C0 (Unordered): %d\n", (status >> 8) & 1);
        printf("C2 (Equal): %d\n", (status >> 14) & 1);
        printf("C3 (Greater): %d\n", (status >> 0) & 1);
        print_x87_status();
    }
}

int main() {
    test_fcomp();
    return 0;
}
