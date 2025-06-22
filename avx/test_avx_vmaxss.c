#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Helper function to create SNaN (signaling NaN) for float
static float create_snan_f() {
    uint32_t snan = 0x7F800001;
    return *(float*)&snan;
}

// Compare two floats including NaN bit patterns
static int float_equal_bits(float a, float b) {
    uint32_t a_bits = *(uint32_t*)&a;
    uint32_t b_bits = *(uint32_t*)&b;
    return a_bits == b_bits;
}

// VMAXSS - Scalar Single-FP Maximum
// Compares scalar single-precision floating-point values and returns the maximum value

static void test_vmaxss() {
    printf("Testing VMAXSS\n");
    
    // Test cases
    struct {
        float a;
        float b;
        float expected;
        int check_bits; // 1 = check bit pattern, 0 = check value
    } tests[] = {
        // Basic cases
        {1.5f, -1.0f, 1.5f, 0},
        {-2.5f, 3.0f, 3.0f, 0},
        {3.0f, 2.0f, 3.0f, 0},
        {-4.0f, -3.0f, -3.0f, 0},
        
        // Zero cases - VMAXSS returns second operand when equal
        {0.0f, 0.0f, 0.0f, 0},
        {0.0f, -0.0f, -0.0f, 1}, // Check bit pattern for -0.0
        {-0.0f, 0.0f, 0.0f, 1},  // Check bit pattern for 0.0
        
        // NaN cases - VMAXSS returns second operand if first is NaN
        {NAN, 1.0f, 1.0f, 0},
        {NAN, -1.0f, -1.0f, 0},
        
        // NaN cases - VMAXSS returns second operand if it's NaN
        {1.0f, NAN, NAN, 0},
        {-1.0f, NAN, NAN, 0},
        
        // SNaN cases
        {1.0f, create_snan_f(), create_snan_f(), 1},
        {NAN, create_snan_f(), create_snan_f(), 1},
        
        // Infinity cases
        {INFINITY, 1.0f, INFINITY, 0},
        {-INFINITY, -1.0f, -1.0f, 0},
        {1.0f, INFINITY, INFINITY, 0},
        {-1.0f, -INFINITY, -1.0f, 0},
        {INFINITY, -INFINITY, INFINITY, 0}, // Corrected expected value
        {-INFINITY, INFINITY, INFINITY, 0}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        float result;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmaxss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(tests[i].a), "m"(tests[i].b)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu\n", i);
        printf("Input A: %f\n", tests[i].a);
        printf("Input B: %f\n", tests[i].b);
        printf("Result: %f\n", result);
        printf("Expected: %f\n", tests[i].expected);

        if (tests[i].check_bits) {
            if (!float_equal_bits(result, tests[i].expected)) {
                printf("Mismatch in bit pattern\n");
            }
        } else if (isnan(tests[i].expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN, got %f\n", result);
            }
        } else if (isinf(tests[i].expected)) {
            if (!isinf(result) || (signbit(tests[i].expected) != signbit(result))) {
                printf("Mismatch: got %f, expected %f\n", result, tests[i].expected);
            }
        } else {
            if (!float_equal(result, tests[i].expected, 0.0001f)) {
                printf("Mismatch: got %f, expected %f\n", result, tests[i].expected);
            }
        }
        printf("\n");
    }
}

int main() {
    test_vmaxss();
    printf("VMAXSS tests completed\n");
    return 0;
}
