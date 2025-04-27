#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Test cases for FCOMP3alias (compare ST(0) with ST(3) and pop)
static const struct {
    long double a;
    long double b;
    const char *desc;
    int expected_c0;
    int expected_c2;
    int expected_c3;
} test_cases[] = {
    {POS_ZERO, POS_ZERO,    "0 == 0", 0, 0, 1},
    {POS_ZERO, NEG_ZERO,    "+0 == -0", 0, 0, 1},
    {POS_ONE, POS_ONE,      "1 == 1", 0, 0, 1},
    {POS_ONE, NEG_ONE,      "1 > -1", 0, 0, 0},
    {NEG_ONE, POS_ONE,      "-1 < 1", 1, 0, 0},
    {POS_INF, POS_INF,      "+INF == +INF", 0, 0, 1},
    {POS_INF, NEG_INF,      "+INF > -INF", 0, 0, 0},
    {NEG_INF, POS_INF,      "-INF < +INF", 1, 0, 0},
    {POS_NAN, POS_NAN,      "NAN unordered", 1, 1, 1},
    {POS_NAN, POS_ONE,      "NAN unordered", 1, 1, 1},
    {POS_DENORM, POS_DENORM,"denormal == denormal", 0, 0, 1},
    {PI, E,                 "π > e", 0, 0, 0},
    {E, PI,                 "e < π", 1, 0, 0},
    {1e100L, 1e-100L,       "large > small", 0, 0, 0},
    {1e-100L, 1e100L,       "small < large", 1, 0, 0},
    {1e300L, 1e300L,        "large == large", 0, 0, 1},
    {1e-300L, 1e-300L,      "small == small", 0, 0, 1}
};

int main(int argc, char** argv) {
    // When generating reference output, mask all exceptions
        uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR | 0x3F;
        asm volatile ("fldcw %0" : : "m" (cw));
    
    printf("Testing FCOMP3alias (ST(0) vs ST(3) with pop):\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); ++i) {
        // Initialize x87 environment for each test case
        init_x87_env();
        
        printf("\nTest case %zu: %s\n", i+1, test_cases[i].desc);
        long double a = test_cases[i].a;
        long double b = test_cases[i].b;
        
        printf("ST(0): "); print_float80(a);
        printf("ST(3): "); print_float80(b);
        
        // Load values into x87 stack (need 4 levels to get ST(3))
        // Debug print before comparison
        printf("Before comparison - Stack contents:\n");
        print_x87_stack();
        
        asm volatile (
            "fldt %1\n\t"    // load b into ST(0)
            "fldz\n\t"        // push 0 (b moves to ST(1))
            "fldz\n\t"        // push 0 (b moves to ST(2))
            "fldt %0\n\t"    // load a into ST(0), others move down
            // Debug print after loading
            "fnop\n\t"
            "fcompp\n\t"      // compare ST(0) with ST(3) and pop twice (FCOMP3alias)
            "fnop\n\t"        // for alignment
            : 
            : "m" (a), "m" (b)
            : "st", "st(1)", "st(2)", "st(3)"
        );
        
        // Debug print after comparison
        printf("After comparison:\n");
        
        // Print and verify x87 status after comparison
        print_x87_status();
        
        // Verify condition flags
        uint16_t sw = get_x87_sw();
        int c0 = (sw >> 8) & 1;
        int c2 = (sw >> 10) & 1;
        int c3 = (sw >> 14) & 1;
        
        if (c0 != test_cases[i].expected_c0 || 
            c2 != test_cases[i].expected_c2 || 
            c3 != test_cases[i].expected_c3) {
            printf("ERROR: Condition flags mismatch! Expected C0=%d C2=%d C3=%d, got C0=%d C2=%d C3=%d\n",
                  test_cases[i].expected_c0, test_cases[i].expected_c2, test_cases[i].expected_c3,
                  c0, c2, c3);
            return 1;
        }
        
        // Clear x87 stack
        asm volatile (
            "ffree %%st(0)\n\t" 
            "ffree %%st(1)\n\t" 
            ::: "st", "st(1)"
        );
    }
    
    return 0;
}
