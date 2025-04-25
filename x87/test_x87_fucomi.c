#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void test_fucomi() {
    struct {
        long double val1;
        long double val2;
        const char* desc;
    } tests[] = {
        {0.0L, 0.0L, "Zero vs Zero"},
        {1.0L, 1.0L, "Equal positive"}, 
        {-1.0L, -1.0L, "Equal negative"},
        {1.0L, 2.0L, "Positive less than"},
        {2.0L, 1.0L, "Positive greater than"},
        {-1.0L, -2.0L, "Negative greater than"},
        {-2.0L, -1.0L, "Negative less than"},
        {POS_INF, POS_INF, "Infinity equal"},
        {NEG_INF, NEG_INF, "Negative infinity equal"},
        {POS_INF, 1.0L, "Infinity greater"},
        {1.0L, POS_INF, "Finite less than infinity"},
        {POS_NAN, 1.0L, "NaN unordered"},
        {1.0L, POS_NAN, "Finite vs NaN unordered"},
        {POS_NAN, POS_NAN, "NaN vs NaN unordered"},
        {DBL_MAX, DBL_MIN, "Max vs min"},
        {POS_DENORM, POS_DENORM, "Denormals equal"}
    };

    printf("=== Testing FUCOMI ===\n");

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Value1: ");
        print_float80(tests[i].val1);
        printf("Value2: ");
        print_float80(tests[i].val2);

        uint32_t eflags;
        asm volatile (
            "fldt %1\n\t"      // Load val1 to st(0)
            "fldt %2\n\t"      // Load val2 to st(1)
            "fucomi %%st(1)\n\t" // Compare st(0) with st(1)
            "mov $0, %0\n\t"   // Clear eflags
            "lahf\n\t"         // Load flags into AH
            "seto %%al\n\t"    // Set OF flag
            "mov %%eax, %0"    // Store to eflags
            : "=r" (eflags)
            : "m" (tests[i].val1), "m" (tests[i].val2)
            : "st", "st(1)", "cc", "eax"
        );

        print_x87_status();
        printf("EFLAGS: 0x%08x\n", eflags);
        printf("CF: %d, PF: %d, ZF: %d\n",
               (eflags >> 0) & 1,  // CF
               (eflags >> 2) & 1,  // PF
               (eflags >> 6) & 1); // ZF
    }
}

int main() {
    test_fucomi();
    return 0;
}
