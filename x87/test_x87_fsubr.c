#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void test_fsubr() {
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {5.0L, 3.0L, "Basic reverse subtraction: 3.0 - 5.0"},
        {3.0L, 5.0L, "Positive result: 5.0 - 3.0"},
        {0.0L, 0.0L, "Zero - Zero"},
        {POS_INF, 1.0L, "Finite - Infinity"},
        {1.0L, POS_INF, "Infinity - Finite"},
        {POS_INF, POS_INF, "Infinity - Infinity"},
        {NEG_INF, NEG_INF, "-Infinity - -Infinity"},
        {POS_NAN, 1.0L, "Finite - NaN"},
        {1.0L, POS_NAN, "NaN - Finite"},
        {DBL_MAX, -DBL_MAX, "-Max - Max"},
        {DBL_MIN, DBL_MIN, "Min - Min"},
        {1.0L, 0.0000000000000001L, "Small difference"}
    };

    printf("=== Testing FSUBR ===\n");
    
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
    test_fsubr();
    return 0;
}
