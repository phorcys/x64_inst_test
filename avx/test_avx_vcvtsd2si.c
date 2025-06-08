#include <stdio.h>
#include <stdint.h>
#include <fenv.h>
#include <math.h>
#include <float.h>
#include "avx.h"

// Function to set MXCSR rounding mode
static void set_mxcsr_rounding(uint32_t mode) {
    uint32_t mxcsr = get_mxcsr();
    mxcsr = (mxcsr & ~0x6000) | mode;
    set_mxcsr(mxcsr);
}

#define TEST_ROUNDING_MODE(mode, name) \
    set_mxcsr_rounding(mode); \
    printf("Testing with rounding mode: %s\n", name);

// Test VCVTSD2SI with register operand
static void test_reg_operand(double double_val, int32_t expected, const char* test_name, int truncate) {
    int32_t result;
    uint32_t mxcsr_before = get_mxcsr();
    
    if (truncate) {
        __asm__ __volatile__(
            "vcvttsd2si %[double_val], %[res]"
            : [res] "=r" (result)
            : [double_val] "x" (double_val)
        );
    } else {
        __asm__ __volatile__(
            "vcvtsd2si %[double_val], %[res]"
            : [res] "=r" (result)
            : [double_val] "x" (double_val)
        );
    }
    
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s\n", test_name);
    printf("Input double: %.6f (0x%016lX)\n", double_val, *(uint64_t*)&double_val);
    printf("Expected integer: %d (0x%08X)\n", expected, expected);
    printf("Actual result:   %d (0x%08X)\n", result, result);
    print_mxcsr(mxcsr_after);
    
    // Check result and flags
    int pass = 1;
    
    // Handle special cases: NaN, infinity
    if (isnan(double_val)) {
        // Result for NaN is undefined, but we expect 0x80000000
        if (result != 0x80000000) {
            printf("  FAIL: Expected 0x80000000 for NaN, got 0x%08X\n", result);
            pass = 0;
        }
    } else if (isinf(double_val)) {
        // Result for infinity is undefined, but we expect 0x80000000
        if (result != 0x80000000) {
            printf("  FAIL: Expected 0x80000000 for infinity, got 0x%08X\n", result);
            pass = 0;
        }
    } else {
        // Normal number comparison
        if (result != expected) {
            printf("  FAIL: Result mismatch\n");
            pass = 0;
        }
    }
    
    // Check for invalid operation flag (I) if needed
    if (isnan(double_val) || isinf(double_val)) {
        if (!(mxcsr_after & 0x01)) {
            printf("  FAIL: Invalid operation flag not set\n");
            pass = 0;
        }
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

// Test VCVTSD2SI with memory operand
static void test_mem_operand(double double_val, int32_t expected, const char* test_name, int truncate) {
    int32_t result;
    uint32_t mxcsr_before = get_mxcsr();
    
    if (truncate) {
        __asm__ __volatile__(
            "vcvttsd2si %[double_val], %[res]"
            : [res] "=r" (result)
            : [double_val] "m" (double_val)
        );
    } else {
        __asm__ __volatile__(
            "vcvtsd2si %[double_val], %[res]"
            : [res] "=r" (result)
            : [double_val] "m" (double_val)
        );
    }
    
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s (Memory operand)\n", test_name);
    printf("Input double: %.6f (0x%016lX)\n", double_val, *(uint64_t*)&double_val);
    printf("Expected integer: %d (0x%08X)\n", expected, expected);
    printf("Actual result:   %d (0x%08X)\n", result, result);
    print_mxcsr(mxcsr_after);
    
    // Check result and flags
    int pass = 1;
    
    // Handle special cases: NaN, infinity
    if (isnan(double_val)) {
        // Result for NaN is undefined, but we expect 0x80000000
        if (result != 0x80000000) {
            printf("  FAIL: Expected 0x80000000 for NaN, got 0x%08X\n", result);
            pass = 0;
        }
    } else if (isinf(double_val)) {
        // Result for infinity is undefined, but we expect 0x80000000
        if (result != 0x80000000) {
            printf("  FAIL: Expected 0x80000000 for infinity, got 0x%08X\n", result);
            pass = 0;
        }
    } else {
        // Normal number comparison
        if (result != expected) {
            printf("  FAIL: Result mismatch\n");
            pass = 0;
        }
    }
    
    // Check for invalid operation flag (I) if needed
    if (isnan(double_val) || isinf(double_val)) {
        if (!(mxcsr_after & 0x01)) {
            printf("  FAIL: Invalid operation flag not set\n");
            pass = 0;
        }
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

int main() {
    printf("====================\n");
    printf("VCVTSD2SI TEST SUITE\n");
    printf("====================\n\n");
    
    // Test different rounding modes (non-truncating version)
    TEST_ROUNDING_MODE(0x0000, "Round to nearest");
    test_reg_operand(123.456, 123, "Positive double", 0);
    test_reg_operand(-123.456, -123, "Negative double", 0);
    
    TEST_ROUNDING_MODE(0x2000, "Round toward -inf");
    test_mem_operand(123.456, 123, "Positive double", 0);
    test_mem_operand(-123.456, -124, "Negative double", 0); // Should round toward -inf
    
    TEST_ROUNDING_MODE(0x4000, "Round toward +inf");
    test_reg_operand(123.456, 124, "Positive double", 0); // Should round toward +inf
    test_reg_operand(-123.456, -123, "Negative double", 0);
    
    TEST_ROUNDING_MODE(0x6000, "Round toward zero");
    test_mem_operand(123.456, 123, "Positive double", 0);
    test_mem_operand(-123.456, -123, "Negative double", 0);
    
    // Test truncating version (vcvttsd2si)
    TEST_ROUNDING_MODE(0x0000, "Truncate (Round to nearest)");
    test_reg_operand(123.999, 123, "Truncate positive", 1);
    test_reg_operand(-123.999, -123, "Truncate negative", 1);
    
    // Boundary cases
    test_reg_operand(DBL_MAX, 0x80000000, "DBL_MAX (out of range)", 0);
    test_reg_operand(DBL_MIN, 0, "DBL_MIN", 0);
    test_reg_operand(NAN, 0x80000000, "NaN input", 0);
    test_reg_operand(INFINITY, 0x80000000, "Positive infinity", 0);
    test_reg_operand(-INFINITY, 0x80000000, "Negative infinity", 0);
    
    // Precision test
    // Test large positive and negative values
    test_reg_operand(2147483647.0, 2147483647, "Max int32", 0);
    test_reg_operand(-2147483648.0, -2147483648, "Min int32", 0);
    test_reg_operand(2147483648.0, 0x80000000, "Positive out of range", 0);
    test_reg_operand(-2147483649.0, 0x80000000, "Negative out of range", 0);
    
    test_reg_operand(123456789.123456, 123456789, "Precision test", 0);
    
    return 0;
}
