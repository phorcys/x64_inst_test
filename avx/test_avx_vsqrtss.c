#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSQRTSS - Compute Square Root of Scalar Single-Precision Floating-Point Value
// Computes square root of scalar single-precision floating-point value

static void test_vsqrtss() {
    printf("Testing VSQRTSS\n");
    
    // 测试用例
    struct TestCase {
        float input;
        float expected;
    } test_cases[] = {
        {1.0f, sqrtf(1.0f)},
        {4.0f, sqrtf(4.0f)},
        {16.0f, sqrtf(16.0f)},
        {64.0f, sqrtf(64.0f)},
        {0.25f, sqrtf(0.25f)},
        {-0.25f, NAN},  // 负数输入应返回NaN
        {INFINITY, INFINITY},
        {NAN, NAN}
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        float result = 0;
        float input = test_cases[i].input;
        float expected = test_cases[i].expected;

        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vsqrtss %%xmm0, %%xmm0, %%xmm1\n\t"
            "vmovss %%xmm1, %0\n\t"
            : "=m"(result)
            : "m"(input)
            : "xmm0", "xmm1"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input: %.9g\n", input);
        printf("Result: %.9g\n", result);
        printf("Expected: %.9g\n", expected);
        
        if (isnan(expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (isinf(expected)) {
            if (!isinf(result) || signbit(expected) != signbit(result)) {
                printf("Mismatch: expected %s\n", expected > 0 ? "+INF" : "-INF");
            }
        } else if (fabsf(result - expected) > 0.0001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result, expected);
        }
        printf("\n");
    }
}

int main() {
    test_vsqrtss();
    printf("VSQRTSS tests completed\n");
    return 0;
}
