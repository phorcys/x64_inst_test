#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Test cases for FCOMP3alias (compare ST(0) with ST(3) and pop)
static const long double test_cases[][2] = {
    {POS_ZERO, POS_ZERO},    // 0 vs 0
    {POS_ZERO, NEG_ZERO},    // +0 vs -0
    {POS_ONE, POS_ONE},      // 1 vs 1
    {POS_ONE, NEG_ONE},      // 1 vs -1
    {POS_INF, POS_INF},      // +INF vs +INF
    {POS_INF, NEG_INF},      // +INF vs -INF
    {POS_NAN, POS_NAN},      // NAN vs NAN
    {POS_NAN, POS_ONE},      // NAN vs 1
    {POS_DENORM, POS_DENORM},// denormal vs denormal
    {PI, E},                 // π vs e
    {1e100L, 1e-100L},       // large vs small
    {1e-100L, 1e100L}        // small vs large
};

int main() {
    printf("Testing FCOMP3alias (ST(0) vs ST(3) with pop):\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); ++i) {
        long double a = test_cases[i][0];
        long double b = test_cases[i][1];
        
        printf("\nTest case %zu:\n", i+1);
        printf("ST(0): "); print_float80(a);
        printf("ST(3): "); print_float80(b);
        
        // Load values into x87 stack (need 4 levels to get ST(3))
        asm volatile (
            "fldt %1\n\t"    // load b into ST(0)
            "fldz\n\t"        // push 0 (b moves to ST(1))
            "fldz\n\t"        // push 0 (b moves to ST(2))
            "fldt %0\n\t"    // load a into ST(0), others move down
            "fcompp\n\t"      // compare ST(0) with ST(3) and pop twice (FCOMP3alias)
            "fnop\n\t"        // for alignment
            : 
            : "m" (a), "m" (b)
            : "st", "st(1)", "st(2)", "st(3)"
        );
        
        // Print x87 status after comparison
        print_x87_status();
        
        // Clear x87 stack
        asm volatile (
            "ffree %%st(0)\n\t" 
            "ffree %%st(1)\n\t" 
            ::: "st", "st(1)"
        );
    }
    
    return 0;
}
