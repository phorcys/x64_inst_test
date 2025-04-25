#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Basic FDIV tests
void test_fdiv_basic() {
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {4.0L, 2.0L, "Normal division"},
        {1.0L, 3.0L, "Fraction division"},
        {1.0L, 0.0L, "Divide by zero"},
        {POS_INF, 2.0L, "Infinity divided"},
        {2.0L, POS_INF, "Divide by infinity"},
        {POS_NAN, 2.0L, "NaN divided"},
        {2.0L, POS_NAN, "Divide by NaN"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fldt %2\n\t"      // Load b (divisor)
            "fldt %1\n\t"      // Load a (dividend)
            "fdivp %%st, %%st(1)\n\t" // st(1) = st(1)/st(0) and pop
            "fstpt %0\n\t"     // Store result
            : "=m" (result)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "st", "st(1)"
        );
        
        printf("Result: %Lf\n", result);
        print_x87_status();
    }
}

// Test FDIV instruction
void test_fdiv() {
    printf("=== Testing FDIV ===\n");
    test_fdiv_basic();
}

int main() {
    printf("NOTE: FDIV test needs further debugging\n");
    test_fdiv();
    return 0;
}
