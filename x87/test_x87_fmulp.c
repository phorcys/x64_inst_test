#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>

void test_fmulp() {
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {2.0L, 3.0L, "Normal multiplication"},
        {0.5L, 0.25L, "Fraction multiplication"},
        {0.0L, 1.0L, "Multiply by zero"},
        {POS_INF, 2.0L, "Infinity multiplication"},
        {POS_NAN, 2.0L, "NaN multiplication"},
        {DBL_MAX, 2.0L, "Overflow test"},
        {DBL_MIN, 0.5L, "Underflow test"}
    };

    printf("=== Testing FMULP ===\n");
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf * %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fldt %1\n\t"      // Load a to st(0)
            "fldt %2\n\t"      // Load b to st(0), a moves to st(1)
            "fmulp\n\t"        // st(1) = st(1) * st(0), then pop
            "fstpt %0\n\t"     // Store result
            : "=m" (result)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "st"
        );

        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

int main() {
    test_fmulp();
    return 0;
}
