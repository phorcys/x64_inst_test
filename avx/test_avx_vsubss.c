#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSUBSS - Subtract Scalar Single-Precision Floating-Point Values
// Performs subtraction on scalar single-precision floating-point values

static void test_vsubss() {
    printf("Testing VSUBSS\n");
    
    // 测试用例
    struct TestCase {
        float input1;
        float input2;
        float expected;
    } test_cases[] = {
        {1.0f, 0.5f, 0.5f},       // 基本减法
        {4.0f, 2.0f, 2.0f},       // 整数减法
        {1.5e38f, 0.5e38f, 1.0e38f},  // 大数减法(允许一定精度误差)
        {1.5e-38f, 0.5e-38f, 1.0e-38f}, // 小数减法(允许一定精度误差)
        {0.0f, 0.0f, 0.0f},       // 零减零
        {-1.0f, 0.5f, -1.5f},     // 负数减法
        {INFINITY, 1.0f, INFINITY}, // 无穷大减有限数
        {NAN, 1.0f, NAN},       // NaN减有限数
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        float result = 0.0f;
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vsubss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(test_cases[i].input1), "m"(test_cases[i].input2)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input1: %.9g\n", test_cases[i].input1);
        printf("Input2: %.9g\n", test_cases[i].input2);
        printf("Result: %.9g\n", result);
        printf("Expected: %.9g\n", test_cases[i].expected);
        
        if (isnan(test_cases[i].expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (fabsf(result - test_cases[i].expected) > fabsf(test_cases[i].expected * 0.0001f)) {
            printf("Mismatch: got %.9g, expected %.9g (relative error: %.9g)\n", 
                  result, test_cases[i].expected, fabsf((result - test_cases[i].expected)/test_cases[i].expected));
        }
        printf("\n");
    }
}

int main() {
    test_vsubss();
    printf("VSUBSS tests completed\n");
    return 0;
}
