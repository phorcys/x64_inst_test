#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Helper function to create SNaN (signaling NaN)
static double create_snan() {
    uint64_t snan = 0x7FF0000000000001ULL;
    return *(double*)&snan;
}

// Compare two doubles including NaN bit patterns
static int double_equal_bits(double a, double b) {
    uint64_t a_bits = *(uint64_t*)&a;
    uint64_t b_bits = *(uint64_t*)&b;
    return a_bits == b_bits;
}

// VMAXSD - Scalar Double-FP Maximum
// Compares scalar double-precision floating-point values and returns the maximum value

static void test_vmaxsd() {
    printf("Testing VMAXSD\n");
    
    // Test cases
    struct {
        double a;
        double b;
        double expected;
        int check_bits; // 1 = check bit pattern, 0 = check value
    } tests[] = {
        // Basic cases
        {1.5, -1.0, 1.5, 0},
        {-2.5, 3.0, 3.0, 0},
        {3.0, 2.0, 3.0, 0},
        {-4.0, -3.0, -3.0, 0},
        
        // Zero cases - VMAXSD returns second operand when equal
        {0.0, 0.0, 0.0, 0},
        {0.0, -0.0, -0.0, 1}, // Check bit pattern for -0.0
        {-0.0, 0.0, 0.0, 1},  // Check bit pattern for 0.0
        
        // NaN cases - VMAXSD returns second operand if first is NaN
        {NAN, 1.0, 1.0, 0},
        {NAN, -1.0, -1.0, 0},
        
        // NaN cases - VMAXSD returns second operand if it's NaN
        {1.0, NAN, NAN, 0},
        {-1.0, NAN, NAN, 0},
        
        // SNaN cases
        {1.0, create_snan(), create_snan(), 1},
        {NAN, create_snan(), create_snan(), 1},
        
        // Infinity cases
        {INFINITY, 1.0, INFINITY, 0},
        {-INFINITY, -1.0, -1.0, 0},
        {1.0, INFINITY, INFINITY, 0},
        {-1.0, -INFINITY, -1.0, 0},
        {INFINITY, -INFINITY, INFINITY, 0}, // Corrected expected value
        {-INFINITY, INFINITY, INFINITY, 0}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        double result;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmaxsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
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
            if (!double_equal_bits(result, tests[i].expected)) {
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
            if (!double_equal(result, tests[i].expected, 0.0001)) {
                printf("Mismatch: got %f, expected %f\n", result, tests[i].expected);
            }
        }
        printf("\n");
    }
}

int main() {
    test_vmaxsd();
    printf("VMAXSD tests completed\n");
    return 0;
}
