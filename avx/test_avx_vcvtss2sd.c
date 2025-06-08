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

// Test VCVTSS2SD with register operand
static void test_reg_operand(float float_val, double expected, const char* test_name) {
    __m128d vec = _mm_setr_pd(1.0, 2.0);
    __m128d result;
    
    __asm__ __volatile__(
        "vcvtss2sd %[float_val], %[vec], %[res]"
        : [res] "=x" (result)
        : [vec] "x" (vec),
          [float_val] "x" (float_val)
    );
    
    double res[2];
    _mm_storeu_pd(res, result);
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s\n", test_name);
    printf("Input float: %.6f (0x%08X)\n", float_val, *(uint32_t*)&float_val);
    printf("Expected double: %.6f (0x%016lX)\n", expected, *(uint64_t*)&expected);
    printf("Actual result:  %.6f (0x%016lX)\n", res[0], *(uint64_t*)&res[0]);
    print_mxcsr(mxcsr_after);
    
    // Check result and upper vector preservation
    int pass = 1;
    
    // Handle special cases: NaN, infinity
    if (isnan(expected)) {
        if (!isnan(res[0])) {
            printf("  FAIL: Expected NaN, got %.6f\n", res[0]);
            pass = 0;
        }
    } else if (isinf(expected)) {
        if (!isinf(res[0]) || (signbit(res[0]) != signbit(expected))) {
            printf("  FAIL: Expected %s infinity, got %s%.6f\n", 
                   signbit(expected) ? "negative" : "positive",
                   signbit(res[0]) ? "negative" : "positive", res[0]);
            pass = 0;
        }
    } else {
        // Normal number comparison
        if (!double_equal(res[0], expected, 0.0001)) {
            printf("  FAIL: Scalar result mismatch\n");
            pass = 0;
        }
    }
    
    if (res[1] != 2.0) {
        printf("  FAIL: Upper vector element not preserved\n");
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

// Test VCVTSS2SD with memory operand
static void test_mem_operand(float float_val, double expected, const char* test_name) {
    __m128d vec = _mm_setr_pd(3.0, 4.0);
    __m128d result;
    
    __asm__ __volatile__(
        "vcvtss2sd %[float_val], %[vec], %[res]"
        : [res] "=x" (result)
        : [vec] "x" (vec),
          [float_val] "m" (float_val)
    );
    
    double res[2];
    _mm_storeu_pd(res, result);
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s (Memory operand)\n", test_name);
    printf("Input float: %.6f (0x%08X)\n", float_val, *(uint32_t*)&float_val);
    printf("Expected double: %.6f (0x%016lX)\n", expected, *(uint64_t*)&expected);
    printf("Actual result:  %.6f (0x%016lX)\n", res[0], *(uint64_t*)&res[0]);
    print_mxcsr(mxcsr_after);
    
    // Check result and upper vector preservation
    int pass = 1;
    
    // Handle special cases: NaN, infinity
    if (isnan(expected)) {
        if (!isnan(res[0])) {
            printf("  FAIL: Expected NaN, got %.6f\n", res[0]);
            pass = 0;
        }
    } else if (isinf(expected)) {
        if (!isinf(res[0]) || (signbit(res[0]) != signbit(expected))) {
            printf("  FAIL: Expected %s infinity, got %s%.6f\n", 
                   signbit(expected) ? "negative" : "positive",
                   signbit(res[0]) ? "negative" : "positive", res[0]);
            pass = 0;
        }
    } else {
        // Normal number comparison
        if (!double_equal(res[0], expected, 0.0001)) {
            printf("  FAIL: Scalar result mismatch\n");
            pass = 0;
        }
    }
    
    if (res[1] != 4.0) {
        printf("  FAIL: Upper vector element not preserved\n");
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

int main() {
    printf("====================\n");
    printf("VCVTSS2SD TEST SUITE\n");
    printf("====================\n\n");
    
    // Test different rounding modes
    TEST_ROUNDING_MODE(0x0000, "Round to nearest");
    test_reg_operand(123.456f, 123.456, "Positive float");
    test_reg_operand(-123.456f, -123.456, "Negative float");
    
    TEST_ROUNDING_MODE(0x2000, "Round toward -inf");
    test_mem_operand(123.456f, 123.456, "Positive float");
    test_mem_operand(-123.456f, -123.456, "Negative float");
    
    TEST_ROUNDING_MODE(0x4000, "Round toward +inf");
    test_reg_operand(0.0f, 0.0, "Zero");
    test_reg_operand(1.0f, 1.0, "One");
    
    TEST_ROUNDING_MODE(0x6000, "Round toward zero");
    test_mem_operand(123.456f, 123.456, "Positive float");
    test_mem_operand(-123.456f, -123.456, "Negative float");
    
    // Boundary cases
    test_reg_operand(FLT_MAX, (double)FLT_MAX, "FLT_MAX");
    test_reg_operand(FLT_MIN, (double)FLT_MIN, "FLT_MIN");
    test_reg_operand(NAN, NAN, "NaN input");
    test_reg_operand(INFINITY, INFINITY, "Positive infinity");
    test_reg_operand(-INFINITY, -INFINITY, "Negative infinity");
    
    // Precision test
    test_reg_operand(0.123456789f, 0.123456789, "Precision test");
    
    return 0;
}
