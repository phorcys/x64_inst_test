#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <float.h>

// Test FCOMP instruction
void test_fcomp() {
    // Test memory operands
    float m32fp = 1.5f;
    double m64fp = 2.5;
    uint16_t status = 0;
    int stack_depth = 0;
    
    printf("=== Testing FCOMP ===\n");
    
    // Test memory operands
    printf("\nTesting m32fp operand:\n");
    {
        long double tmp = 1.0L;
        asm volatile (
            "fldt %0\n\t"      // Load test value
            "fcomps %1\n\t"    // Compare with m32fp and pop
            "fstsw %%ax\n\t"   // Store status word
            : : "m" (tmp), "m" (m32fp)
        );
    }
    print_x87_status();

    printf("\nTesting m64fp operand:\n");
    {
        long double tmp = 1.0L;
        asm volatile (
            "fldt %0\n\t"      // Load test value
            "fcompl %1\n\t"    // Compare with m64fp and pop
            "fstsw %%ax\n\t"   // Store status word
            : : "m" (tmp), "m" (m64fp)
        );
    }
    print_x87_status();
    
    // Test cases
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {1.0L, 1.0L, "Equal values"},
        {1.0L, 2.0L, "First less than second"},
        {2.0L, 1.0L, "First greater than second"},
        {0.0L, -0.0L, "Positive and negative zero"},
        {POS_INF, POS_INF, "Positive infinities"},
        {POS_INF, NEG_INF, "Positive vs negative infinity"},
        {POS_INF, 1.0L, "Infinity vs finite"},
        {POS_NAN, POS_NAN, "NaN values"},
        {POS_NAN, 1.0L, "NaN vs finite"},
        {POS_DENORM, POS_DENORM, "Denormal values"},
        {PI, E, "PI vs E"},
        {LDBL_MAX, LDBL_MAX, "Max normal values"},
        {LDBL_MIN, LDBL_MIN, "Min normal values"},
        {LDBL_MAX, LDBL_MIN, "Max vs min normal"},
        {1.0L, POS_DENORM, "Normal vs denormal"},
        {LDBL_MAX, POS_INF, "Max normal vs infinity"},
        {POS_DENORM, 0.0L, "Denormal vs zero"},
        {POS_NAN, NEG_NAN, "Positive NaN vs negative NaN"},
        {POS_NAN, POS_INF, "NaN vs infinity"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        // Get initial stack depth
        asm volatile ("fnstsw %0" : "=m" (status));
        stack_depth = (status >> 11) & 0x7;
        
        if (isnan(tests[i].a) || isnan(tests[i].b)) {
            uint16_t old_cw, new_cw;
            asm volatile ("fstcw %0" : "=m" (old_cw));
            new_cw = old_cw | 0x3F; // Mask all exceptions
            asm volatile ("fldcw %0" : : "m" (new_cw));
            
            asm volatile (
                "fldt %1\n\t"      // Load first value
                "fldt %2\n\t"      // Load second value
                "fcomp %%st(1)\n\t" // Compare and pop
                "fstsw %0\n\t"     // Store status word
                : "=m" (status)
                : "m" (tests[i].a), "m" (tests[i].b)
                : "cc"
            );
            
            asm volatile ("fldcw %0" : : "m" (old_cw)); // Restore control word
        } else {
            asm volatile (
                "fldt %1\n\t"      // Load first value
                "fldt %2\n\t"      // Load second value
                "fcomp %%st(1)\n\t" // Compare and pop
                "fstsw %0\n\t"     // Store status word
                : "=m" (status)
                : "m" (tests[i].a), "m" (tests[i].b)
                : "cc"
            );
        }
        
        // Get new stack depth
        uint16_t new_status = 0;
        asm volatile ("fnstsw %0" : "=m" (new_status));
        int new_stack_depth = (new_status >> 11) & 0x7;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Values: %Lf vs %Lf\n", tests[i].a, tests[i].b);
        printf("Stack depth before: %d, after: %d\n", stack_depth, new_stack_depth);
        printf("Status Word: 0x%04x\n", status);
        printf("C0 (Unordered): %d\n", (status >> 8) & 1);
        printf("C2 (Equal): %d\n", (status >> 14) & 1);
        printf("C3 (Greater): %d\n", (status >> 0) & 1);
        print_x87_status();
    }
}

int main() {
    // Test different rounding modes
    uint16_t old_cw = get_x87_cw();
    uint16_t modes[] = {X87_CW_RC_NEAR, X87_CW_RC_DOWN, X87_CW_RC_UP, X87_CW_RC_ZERO};
    const char* mode_names[] = {"Round to nearest", "Round down", "Round up", "Round toward zero"};
    
    for (int i = 0; i < 4; i++) {
        uint16_t new_cw = (old_cw & ~X87_CW_RC_MASK) | modes[i];
        asm volatile ("fldcw %0" : : "m" (new_cw));
        
        printf("\nTesting with rounding mode: %s\n", mode_names[i]);
        {
            long double a = 1.0L;
            long double b = 1.0000000000000001L;
            asm volatile (
                "fldt %0\n\t"      // Load 1.0
                "fldt %1\n\t"      // Load 1.000...1
                "fcomp %%st(1)\n\t" // Compare and pop
                "fstsw %%ax\n\t"   // Store status word
                : : "m" (a), "m" (b)
            );
        }
        print_x87_status();
    }
    asm volatile ("fldcw %0" : : "m" (old_cw));  // Restore original control word

    test_fcomp();
    return 0;
}
