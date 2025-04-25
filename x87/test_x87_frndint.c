#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <fenv.h>

void test_frndint() {
    struct {
        long double val;
        const char* desc;
    } tests[] = {
        {3.3L, "Positive fraction"},
        {-3.3L, "Negative fraction"},
        {3.5L, "Positive half"},
        {-3.5L, "Negative half"},
        {3.7L, "Positive fraction > 0.5"},
        {-3.7L, "Negative fraction < -0.5"},
        {0.0L, "Zero"},
        {POS_INF, "Positive infinity"},
        {NEG_INF, "Negative infinity"},
        {POS_NAN, "Positive NaN"},
        {NEG_NAN, "Negative NaN"},
        {DBL_MAX, "Max double"},
        {DBL_MIN, "Min double"}
    };

    const int rounding_modes[] = {
        FE_TONEAREST,
        FE_DOWNWARD,
        FE_UPWARD,
        FE_TOWARDZERO
    };
    const char* mode_names[] = {
        "Round to nearest",
        "Round down",
        "Round up",
        "Round toward zero"
    };

    printf("=== Testing FRNDINT ===\n");
    
    for (int m = 0; m < 4; m++) {
        fesetround(rounding_modes[m]);
        printf("\nRounding mode: %s\n", mode_names[m]);
        
        for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
            long double result;
            
            printf("\nTest %zu: %s\n", i+1, tests[i].desc);
            printf("Input: ");
            print_float80(tests[i].val);
            
            asm volatile (
                "fldt %1\n\t"      // Load value to st(0)
                "frndint\n\t"      // Round to integer
                "fstpt %0\n\t"     // Store result
                : "=m" (result)
                : "m" (tests[i].val)
                : "st"
            );

            printf("Result: ");
            print_float80(result);
            print_x87_status();
        }
    }
}

int main() {
    test_frndint();
    return 0;
}
