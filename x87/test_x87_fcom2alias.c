#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Test cases for register operands
static const double reg_test_cases[][2] = {
    {POS_ZERO, POS_ZERO},    // 0 vs 0
    {POS_ZERO, NEG_ZERO},    // +0 vs -0 
    {POS_ONE, POS_ONE},      // 1 vs 1
    {POS_ONE, NEG_ONE},      // 1 vs -1
    {POS_INF, POS_INF},      // +INF vs +INF
    {POS_INF, NEG_INF},      // +INF vs -INF
    {POS_NAN, POS_NAN},      // NAN vs NAN
    {POS_NAN, POS_ONE},      // NAN vs 1
    {PI, E},                 // π vs e
    {1e100, 1e-100},         // large vs small
    {1e-100, 1e100}          // small vs large
};

// Test cases for 32-bit memory operands
static const float mem32_test_cases[][2] = {
    {1.5f, 1.5f},
    {1.5f, -1.5f},
    {1e10f, 1e-10f},
    {1e-10f, 1e10f},
    {NAN, 1.0f}
};

// Test cases for 64-bit memory operands 
static const double mem64_test_cases[][2] = {
    {1.5, 1.5},
    {1.5, -1.5},
    {1e100, 1e-100},
    {1e-100, 1e100},
    {NAN, 1.0}
};

void test_reg_operands() {
    printf("\n=== Testing FCOM2alias with register operands (ST(0) vs ST(2)) ===\n");
    
    // Set double precision
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(reg_test_cases)/sizeof(reg_test_cases[0]); ++i) {
        double a = reg_test_cases[i][0];
        double b = reg_test_cases[i][1];
        
        printf("\nTest case %zu:\n", i+1);
        printf("ST(0): %.20g\n", a);
        printf("ST(2): %.20g\n", b);
        
        if (isnan(a) || isnan(b)) {
            // Special handling for NAN comparisons
            uint16_t old_cw, new_cw;
            asm volatile ("fstcw %0" : "=m" (old_cw));
            new_cw = old_cw | 0x3F; // Mask all exceptions (bits 0-5)
            asm volatile ("fldcw %0" : : "m" (new_cw));
            
            asm volatile (
                "fwait\n\t"
                "fnclex\n\t"  // Clear any pending exceptions
                "fldl %1\n\t" // load b into ST(0)
                "fldl %0\n\t" // load a into ST(0), b moves to ST(1)
                "fldz\n\t"    // push 0 to make b move to ST(2)
                "fcom %%st(2)\n"
                "fwait\n\t"
                : 
                : "m" (a), "m" (b)
            );
            
            uint16_t sw;
            asm volatile ("fstsw %0" : "=m" (sw));
            printf("NAN comparison - Status Word: 0x%04x\n", sw);
            printf("Exception Flags: %s%s%s%s%s\n",
                   (sw & 0x01) ? "IE " : "",
                   (sw & 0x02) ? "DE " : "",
                   (sw & 0x04) ? "ZE " : "",
                   (sw & 0x08) ? "OE " : "",
                   (sw & 0x10) ? "UE " : "");
            
            // Restore original control word and clear any exceptions
            asm volatile ("fldcw %0\n\t"
                          "fnclex" : : "m" (old_cw));
        } else {
            asm volatile (
                "fwait\n\t"
                "fldl %1\n\t"    // load b into ST(0)
                "fldl %0\n\t"    // load a into ST(0), b moves to ST(1)
                "fldz\n\t"       // push 0 to make b move to ST(2)
                "fcom %%st(2)\n" // compare ST(0) with ST(2)
                "fwait\n\t"
                : 
                : "m" (a), "m" (b)
            );
        }
        
        print_x87_status();
       // print_x87_stack();
        asm volatile ("fstp %st(0)\n\t" "fstp %st(0)\n\t" "fstp %st(0)\n\t");
    }
}

void test_mem32_operands() {
    printf("\n=== Testing FCOM2alias with 32-bit memory operands ===\n");
    
    // Set single precision
    uint16_t cw = X87_CW_PC_SINGLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(mem32_test_cases)/sizeof(mem32_test_cases[0]); ++i) {
        float a = mem32_test_cases[i][0];
        float b = mem32_test_cases[i][1];
        
        printf("\nTest case %zu:\n", i+1);
        printf("ST(0): %.10g\n", a);
        printf("m32: %.10g\n", b);
        
        if (isnan(a) || isnan(b)) {
            // Special handling for NAN comparisons
            uint16_t old_cw, new_cw;
            asm volatile ("fstcw %0" : "=m" (old_cw));
            new_cw = old_cw | 0x3F; // Mask all exceptions (bits 0-5)
            asm volatile ("fldcw %0" : : "m" (new_cw));
            
            asm volatile (
                "fwait\n\t"
                "fnclex\n\t"  // Clear any pending exceptions
                "flds %1\n\t" // load b into ST(0)
                "flds %0\n\t" // load a into ST(0), b moves to ST(1)
                "fldz\n\t"    // push 0 to make b move to ST(2)
                "fcoms %2\n"
                "fwait\n\t"
                : 
                : "m" (a), "m" (b), "m" (b)
            );
            
            uint16_t sw;
            asm volatile ("fstsw %0" : "=m" (sw));
            printf("NAN comparison - Status Word: 0x%04x\n", sw);
            printf("Exception Flags: %s%s%s%s%s\n",
                   (sw & 0x01) ? "IE " : "",
                   (sw & 0x02) ? "DE " : "",
                   (sw & 0x04) ? "ZE " : "",
                   (sw & 0x08) ? "OE " : "",
                   (sw & 0x10) ? "UE " : "");
            
            // Restore original control word
            asm volatile ("fldcw %0" : : "m" (old_cw));
        } else {
            asm volatile (
                "fwait\n\t"
                "flds %1\n\t"    // load b into ST(0)
                "flds %0\n\t"    // load a into ST(0), b moves to ST(1)
                "fldz\n\t"       // push 0 to make b move to ST(2)
                "fcoms %2\n"     // compare ST(0) with m32 (32-bit)
                "fwait\n\t"
                : 
                : "m" (a), "m" (b), "m" (b)
            );
        }
        
        print_x87_status();
        asm volatile ("fstp %st(0)\n\t" "fstp %st(0)\n\t" "fstp %st(0)\n\t");
    }
}

void test_mem64_operands() {
    printf("\n=== Testing FCOM2alias with 64-bit memory operands ===\n");
    
    // Set double precision
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    for (size_t i = 0; i < sizeof(mem64_test_cases)/sizeof(mem64_test_cases[0]); ++i) {
        double a = mem64_test_cases[i][0];
        double b = mem64_test_cases[i][1];
        
        printf("\nTest case %zu:\n", i+1);
        printf("ST(0): %.20g\n", a);
        printf("m64: %.20g\n", b);
        
        if (isnan(a) || isnan(b)) {
            // Special handling for NAN comparisons
            uint16_t old_cw, new_cw;
            asm volatile ("fstcw %0" : "=m" (old_cw));
            new_cw = old_cw | 0x3F; // Mask all exceptions (bits 0-5)
            asm volatile ("fldcw %0" : : "m" (new_cw));
            
            asm volatile (
                "fwait\n\t"
                "fnclex\n\t"  // Clear any pending exceptions
                "fldl %1\n\t" // load b into ST(0)
                "fldl %0\n\t" // load a into ST(0), b moves to ST(1)
                "fldz\n\t"    // push 0 to make b move to ST(2)
                "fcoml %2\n"
                "fwait\n\t"
                : 
                : "m" (a), "m" (b), "m" (b)
            );
            
            uint16_t sw;
            asm volatile ("fstsw %0" : "=m" (sw));
            printf("NAN comparison - Status Word: 0x%04x\n", sw);
            printf("Exception Flags: %s%s%s%s%s\n",
                   (sw & 0x01) ? "IE " : "",
                   (sw & 0x02) ? "DE " : "",
                   (sw & 0x04) ? "ZE " : "",
                   (sw & 0x08) ? "OE " : "",
                   (sw & 0x10) ? "UE " : "");
            
            // Restore original control word
            asm volatile ("fldcw %0" : : "m" (old_cw));
        } else {
            asm volatile (
                "fwait\n\t"
                "fldl %1\n\t"    // load b into ST(0)
                "fldl %0\n\t"    // load a into ST(0), b moves to ST(1)
                "fldz\n\t"       // push 0 to make b move to ST(2)
                "fcoml %2\n"     // compare ST(0) with m64 (64-bit)
                "fwait\n\t"
                : 
                : "m" (a), "m" (b), "m" (b)
            );
        }
        
        print_x87_status();
        asm volatile ("fstp %st(0)\n\t" "fstp %st(0)\n\t" "fstp %st(0)\n\t");
    }
}

int main() {
    init_x87_env();
     // When generating reference output, mask all exceptions
    printf("Testing FCOM2alias instruction\n");
    
    test_reg_operands();
    test_mem32_operands();
    test_mem64_operands();
    
    return 0;
}
