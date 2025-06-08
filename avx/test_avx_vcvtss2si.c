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

// Test VCVTSS2SI with register operand
static void test_reg_operand(float float_val, int32_t expected, const char* test_name) {
    int32_t result;
    
    __asm__ __volatile__(
        "vcvtss2si %[float_val], %[res]"
        : [res] "=r" (result)
        : [float_val] "x" (float_val)
    );
    
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s\n", test_name);
    printf("Input float: %.6f (0x%08X)\n", float_val, *(uint32_t*)&float_val);
    printf("Expected integer: %d (0x%08X)\n", expected, expected);
    printf("Actual result:  %d (0x%08X)\n", result, result);
    print_mxcsr(mxcsr_after);
    
    // Check result
    int pass = 1;
    if (result != expected) {
        printf("  FAIL: Result mismatch\n");
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

// Test VCVTSS2SI with memory operand
static void test_mem_operand(float float_val, int32_t expected, const char* test_name) {
    int32_t result;
    
    __asm__ __volatile__(
        "vcvtss2si %[float_val], %[res]"
        : [res] "=r" (result)
        : [float_val] "m" (float_val)
    );
    
    uint32_t mxcsr_after = get_mxcsr();
    
    printf("\nTest: %s (Memory operand)\n", test_name);
    printf("Input float: %.6f (0x%08X)\n", float_val, *(uint32_t*)&float_val);
    printf("Expected integer: %d (0x%08X)\n", expected, expected);
    printf("Actual result:  %d (0x%08X)\n", result, result);
    print_mxcsr(mxcsr_after);
    
    // Check result
    int pass = 1;
    if (result != expected) {
        printf("  FAIL: Result mismatch\n");
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

int main() {
    printf("====================\n");
    printf("VCVTSS2SI TEST SUITE\n");
    printf("====================\n\n");
    
    // Test different rounding modes
    TEST_ROUNDING_MODE(0x0000, "Round to nearest");
    test_reg_operand(123.456, 123, "Positive float");
    test_reg_operand(-123.456, -123, "Negative float");
    
    TEST_ROUNDING_MODE(0x2000, "Round toward -inf");
    test_mem_operand(123.456, 123, "Positive float");
    test_mem_operand(-123.456, -124, "Negative float");
    
    TEST_ROUNDING_MODE(0x4000, "Round toward +inf");
    test_reg_operand(123.456, 124, "Positive float");
    test_reg_operand(-123.456, -123, "Negative float");
    
    TEST_ROUNDING_MODE(0x6000, "Round toward zero");
    test_mem_operand(123.456, 123, "Positive float");
    test_mem_operand(-123.456, -123, "Negative float");
    
    // Boundary cases
    test_reg_operand(2147483520.0f, 2147483520, "Max representable integer");
    test_reg_operand(-2147483520.0f, -2147483520, "Min representable integer");
    test_reg_operand(NAN, 0x80000000, "NaN input");
    test_reg_operand(INFINITY, 0x80000000, "Positive infinity");
    test_reg_operand(-INFINITY, 0x80000000, "Negative infinity");
    
    return 0;
}
