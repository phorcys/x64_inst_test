#include <stdio.h>
#include <stdint.h>
#include <fenv.h>
#include <math.h>
#include <float.h>
#include "avx.h"

static void test_reg_operand(double d_val, float expected, const char* test_name) {
    __m128 dst = _mm_set_ps(1.1f, 2.2f, 3.3f, 4.4f);
    __m128d src = _mm_set_sd(d_val);
    
    __asm__ __volatile__(
        "vcvtsd2ss %[src], %[dst], %[dst]"
        : [dst] "+x" (dst)
        : [src] "x" (src)
    );
    float result = dst[0];
    printf("\nTest: %s\n", test_name);
    printf("Input double: %.15g (0x%016lX)\n", d_val, *(uint64_t*)&d_val);
    printf("Expected float: %.15g (0x%08X)\n", expected, *(uint32_t*)&expected);
    printf("Actual result:   %.15g (0x%08X)\n", result, *(uint32_t*)&result);
    //print_mxcsr(mxcsr_after);
    
    int pass = 1;
    if (isnan(d_val)) {
        if (!isnan(result)) {
            printf("  FAIL: Expected NaN, got %.15g\n", result);
            pass = 0;
        }
    } else if (isinf(d_val)) {
        // Check if result is infinity with matching sign
        // Normalize signbit to 0/1 since different implementations may return different non-zero values
        int sign_d = (signbit(d_val) != 0) ? 1 : 0;
        int sign_r = (signbit(result) != 0) ? 1 : 0;
        if (!isinf(result) || sign_d != sign_r) {
            printf("  FAIL: Expected %cInf, got %.15g\n", signbit(d_val)?'-':'+', result);
            pass = 0;
        }
    } else if (fabs(result - expected) > FLT_EPSILON) {
        printf("  FAIL: Result mismatch (diff: %g)\n", fabs(result - expected));
        pass = 0;
    }
    
    // Check upper bits are preserved
    float* f = (float*)&dst;
    if (fabs(f[1] - 3.3f) > FLT_EPSILON || 
        fabs(f[2] - 2.2f) > FLT_EPSILON || 
        fabs(f[3] - 1.1f) > FLT_EPSILON) {
        printf("  FAIL: Upper bits not preserved: [%.15g, %.15g, %.15g]\n", f[1], f[2], f[3]);
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

static void test_mem_operand(double d_val, float expected, const char* test_name) {
    __m128 dst = _mm_set_ps(1.1f, 2.2f, 3.3f, 4.4f);
    
    __asm__ __volatile__(
        "vcvtsd2ss %[src], %[dst], %[dst]"
        : [dst] "+x" (dst)
        : [src] "m" (d_val)
    );
    
    float result = dst[0];
    
    printf("\nTest: %s (Memory operand)\n", test_name);
    printf("Input double: %.15g (0x%016lX)\n", d_val, *(uint64_t*)&d_val);
    printf("Expected float: %.15g (0x%08X)\n", expected, *(uint32_t*)&expected);
    printf("Actual result:   %.15g (0x%08X)\n", result, *(uint32_t*)&result);
    //print_mxcsr(mxcsr_after);
    
    int pass = 1;
    if (isnan(d_val)) {
        if (!isnan(result)) {
            printf("  FAIL: Expected NaN, got %.15g\n", result);
            pass = 0;
        }
    } else if (isinf(d_val)) {
        // Check if result is infinity with matching sign
        // Normalize signbit to 0/1 since different implementations may return different non-zero values
        int sign_d = (signbit(d_val) != 0) ? 1 : 0;
        int sign_r = (signbit(result) != 0) ? 1 : 0;
        if (!isinf(result) || sign_d != sign_r) {
            printf("  FAIL: Expected %cInf, got %.15g\n", signbit(d_val)?'-':'+', result);
            pass = 0;
        }
    } else if (fabs(result - expected) > FLT_EPSILON) {
        printf("  FAIL: Result mismatch (diff: %g)\n", fabs(result - expected));
        pass = 0;
    }
    
    // Check upper bits are preserved
    float* f = (float*)&dst;
    if (fabs(f[1] - 3.3f) > FLT_EPSILON || 
        fabs(f[2] - 2.2f) > FLT_EPSILON || 
        fabs(f[3] - 1.1f) > FLT_EPSILON) {
        printf("  FAIL: Upper bits not preserved: [%.15g, %.15g, %.15g]\n", f[1], f[2], f[3]);
        pass = 0;
    }
    
    printf("%s\n\n", pass ? "TEST PASSED" : "TEST FAILED");
}

int main() {
    printf("====================\n");
    printf("VCVTSD2SS TEST SUITE\n");
    printf("====================\n\n");
    
    // Test normal conversions
    test_reg_operand(123.456789, 123.45679f, "Normal positive");
    test_reg_operand(-123.456789, -123.45679f, "Normal negative");
    
    // Test exact representations
    test_mem_operand(1.0, 1.0f, "Exact representation 1.0");
    test_mem_operand(0.5, 0.5f, "Exact representation 0.5");
    
    // Test denormals
    test_reg_operand(DBL_MIN, FLT_MIN, "Smallest denormal");
    test_reg_operand(1e-310, 0.0f, "Subnormal to zero");
    
    // Test special values
    test_reg_operand(INFINITY, INFINITY, "Positive infinity");
    test_reg_operand(-INFINITY, -INFINITY, "Negative infinity");
    test_reg_operand(NAN, NAN, "NaN");
    test_reg_operand(-NAN, -NAN, "-NaN");
    
    // Test precision limits
    test_mem_operand(DBL_MAX, INFINITY, "Double max to infinity");
    test_mem_operand(DBL_MIN, FLT_MIN, "Double min to zero");
    test_mem_operand(-DBL_MAX, -INFINITY, "Double max to negtive infinity");
    test_mem_operand(-DBL_MIN, 0.0f, "Double min to zero");
    test_mem_operand(1.234567890123456e300, INFINITY, "Large value to infinity");
    
    return 0;
}
