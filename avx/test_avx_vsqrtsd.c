#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSQRTSD - Compute Square Root of Scalar Double-Precision Floating-Point Value
// Computes square root of scalar double-precision floating-point value

static void test_vsqrtsd() {
    printf("Testing VSQRTSD\n");
    
    // 测试用例
    struct TestCase {
        double input;
        double expected;
    } test_cases[] = {
        {1.0, sqrt(1.0)},
        {4.0, sqrt(4.0)},
        {16.0, sqrt(16.0)},
        {64.0, sqrt(64.0)},
        {0.25, sqrt(0.25)},
        {-0.25, NAN},  // 负数输入应返回NaN
        {INFINITY, INFINITY},
        {NAN, NAN}
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double result = 0;
        double input = test_cases[i].input;
        double expected = test_cases[i].expected;

        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vsqrtsd %%xmm0, %%xmm0, %%xmm1\n\t"
            "vmovsd %%xmm1, %0\n\t"
            : "=m"(result)
            : "m"(input)
            : "xmm0", "xmm1"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input: %.17g\n", input);
        printf("Result: %.17g\n", result);
        printf("Expected: %.17g\n", expected);
        
        if (isnan(expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (isinf(expected)) {
            if (!isinf(result) || signbit(expected) != signbit(result)) {
                printf("Mismatch: expected %s\n", expected > 0 ? "+INF" : "-INF");
            }
        } else if (fabs(result - expected) > 0.0000001) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result, expected);
        }
        printf("\n");
    }
}

int main() {
    test_vsqrtsd();
    printf("VSQRTSD tests completed\n");
    return 0;
}
