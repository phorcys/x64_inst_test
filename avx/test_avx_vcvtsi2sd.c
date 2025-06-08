#include <stdio.h>
#include <stdint.h>
#include <fenv.h>
#include <math.h>
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

// Test VCVTSI2SD with register operand
static void test_reg_operand(int32_t int_val, double expected, const char* test_name) {
    __m128d vec = _mm_setr_pd(1.0, 2.0);
    __m128d result;
    
    __asm__ __volatile__(
        "vcvtsi2sd %[int_val], %[vec], %[res]"
        : [res] "=x" (result)
        : [vec] "x" (vec),
          [int_val] "r" (int_val)
    );
    
    double res[2];
    _mm_storeu_pd(res, result);
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s\n", test_name);
    printf("Input integer: %d (0x%08X)\n", int_val, int_val);
    printf("Expected double: %.6f (0x%016lX)\n", expected, *(uint64_t*)&expected);
    printf("Actual result:  %.6f (0x%016lX)\n", res[0], *(uint64_t*)&res[0]);
    print_mxcsr(mxcsr_after);
    
    // Check result and upper vector preservation
    int pass = 1;
    if (!double_equal(res[0], expected, 0.0001)) {
        printf("  FAIL: Scalar result mismatch\n");
        pass = 0;
    }
    if (res[1] != 2.0) {
        printf("  FAIL: Upper vector element not preserved\n");
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

// Test VCVTSI2SD with memory operand
static void test_mem_operand(int32_t int_val, double expected, const char* test_name) {
    __m128d vec = _mm_setr_pd(3.0, 4.0);
    __m128d result;
    
    __asm__ __volatile__(
        "vcvtsi2sd %[int_val], %[vec], %[res]"
        : [res] "=x" (result)
        : [vec] "x" (vec),
          [int_val] "m" (int_val)
    );
    
    double res[2];
    _mm_storeu_pd(res, result);
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s (Memory operand)\n", test_name);
    printf("Input integer: %d (0x%08X)\n", int_val, int_val);
    printf("Expected double: %.6f (0x%016lX)\n", expected, *(uint64_t*)&expected);
    printf("Actual result:  %.6f (0x%016lX)\n", res[0], *(uint64_t*)&res[0]);
    print_mxcsr(mxcsr_after);
    
    // Check result and upper vector preservation
    int pass = 1;
    if (!double_equal(res[0], expected, 0.0001)) {
        printf("  FAIL: Scalar result mismatch\n");
        pass = 0;
    }
    if (res[1] != 4.0) {
        printf("  FAIL: Upper vector element not preserved\n");
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

int main() {
    printf("====================\n");
    printf("VCVTSI2SD TEST SUITE\n");
    printf("====================\n\n");
    
    // Test different rounding modes
    TEST_ROUNDING_MODE(0x0000, "Round to nearest");
    test_reg_operand(123456789, 123456789.0, "Positive integer");
    test_reg_operand(-123456789, -123456789.0, "Negative integer");
    
    TEST_ROUNDING_MODE(0x2000, "Round toward -inf");
    test_mem_operand(2147483647, 2147483647.0, "INT_MAX");
    test_mem_operand(-2147483648, -2147483648.0, "INT_MIN");
    
    TEST_ROUNDING_MODE(0x4000, "Round toward +inf");
    test_reg_operand(0, 0.0, "Zero");
    test_reg_operand(1, 1.0, "One");
    
    TEST_ROUNDING_MODE(0x6000, "Round toward zero");
    test_mem_operand(0x7FFFFFFF, 2147483647.0, "Large positive");
    test_mem_operand(0x80000000, -2147483648.0, "Large negative");
    
    // 32-bit boundary cases
    test_reg_operand(16777216, 16777216.0, "2^24 - Exact representation in float");
    test_reg_operand(16777217, 16777217.0, "2^24+1 - Not exactly representable in float but exact in double");
    test_reg_operand(2147483647, 2147483647.0, "INT_MAX - Maximum 32-bit integer");
    
    return 0;
}
