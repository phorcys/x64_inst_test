#include "x87.h"
#include <stdio.h>
#include <stdint.h>

void test_fdivr() {
    long double result = 0;
    uint16_t status = 0;
    
    printf("=== Testing FDIVR ===\n");
    
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {8.0L, 2.0L, "Normal reverse division"},
        {1.0L, 0.0L, "Divide zero by value"}, 
        {0.0L, 0.0L, "Zero divided by zero"},
        {POS_INF, 2.0L, "Infinity divided"},
        {2.0L, POS_INF, "Divide by infinity"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        asm volatile (
            "fldt %2\n\t"      // Load b (divisor)
            "fldt %1\n\t"      // Load a (dividend)
            "fdivr %%st, %%st(1)\n\t" // st(1) = st(0)/st(1)
            "fstpt %0\n\t"     // Store result
            "fnstsw %3\n\t"    // Store status
            "fstp %%st(0)\n\t" // Pop stack
            : "=m" (result), "=m" (status)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "cc"
        );
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %Lf (reversed)\n", tests[i].a, tests[i].b);
        printf("Result: %Lf\n", result);
        printf("Status Word: 0x%04x\n", status);
        print_x87_status();
    }
}

int main() {
    printf("NOTE: FDIVR test needs further debugging\n");
    test_fdivr();
    return 0;
}
