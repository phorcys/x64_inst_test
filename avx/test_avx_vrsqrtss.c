#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VRSQRTSS - Compute Reciprocal of Square Root of Scalar Single-Precision Floating-Point Value
// Computes approximate reciprocal of square root (1/√x) of scalar single-precision floating-point value

static void test_vrsqrtss() {
    printf("Testing VRSQRTSS\n");
    
    struct TestCase {
        float a;
        float expected;
    } test_cases[] = {
        {1.0f, 1.0f/sqrtf(1.0f)},  // 基本测试
        {4.0f, 1.0f/sqrtf(4.0f)},
        {16.0f, 1.0f/sqrtf(16.0f)},
        {64.0f, 1.0f/sqrtf(64.0f)},
        {0.25f, 1.0f/sqrtf(0.25f)},
        {-0.25f, NAN},  // 负数测试
        {0.0f, INFINITY},  // 零值测试
        {-0.0f, -INFINITY},
        {INFINITY, 0.0f},  // 无穷大测试
        {-INFINITY, -0.0f},
        {NAN, NAN}  // NaN测试
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        float result = 0;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vrsqrtss %%xmm0, %%xmm0, %%xmm1\n\t"
            "vmovss %%xmm1, %0\n\t"
            : "=m"(result)
            : "m"(test_cases[i].a)
            : "xmm0", "xmm1"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input: %.9g\n", test_cases[i].a);
        printf("Result: %.9g\n", result);
        printf("Expected: %.9g\n", test_cases[i].expected);

        if (isnan(test_cases[i].expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (isinf(test_cases[i].expected) && isinf(result)) {
            if (signbit(test_cases[i].expected) != signbit(result)) {
                printf("Mismatch: sign differs for infinity\n");
            }
        } else if (fabsf(result - test_cases[i].expected) > 0.001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result, test_cases[i].expected);
        }
        printf("\n");
    }
}

int main() {
    test_vrsqrtss();
    printf("VRSQRTSS tests completed\n");
    return 0;
}
