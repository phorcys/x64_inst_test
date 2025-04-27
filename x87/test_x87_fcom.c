#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Register operand test cases
static const double reg_test_cases[][2] = {
    {1.0, 1.0},        // Equal values
    {1.0, 2.0},        // First less than second
    {2.0, 1.0},        // First greater than second 
    {0.0, -0.0},       // Positive and negative zero
    {POS_INF, POS_INF}, // Positive infinities
    {POS_INF, NEG_INF}, // Positive vs negative infinity
    {POS_INF, 1.0},     // Infinity vs finite
    {PI, E},            // PI vs E
    {POS_NAN, POS_NAN}, // NaN vs NaN
    {POS_NAN, 1.0},     // NaN vs number
    {POS_DENORM, POS_DENORM}, // Denormal vs denormal
    {POS_DENORM, 0.0},        // Denormal vs zero
    {0.0, POS_DENORM},        // Zero vs denormal
    {POS_DENORM, 1.0},        // Denormal vs normal
    {1.0, POS_DENORM}         // Normal vs denormal
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

void test_rounding_modes() {
    printf("\n=== Testing FCOM with different rounding modes ===\n");
    
    static const uint16_t rounding_modes[] = {
        X87_CW_RC_NEAR,
        X87_CW_RC_DOWN,
        X87_CW_RC_UP,
        X87_CW_RC_ZERO
    };
    
    static const char* mode_names[] = {
        "Round to nearest",
        "Round down",
        "Round up",
        "Round toward zero"
    };
    
    double test_values[][2] = {
        {1.1, 1.0},
        {1.5, 1.0},
        {1.9, 1.0},
        {-1.1, -1.0},
        {-1.5, -1.0},
        {-1.9, -1.0}
    };
    
    for (size_t i = 0; i < sizeof(rounding_modes)/sizeof(rounding_modes[0]); i++) {
        uint16_t cw = X87_CW_PC_DOUBLE | rounding_modes[i];
        asm volatile ("fldcw %0" : : "m" (cw));
        
        printf("\nRounding mode: %s\n", mode_names[i]);
        
        for (size_t j = 0; j < sizeof(test_values)/sizeof(test_values[0]); j++) {
            double a = test_values[j][0];
            double b = test_values[j][1];
            
            printf("Comparing %.20g vs %.20g\n", a, b);
            
            asm volatile (
                "fldl %1\n\t"
                "fldl %0\n\t"
                "fcom %%st(1)\n\t"
                "fnop\n\t"
                :
                : "m" (a), "m" (b)
                : "st", "st(1)"
            );
            
            print_x87_status();
            asm volatile ("fnclex\n\t");
            asm volatile ("ffree %%st(0)\n\t" "ffree %%st(1)\n\t" ::: "st", "st(1)");
        }
    }
}

void test_reg_operands() {
    printf("\n=== Testing FCOM with register operands ===\n");
    
    // Set double precision
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    // Test with ST(1) register
    printf("\n--- Testing with ST(0) vs ST(1) ---\n");
    double val1 = 1.0;
    double val2 = 2.0;
    asm volatile ("fldl %1\n\t"
                 "fldl %0\n\t"
                 "fcom %%st(1)\n\t"
                 : 
                 : "m" (val1), "m" (val2)
                 : "st", "st(1)");
    print_x87_status();
    asm volatile ("fnclex");
    asm volatile ("ffree %%st(0)\n\t" "ffree %%st(1)\n\t" ::: "st", "st(1)");
    
    for (size_t i = 0; i < sizeof(reg_test_cases)/sizeof(reg_test_cases[0]); i++) {
        double a = reg_test_cases[i][0];
        double b = reg_test_cases[i][1];
        
        printf("\nTest %zu: %.20g vs %.20g\n", i+1, a, b);
        
        if (isnan(a) || isnan(b)) {
            uint16_t old_cw, new_cw;
            asm volatile ("fstcw %0" : "=m" (old_cw));
            new_cw = old_cw | 0x3F; // Mask all exceptions
            asm volatile ("fldcw %0" : : "m" (new_cw));
            
            asm volatile (
                "fldl %1\n\t"      // Load first value
                "fldl %0\n\t"      // Load second value
                "fcom %%st(1)\n\t" // Compare st(0) and st(1)
                "fnop\n\t"
                :
                : "m" (a), "m" (b)
                : "st", "st(1)"
            );
            
            asm volatile ("fldcw %0" : : "m" (old_cw)); // Restore control word
        } else {
            asm volatile (
                "fldl %1\n\t"      // Load first value
                "fldl %0\n\t"      // Load second value
                "fcom %%st(1)\n\t" // Compare st(0) and st(1)
                "fnop\n\t"
                :
                : "m" (a), "m" (b)
                : "st", "st(1)"
            );
        }
        
        print_x87_status();
        asm volatile ("fnclex\n\t");  // Clear exceptions after printing status
        asm volatile ("ffree %%st(0)\n\t" "ffree %%st(1)\n\t" ::: "st", "st(1)");
    }
}

void test_mem32_operands() {
    printf("\n=== Testing FCOM with 32-bit memory operands ===\n");
    
    // Set single precision
    uint16_t cw = X87_CW_PC_SINGLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(mem32_test_cases)/sizeof(mem32_test_cases[0]); i++) {
        float a = mem32_test_cases[i][0];
        float b = mem32_test_cases[i][1];
        
        printf("\nTest %zu: %.10g vs %.10g\n", i+1, a, b);
        
        asm volatile (
            "flds %0\n\t"      // Load first value
            "fcoms %1\n\t"     // Compare st(0) with m32
            "fnop\n\t"
            :
            : "m" (a), "m" (b)
            : "st"
        );
        
        print_x87_status();
        asm volatile ("fnclex\n\t");  // Clear exceptions after printing status
        asm volatile ("ffree %%st(0)\n\t" ::: "st");
    }
}

void test_mem64_operands() {
    printf("\n=== Testing FCOM with 64-bit memory operands ===\n");
    
    // Set double precision
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(mem64_test_cases)/sizeof(mem64_test_cases[0]); i++) {
        double a = mem64_test_cases[i][0];
        double b = mem64_test_cases[i][1];
        
        printf("\nTest %zu: %.20g vs %.20g\n", i+1, a, b);
        
        asm volatile (
            "fldl %0\n\t"      // Load first value
            "fcoml %1\n\t"     // Compare st(0) with m64
            "fnop\n\t"
            :
            : "m" (a), "m" (b)
            : "st"
        );
        
        print_x87_status();
        asm volatile ("fnclex\n\t");  // Clear exceptions after printing status
        asm volatile ("ffree %%st(0)\n\t" ::: "st");
    }
}

int main(void) {
    init_x87_env();
    
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR | 0x3F;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    printf("Testing FCOM instruction\n");
    
    test_rounding_modes();
    test_reg_operands();
    test_mem32_operands();
    test_mem64_operands();
    
    return 0;
}
