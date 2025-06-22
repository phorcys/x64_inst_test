#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMINSD - Scalar Double-FP Minimum
// Compares scalar double-precision floating-point values and returns the minimum value

static void test_vminsd() {
    printf("Testing VMINSD\n");
    
    struct TestCase {
        double a;
        double b;
        double expected;
    } test_cases[] = {
        {1.5, -1.0, -1.0},       // 基本测试
        {-2.5, 3.0, -2.5},       // 负数 vs 正数
        {0.0, -0.0, -0.0},       // 零值测试
        {NAN, 1.0, 1.0},         // NaN处理
        {INFINITY, 1.0, 1.0},    // 无穷大测试
        {-INFINITY, -1.0, -INFINITY}, // 负无穷测试
        {1.7976931348623157e+308, -1.7976931348623157e+308, -1.7976931348623157e+308} // 边界值测试
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double a = test_cases[i].a;
        double b = test_cases[i].b;
        double expected = test_cases[i].expected;
        double result;

        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vminsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(a), "m"(b)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input A: %.17g\n", a);
        printf("Input B: %.17g\n", b);
        printf("Result: %.17g\n", result);
        printf("Expected: %.17g\n", expected);

        if (isnan(expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN, got %.17g\n", result);
            }
        } else if (isinf(expected) && isinf(result)) {
            // 无穷大比较符号
            if (signbit(expected) != signbit(result)) {
                printf("Mismatch: sign differs for infinity\n");
            }
        } else if (!double_equal(result, expected, 0.0001)) {
            printf("Mismatch: got %.17g, expected %.17g\n", result, expected);
        }
        printf("\n");
    }
}

int main() {
    test_vminsd();
    printf("VMINSD tests completed\n");
    return 0;
}
