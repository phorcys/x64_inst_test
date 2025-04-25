#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void test_fsubrp() {
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {5.0L, 3.0L, "Basic reverse subtraction with pop: st(1) = st(0) - st(1)"},
        {3.0L, 5.0L, "Negative result with pop: st(1) = st(0) - st(1)"},
        {0.0L, 0.0L, "Zero - Zero with pop"},
        {POS_INF, 1.0L, "Finite - Infinity with pop"},
        {1.0L, POS_INF, "Infinity - Finite with pop"},
        {POS_INF, POS_INF, "Infinity - Infinity with pop"},
        {NEG_INF, NEG_INF, "-Infinity - -Infinity with pop"},
        {POS_NAN, 1.0L, "Finite - NaN with pop"},
        {1.0L, POS_NAN, "NaN - Finite with pop"},
        {DBL_MAX, -DBL_MAX, "-Max - Max with pop"},
        {DBL_MIN, DBL_MIN, "Min - Min with pop"},
        {1.0L, 0.0000000000000001L, "Small difference with pop"}
    };

    printf("=== Testing FSUBRP ===\n");
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operand A: ");
        print_float80(tests[i].a);
        printf("Operand B: ");
        print_float80(tests[i].b);

        asm volatile (
            "fldt %2\n\t"      // Load a to st(0)
            "fldt %1\n\t"      // Load b to st(0), a moves to st(1)
            "fsubrp %%st, %%st(1)\n\t"  // st(1) = st(0) - st(1) and pop
            "fstpt %0\n\t"     // Store result (now in st(0))
            : "=m" (result)
            : "m" (tests[i].b), "m" (tests[i].a)
            : "st"
        );

        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

int main() {
    test_fsubrp();
    return 0;
}
