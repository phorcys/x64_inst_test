#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void test_fsubp() {
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {5.0L, 3.0L, "Basic subtraction: 5.0 - 3.0"},
        {3.0L, 5.0L, "Negative result: 3.0 - 5.0"},
        {0.0L, 0.0L, "Zero - Zero"},
        {POS_INF, 1.0L, "Infinity - Finite"},
        {1.0L, POS_INF, "Finite - Infinity"}, 
        {POS_INF, POS_INF, "Infinity - Infinity"},
        {NEG_INF, NEG_INF, "-Infinity - -Infinity"},
        {POS_NAN, 1.0L, "NaN - Finite"},
        {1.0L, POS_NAN, "Finite - NaN"},
        {DBL_MAX, -DBL_MAX, "Max - (-Max)"},
        {DBL_MIN, DBL_MIN, "Min - Min"},
        {1.0L, 0.0000000000000001L, "Small difference"}
    };

    printf("=== Testing FSUBP ===\n");
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        int stack_top_before, stack_top_after;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operand A: ");
        print_float80(tests[i].a);
        printf("Operand B: ");
        print_float80(tests[i].b);

        // Get initial stack top
        uint16_t status_before;
        asm volatile ("fnstsw %0" : "=a" (status_before));
        stack_top_before = (status_before >> 11) & 0x7;

        asm volatile (
            "fldt %1\n\t"      // Load b to st(0)
            "fldt %2\n\t"      // Load a to st(0), b moves to st(1)
            "fsubp %%st, %%st(1)\n\t"  // st(1) = st(1) - st(0); pop
            "fstpt %0\n\t"     // Store result
            : "=m" (result)
            : "m" (tests[i].b), "m" (tests[i].a)
            : "st"
        );

        // Get final stack top
        uint16_t status_after;
        asm volatile ("fnstsw %0" : "=a" (status_after));
        stack_top_after = (status_after >> 11) & 0x7;

        printf("Result: ");
        print_float80(result);
        printf("Stack top before: %d, after: %d\n", stack_top_before, stack_top_after);
        print_x87_status();
    }
}

int main() {
    test_fsubp();
    return 0;
}
