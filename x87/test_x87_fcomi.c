#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <float.h>

// Register operand test cases
static const double reg_test_cases[][2] = {
    {1.0, 1.0},    // Equal values
    {1.0, 2.0},    // First less than second
    {2.0, 1.0},    // First greater than second 
    {0.0, -0.0},   // Positive and negative zero
    {POS_INF, POS_INF}, // Positive infinities
    {POS_INF, NEG_INF}, // Positive vs negative infinity
    {POS_INF, 1.0},     // Infinity vs finite
    {PI, E},            // PI vs E
    {POS_NAN, POS_NAN}, // NaN values
    {POS_NAN, 1.0},     // NaN vs finite
    {POS_DENORM, POS_DENORM}, // Denormal values
    {LDBL_MAX, LDBL_MAX},     // Max normal values
    {LDBL_MIN, LDBL_MIN}      // Min normal values
};

// 32-bit memory operand test cases
static const float mem32_test_cases[][2] = {
    {1.5f, 1.5f},
    {1.5f, 2.5f},
    {2.5f, 1.5f},
    {0.0f, -0.0f},
    {INFINITY, INFINITY},
    {INFINITY, -INFINITY},
    {INFINITY, 1.0f}
};

// 64-bit memory operand test cases
static const double mem64_test_cases[][2] = {
    {1.5, 1.5},
    {1.5, 2.5},
    {2.5, 1.5},
    {0.0, -0.0},
    {POS_INF, POS_INF},
    {POS_INF, NEG_INF},
    {POS_INF, 1.0}
};

void test_reg_operands() {
    uint64_t eflags = 0;
    
    printf("\n=== Testing FCOMI with register operands ===\n");
    
    // Set double precision
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(reg_test_cases)/sizeof(reg_test_cases[0]); i++) {
        double a = reg_test_cases[i][0];
        double b = reg_test_cases[i][1];
        
        printf("\nTest %zu: %.20g vs %.20g\n", i+1, a, b);
        
        if (isnan(a) || isnan(b)) {
            // Mask exceptions for NaN tests
            uint16_t old_cw = get_x87_cw();
            uint16_t new_cw = old_cw | 0x3F; // Mask all exceptions
            asm volatile ("fldcw %0" : : "m" (new_cw));
            
            // Clear any pending exceptions
            asm volatile ("fnclex");
            
            asm volatile (
                "pushf\n"          // Save EFLAGS
                "fldl %1\n"        // Load first value
                "fldl %2\n"        // Load second value
                "fcomi %%st(1)\n"  // Compare st(0) and st(1)
                "pushf\n"          // Store EFLAGS
                "pop %0\n"         // Pop to eflags
                "fstp %%st(0)\n"   // Pop stack
                "fstp %%st(0)\n"   // Pop stack
                "popf\n"           // Restore EFLAGS
                : "=r" (eflags)
                : "m" (a), "m" (b)
                : "cc", "st", "st(1)"
            );
            
            // Clear any pending exceptions from NaN comparison
            asm volatile ("fnclex");
            
            // Check and ignore expected NaN comparison exceptions
            uint16_t status = get_x87_sw();
            if (status & X87_SW_ES) {
                printf("Note: Expected FPU Exception from NaN comparison (Status: 0x%04x)\n", status);
                // Clear the exception state
                asm volatile ("fnclex");
            }
            
            asm volatile ("fldcw %0" : : "m" (old_cw)); // Restore control word
        } else {
            asm volatile (
                "pushf\n"          // Save EFLAGS
                "fldl %1\n"        // Load first value
                "fldl %2\n"        // Load second value
                "fcomi %%st(1)\n"  // Compare st(0) and st(1)
                "pushf\n"          // Store EFLAGS
                "pop %0\n"         // Pop to eflags
                "fstp %%st(0)\n"   // Pop stack
                "fstp %%st(0)\n"   // Pop stack
                "popf\n"           // Restore EFLAGS
                : "=r" (eflags)
                : "m" (a), "m" (b)
                : "cc", "st", "st(1)"
            );
        }
        
        printf("EFLAGS: 0x%08lx\n", eflags);
        printf("ZF: %ld, PF: %ld, CF: %ld\n", 
              (eflags >> 6) & 1, (eflags >> 2) & 1, (eflags >> 0) & 1);
        print_x87_status();
    }
}

void test_mem32_operands() {
    uint64_t eflags = 0;
    
    printf("\n=== Testing FCOMI with 32-bit memory operands ===\n");
    
    // Set single precision
    uint16_t cw = X87_CW_PC_SINGLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(mem32_test_cases)/sizeof(mem32_test_cases[0]); i++) {
        float a = mem32_test_cases[i][0];
        float b = mem32_test_cases[i][1];
        
        printf("\nTest %zu: %.10g vs %.10g\n", i+1, a, b);
        
        asm volatile (
            "flds %1\n"        // Load first value
            "flds %2\n"        // Load second value
            "fcomi %%st(1)\n"  // Compare st(0) and st(1)
            "pushf\n"          // Store EFLAGS
            "pop %0\n"         // Pop to eflags
            "fstp %%st(0)\n"   // Pop stack
            "fstp %%st(0)\n"   // Pop stack
            : "=r" (eflags)
            : "m" (a), "m" (b)
            : "cc", "st", "st(1)"
        );
        
        printf("EFLAGS: 0x%08lx\n", eflags);
        printf("ZF: %ld, PF: %ld, CF: %ld\n", 
              (eflags >> 6) & 1, (eflags >> 2) & 1, (eflags >> 0) & 1);
        print_x87_status();
    }
}

void test_mem64_operands() {
    uint64_t eflags = 0;
    
    printf("\n=== Testing FCOMI with 64-bit memory operands ===\n");
    
    // Set double precision
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(mem64_test_cases)/sizeof(mem64_test_cases[0]); i++) {
        double a = mem64_test_cases[i][0];
        double b = mem64_test_cases[i][1];
        
        printf("\nTest %zu: %.20g vs %.20g\n", i+1, a, b);
        
        asm volatile (
            "fldl %1\n"        // Load first value
            "fldl %2\n"        // Load second value
            "fcomi %%st(1)\n"  // Compare st(0) and st(1)
            "pushf\n"          // Store EFLAGS
            "pop %0\n"         // Pop to eflags
            "fstp %%st(0)\n"   // Pop stack
            "fstp %%st(0)\n"   // Pop stack
            : "=r" (eflags)
            : "m" (a), "m" (b)
            : "cc", "st", "st(1)"
        );
        
        printf("EFLAGS: 0x%08lx\n", eflags);
        printf("ZF: %ld, PF: %ld, CF: %ld\n", 
              (eflags >> 6) & 1, (eflags >> 2) & 1, (eflags >> 0) & 1);
        print_x87_status();
    }
}

int main() {
    init_x87_env();
    
    printf("Testing FCOMI instruction\n");
    
    // Test different rounding modes
    uint16_t old_cw = get_x87_cw();
    uint16_t modes[] = {X87_CW_RC_NEAR, X87_CW_RC_DOWN, X87_CW_RC_UP, X87_CW_RC_ZERO};
    const char* mode_names[] = {"Round to nearest", "Round down", "Round up", "Round toward zero"};
    
    for (int i = 0; i < 4; i++) {
        uint16_t new_cw = (old_cw & ~X87_CW_RC_MASK) | modes[i];
        asm volatile ("fldcw %0" : : "m" (new_cw));
        
        printf("\nTesting with rounding mode: %s\n", mode_names[i]);
        test_reg_operands();
    }
    asm volatile ("fldcw %0" : : "m" (old_cw));  // Restore original control word
    
    test_reg_operands();
    test_mem32_operands();
    test_mem64_operands();
    
    return 0;
}
