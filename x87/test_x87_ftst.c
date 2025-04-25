#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void test_ftst() {
    struct {
        long double val;
        const char* desc;
    } tests[] = {
        {0.0L, "Zero"},
        {1.0L, "Positive"},
        {-1.0L, "Negative"},
        {POS_INF, "Positive Infinity"},
        {NEG_INF, "Negative Infinity"},
        {POS_NAN, "Positive NaN"},
        {NEG_NAN, "Negative NaN"},
        {DBL_MAX, "Max double"},
        {-DBL_MAX, "Min double"},
        {DBL_MIN, "Min positive normal"},
        {POS_DENORM, "Positive denormal"},
        {NEG_DENORM, "Negative denormal"}
    };

    printf("=== Testing FTST ===\n");
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Value: ");
        print_float80(tests[i].val);

        uint16_t status;
        asm volatile (
            "fldt %1\n\t"      // Load value to st(0)
            "ftst\n\t"         // Compare st(0) with 0.0
            "fnstsw %0\n\t"    // Store status word
            : "=a" (status)
            : "m" (tests[i].val)
            : "st"
        );

        print_x87_status();
    }
}

int main() {
    test_ftst();
    return 0;
}
