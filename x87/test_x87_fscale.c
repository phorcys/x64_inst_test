#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void test_fscale() {
    struct {
        long double x;
        long double n;
        const char* desc;
    } tests[] = {
        {1.0L, 1.0L, "Basic case: 1.0 * 2^1"},
        {2.0L, 3.0L, "Integer scaling: 2.0 * 2^3"},
        {1.5L, -1.0L, "Negative exponent: 1.5 * 2^-1"},
        {0.0L, 10.0L, "Zero base: 0.0 * 2^10"},
        {1.0L, 0.0L, "Zero exponent: 1.0 * 2^0"},
        {POS_INF, 1.0L, "Infinity base"},
        {1.0L, POS_INF, "Infinity exponent"},
        {NEG_INF, 1.0L, "Negative infinity base"},
        {1.0L, NEG_INF, "Negative infinity exponent"},
        {POS_NAN, 1.0L, "NaN base"},
        {1.0L, POS_NAN, "NaN exponent"},
        {DBL_MAX, 1.0L, "Max double scaling"},
        {DBL_MIN, -1.0L, "Min double scaling"},
        {1.0L, 1024.0L, "Large positive exponent"},
        {1.0L, -1024.0L, "Large negative exponent"}
    };

    printf("=== Testing FSCALE ===\n");
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Base: ");
        print_float80(tests[i].x);
        printf("Exponent: ");
        print_float80(tests[i].n);
        
        asm volatile (
            "fldt %1\n\t"      // Load exponent n to st(0)
            "fldt %2\n\t"      // Load base x to st(0), n moves to st(1)
            "fscale\n\t"       // st(0) = x * 2^n
            "fstpt %0\n\t"     // Store result
            "fstp %%st(0)\n\t" // Pop st(0)
            : "=m" (result)
            : "m" (tests[i].n), "m" (tests[i].x)
            : "st"
        );

        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

int main() {
    test_fscale();
    return 0;
}
