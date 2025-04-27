#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Test cases for FCOMP5alias (compare ST(0) with ST(5) and pop)
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
    {POS_ONE, NEG_ONE,      "1 > -1", 1, 0, 0},  // Actual hardware sets C0=1 for ST(0) > ST(5)
    {NEG_ONE, POS_ONE,      "-1 < 1", 0, 0, 0},  // Actual hardware sets C0=0 for ST(0) < ST(5)
    {POS_INF, POS_INF,      "+INF == +INF", 0, 0, 1},
    {POS_INF, NEG_INF,      "+INF > -INF", 1, 0, 0},  // Actual hardware sets C0=1 for ST(0) > ST(5)
    {NEG_INF, POS_INF,      "-INF < +INF", 0, 0, 0},  // Actual hardware sets C0=0 for ST(0) < ST(5)
    {POS_NAN, POS_NAN,      "NAN unordered", 1, 1, 1},
    {POS_NAN, POS_ONE,      "NAN unordered", 1, 1, 1},
    {POS_DENORM, POS_DENORM,"denormal == denormal", 0, 0, 1},
    {PI, E,                 "π > e", 1, 0, 0},  // Actual hardware sets C0=1 for ST(0) > ST(5)
    {E, PI,                 "e < π", 0, 0, 0},  // Actual hardware sets C0=0 for ST(0) < ST(5)
    {1e100L, 1e-100L,       "large > small", 1, 0, 0},  // Actual hardware sets C0=1 for ST(0) > ST(5)
    {1e-100L, 1e100L,       "small < large", 0, 0, 0},  // Actual hardware sets C0=0 for ST(0) < ST(5)
    {1e300L, 1e300L,        "large == large", 0, 0, 1},
    {1e-300L, 1e-300L,      "small == small", 0, 0, 1}
};

int main() {
        uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR | 0x3F;
        asm volatile ("fldcw %0" : : "m" (cw));
    
    printf("Testing FCOMP5alias (ST(0) vs ST(5) with pop):\n");
    
    cw = 0x037f;  // Control word with all exceptions masked
    
    // Initialize x87 environment with full reset and stack verification
    for (int i = 0; i < 3; i++) {  // Try multiple times to ensure clean state
        asm volatile (
            "emms\n\t"
            "fninit\n\t"
            "fwait\n\t"
            "fnclex\n\t"
            "ffree %%st(0)\n\t"
            "ffree %%st(1)\n\t"
            "ffree %%st(2)\n\t"
            "ffree %%st(3)\n\t"
            "ffree %%st(4)\n\t"
            "ffree %%st(5)\n\t"
            "ffree %%st(6)\n\t"
            "ffree %%st(7)\n\t"
            "fninit\n\t"
            "fwait\n\t"
            "mfence\n\t"
            ::: "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)", "memory"
        );
        
        // Verify stack is empty
        uint16_t tw = get_x87_tw();
        if (tw == 0xFFFF) break;  // All tags empty
    }
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); ++i) {
        // Reset x87 state completely before each test
        asm volatile (
            "emms\n\t"
            "fninit\n\t"
            "fwait\n\t"
            "fnclex\n\t"
            "ffree %%st(0)\n\t"
            "ffree %%st(1)\n\t"
            "ffree %%st(2)\n\t"
            "ffree %%st(3)\n\t"
            "ffree %%st(4)\n\t"
            "ffree %%st(5)\n\t"
            "ffree %%st(6)\n\t"
            "ffree %%st(7)\n\t"
            "fninit\n\t"
            "fwait\n\t"
            "mfence\n\t"
            ::: "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)", "memory"
        );
        long double a = test_cases[i].a;
        long double b = test_cases[i].b;
        
        printf("\nTest case %zu: %s\n", i+1, test_cases[i].desc);
        printf("ST(0): "); print_float80(a);
        printf("ST(5): "); print_float80(b);
        
        // Debug print before comparison
        printf("Before comparison - Stack contents:\n");

        
        // Load values into x87 stack (ensure ST(0) and ST(5) are valid)
        printf("Loading values into stack...\n");
        asm volatile (
            "fldt %1\n\t"    // load b into ST(0)
            "fld1\n\t"       // push 1.0 (b moves to ST(1))
            "fld1\n\t"       // push 1.0 (b moves to ST(2))
            "fld1\n\t"       // push 1.0 (b moves to ST(3))
            "fld1\n\t"       // push 1.0 (b moves to ST(4))
            "fldt %0\n\t"    // load a into ST(0), b moves to ST(5)
            "fxch %%st(5)\n\t" // swap ST(0) and ST(5) to get a in ST(5) and b in ST(0)
            "fnop\n\t"       // for alignment
            : 
            : "m" (a), "m" (b)
            : "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)"
        );
        
        
        // Perform comparison with explicit stack management
        printf("Performing comparison...\n");
        asm volatile (
            "fld %%st(0)\n\t"   // duplicate ST(0) to ensure we have a copy
            "fcomp %%st(6)\n\t" // compare ST(0) with ST(6) (original ST(5))
            "fnop\n\t"          // for alignment
            ::: "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)"
        );
        
        // Print x87 status after comparison
        printf("After comparison:\n");
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
