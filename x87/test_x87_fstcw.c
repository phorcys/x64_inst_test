#include <stdio.h>
#include <stdint.h>
#include <fenv.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void test_fstcw_status() {
    printf("\n==== Testing FSTCW Status Register Effects ====\n");
    
    // Test with different control words and check status register
    struct {
        uint16_t cw;
        const char* desc;
    } tests[] = {
        {0x0000, "All exceptions unmasked, single precision"},
        {0x037F, "Default control word"},
        {0x0C00, "Truncate rounding, all masked"},
        {0xFFFF, "All bits set"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s (CW=0x%04X)\n", i+1, tests[i].desc, tests[i].cw);
        
        // Get initial status
        uint16_t initial_sw = get_x87_sw();
        
        // Set control word
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        // Save control word
        uint16_t saved_cw;
        asm volatile ("fnstcw %0" : "=m" (saved_cw));
        
        // Get new status
        uint16_t new_sw = get_x87_sw();
        
        printf("Status before: 0x%04X\n", initial_sw);
        printf("Status after:  0x%04X\n", new_sw);
        printf("Saved CW:      0x%04X\n", saved_cw);
        
        TEST_CASE("Control word saved correctly", saved_cw == tests[i].cw);
        TEST_CASE("Status word unchanged by FSTCW", new_sw == initial_sw);
    }
}

void test_fstcw_modern() {
    printf("\n==== Testing FSTCW with Modern CPU Features ====\n");
    
    // Test with SSE enabled
    printf("\n-- Test 1: With SSE --\n");
    uint32_t mxcsr_before, mxcsr_after;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_before));
    
    uint16_t cw = 0x037F;
    asm volatile ("fninit");
    asm volatile ("fldcw %0" : : "m" (cw));
    
    uint16_t saved_cw;
    asm volatile ("fnstcw %0" : "=m" (saved_cw));
    
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_after));
    printf("MXCSR before: 0x%08X\n", mxcsr_before);
    printf("MXCSR after:  0x%08X\n", mxcsr_after);
    TEST_CASE("MXCSR unchanged with FSTCW", mxcsr_before == mxcsr_after);
    TEST_CASE("Control word saved correctly", saved_cw == cw);
    
    // Test with AVX enabled
    printf("\n-- Test 2: With AVX --\n");
    uint64_t xcr0_before, xcr0_after;
    asm volatile ("xgetbv" : "=a" (xcr0_before) : "c" (0) : "edx");
    
    asm volatile ("fninit");
    asm volatile ("fldcw %0" : : "m" (cw));
    asm volatile ("fnstcw %0" : "=m" (saved_cw));
    
    asm volatile ("xgetbv" : "=a" (xcr0_after) : "c" (0) : "edx");
    printf("XCR0 before: 0x%016lX\n", xcr0_before);
    printf("XCR0 after:  0x%016lX\n", xcr0_after);
    TEST_CASE("XCR0 unchanged with FSTCW", xcr0_before == xcr0_after);
    TEST_CASE("Control word saved correctly", saved_cw == cw);
}

void test_fstcw_boundary() {
    printf("\n==== Testing FSTCW Boundary Values ====\n");
    
    struct {
        uint16_t cw;
        const char* desc;
        int expected_exceptions;
    } tests[] = {
        {0x0000, "Minimum control word (all unmasked)", FE_ALL_EXCEPT},
        {0x0001, "Single bit set (IE unmasked)", FE_INVALID},
        {0x7FFF, "Max value - 1", 0},
        {0xFFFF, "Maximum control word", 0},
        {0x5555, "Alternating bits", FE_INVALID|FE_UNDERFLOW|FE_OVERFLOW},
        {0xAAAA, "Alternating bits (inverse)", FE_DIVBYZERO|FE_UNDERFLOW|FE_INEXACT}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s (CW=0x%04X)\n", i+1, tests[i].desc, tests[i].cw);
        
        // Clear exceptions
        feclearexcept(FE_ALL_EXCEPT);
        
        // Set control word
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        // Save control word
        uint16_t saved_cw;
        asm volatile ("fnstcw %0" : "=m" (saved_cw));
        
        // Check exceptions
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        
        printf("Set CW:    0x%04X\n", tests[i].cw);
        printf("Saved CW:  0x%04X\n", saved_cw);
        printf("Exceptions: 0x%X\n", exceptions);
        print_x87_status();
        
        TEST_CASE("Control word saved correctly", saved_cw == tests[i].cw);
        TEST_CASE("Correct exceptions raised", 
                 (exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions);
    }
}

void test_fstcw_combinations() {
    printf("\n==== Testing FSTCW Combinations ====\n");
    
    struct {
        uint16_t cw;
        const char* desc;
    } tests[] = {
        {0x037F, "Default settings"},
        {0x027F, "Double precision"},
        {0x007F, "Single precision"},
        {0x033F, "Invalid op unmasked"},
        {0x035F, "Denormal unmasked"},
        {0x036F, "Zero-divide unmasked"},
        {0x0377, "Overflow unmasked"},
        {0x0300, "Extended prec, nearest, all unmasked"},
        {0x0C00, "Extended prec, truncate, all masked"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        uint16_t saved_cw;
        asm volatile ("fnstcw %0" : "=m" (saved_cw));
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Set CW: 0x%04X, Saved CW: 0x%04X\n", tests[i].cw, saved_cw);
        TEST_CASE("Control word saved correctly", saved_cw == tests[i].cw);
    }
}

void test_exception_state() {
    printf("\n==== Testing FSTCW with Exceptions ====\n");
    
    // Generate divide by zero exception
    asm volatile ("fninit");
    asm volatile ("fldz");
    asm volatile ("fld1");
    asm volatile ("fdivp");
    
    printf("After exception:\n");
    print_x87_status();
    
    // Save control word
    uint16_t saved_cw;
    asm volatile ("fnstcw %0" : "=m" (saved_cw));
    
    printf("Saved control word: 0x%04X\n", saved_cw);
    TEST_CASE("Exception flags preserved", (saved_cw & 0x3F) == 0x04);
}

void compare_fstcw_fnstcw() {
    printf("\n==== Comparing FSTCW vs FNSTCW ====\n");
    
    // Set up test state with exception
    asm volatile ("fninit");
    asm volatile ("fldz");
    asm volatile ("fldz");
    asm volatile ("fdivp");  // Create divide by zero
    
    // Save with FSTCW (may trigger exception handling)
    uint16_t fstcw_val;
    asm volatile ("fstcw %0" : "=m" (fstcw_val));
    
    // Save with FNSTCW (no exception handling)
    uint16_t fnstcw_val;
    asm volatile ("fnstcw %0" : "=m" (fnstcw_val));
    
    printf("FSTCW value: 0x%04X\n", fstcw_val);
    printf("FNSTCW value: 0x%04X\n", fnstcw_val);
    TEST_CASE("FSTCW and FNSTCW values match", fstcw_val == fnstcw_val);
}

void test_rounding_modes() {
    printf("\n==== Testing FSTCW with Rounding Modes ====\n");
    
    uint16_t modes[] = {0x0000, 0x0400, 0x0800, 0x0C00};
    const char* mode_names[] = {"Nearest", "Down", "Up", "Truncate"};
    
    for (int i = 0; i < 4; i++) {
        uint16_t cw = 0x037F & ~0x0C00;
        cw |= modes[i];
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (cw));
        
        uint16_t saved_cw;
        asm volatile ("fnstcw %0" : "=m" (saved_cw));
        
        printf("Mode: %s\n", mode_names[i]);
        printf("Set CW: 0x%04X, Saved CW: 0x%04X\n", cw, saved_cw);
        TEST_CASE("Rounding mode saved correctly", (saved_cw & 0x0C00) == modes[i]);
    }
}

void test_precision_control() {
    printf("\n==== Testing FSTCW with Precision Control ====\n");
    
    uint16_t precisions[] = {0x0000, 0x0200, 0x0300};
    const char* prec_names[] = {"Single", "Double", "Extended"};
    
    for (int i = 0; i < 3; i++) {
        uint16_t cw = 0x037F & ~0x0300;
        cw |= precisions[i];
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (cw));
        
        uint16_t saved_cw;
        asm volatile ("fnstcw %0" : "=m" (saved_cw));
        
        printf("Precision: %s\n", prec_names[i]);
        printf("Set CW: 0x%04X, Saved CW: 0x%04X\n", cw, saved_cw);
        TEST_CASE("Precision control saved correctly", (saved_cw & 0x0300) == precisions[i]);
    }
}

void test_control_word_save() {
    printf("\n==== Testing Control Word Save ====\n");
    
    // Set specific control word
    uint16_t test_cw = 0x027F;  // Different rounding and precision
    asm volatile ("fninit");
    asm volatile ("fldcw %0" : : "m" (test_cw));
    
    // Save control word
    uint16_t saved_cw;
    asm volatile ("fnstcw %0" : "=m" (saved_cw));
    
    printf("Original CW: 0x%04X\n", test_cw);
    printf("Saved CW:    0x%04X\n", saved_cw);
    TEST_CASE("Control word saved correctly", saved_cw == test_cw);
    
    // Verify individual fields
    TEST_CASE("Precision control saved", 
             (saved_cw & 0x0300) == (test_cw & 0x0300));
    TEST_CASE("Rounding control saved",
             (saved_cw & 0x0C00) == (test_cw & 0x0C00));
    TEST_CASE("Exception masks saved",
             (saved_cw & 0x003F) == (test_cw & 0x003F));
}

int main() {
    printf("=== Testing FSTCW/FNSTCW instructions ===\n");
    
    compare_fstcw_fnstcw();
    test_rounding_modes();
    test_precision_control();
    test_control_word_save();
    test_fstcw_boundary();
    test_fstcw_modern();
    test_fstcw_status();
    test_fstcw_combinations();
    test_exception_state();
    
    printf("\n=== FSTCW/FNSTCW tests completed ===\n");
    return 0;
}
