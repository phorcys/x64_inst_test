#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMULSS - Scalar Single-FP Multiply
// Multiplies scalar single-precision floating-point values

static void test_vmulss() {
    printf("Testing VMULSS\n");
    
    struct TestCase {
        float a;
        float b;
        float expected;
    } test_cases[] = {
        {1.5f, 2.0f, 3.0f},  // 基本测试
        {0.0f, 1.0f, 0.0f},  // 零值测试
        {-0.0f, -1.0f, 0.0f}, // 负零测试
        {INFINITY, 1.0f, INFINITY}, // 无穷大测试
        {-INFINITY, 1.0f, -INFINITY},
        {NAN, 1.0f, NAN},  // NaN测试
        {1.0f, NAN, NAN},
        {3.402823466e+38f, 2.0f, INFINITY}, // 溢出测试
        {-3.402823466e+38f, 2.0f, -INFINITY},
        {1.175494351e-38f, 0.5f, 5.877471754e-39f} // 下溢测试
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        float result = 0;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmulss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(test_cases[i].a), "m"(test_cases[i].b)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input A: %.9g\n", test_cases[i].a);
        printf("Input B: %.9g\n", test_cases[i].b);
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
        } else if (!float_equal(result, test_cases[i].expected, 0.0001f)) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result, test_cases[i].expected);
        }
        printf("\n");
    }
}

int main() {
    test_vmulss();
    printf("VMULSS tests completed\n");
    return 0;
}
