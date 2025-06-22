#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMINSS - Scalar Single-FP Minimum
// Compares scalar single-precision floating-point values and returns the minimum value

static void test_vminss() {
    printf("Testing VMINSS\n");
    
    struct TestCase {
        float a;
        float b;
        float expected;
    } test_cases[] = {
        {1.5f, -1.0f, -1.0f},       // 基本测试
        {-2.5f, 3.0f, -2.5f},       // 负数 vs 正数
        {0.0f, -0.0f, -0.0f},       // 零值测试
        {NAN, 1.0f, 1.0f},          // NaN处理
        {INFINITY, 1.0f, 1.0f},     // 无穷大测试
        {-INFINITY, -1.0f, -INFINITY}, // 负无穷测试
        {3.402823466e+38f, -3.402823466e+38f, -3.402823466e+38f} // 边界值测试
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        float a = test_cases[i].a;
        float b = test_cases[i].b;
        float expected = test_cases[i].expected;
        float result;

        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vminss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(a), "m"(b)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input A: %.9g\n", a);
        printf("Input B: %.9g\n", b);
        printf("Result: %.9g\n", result);
        printf("Expected: %.9g\n", expected);

        if (isnan(expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN, got %.9g\n", result);
            }
        } else if (isinf(expected) && isinf(result)) {
            // 无穷大比较符号
            if (signbit(expected) != signbit(result)) {
                printf("Mismatch: sign differs for infinity\n");
            }
        } else if (!float_equal(result, expected, 0.0001f)) {
            printf("Mismatch: got %.9g, expected %.9g\n", result, expected);
        }
        printf("\n");
    }
}

int main() {
    test_vminss();
    printf("VMINSS tests completed\n");
    return 0;
}
