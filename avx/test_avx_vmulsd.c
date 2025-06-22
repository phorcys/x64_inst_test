#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMULSD - Scalar Double-FP Multiply
// Multiplies scalar double-precision floating-point values

static void test_vmulsd() {
    printf("Testing VMULSD\n");
    
    struct TestCase {
        double a;
        double b;
        double expected;
    } test_cases[] = {
        {1.5, 2.0, 3.0},  // 基本测试
        {0.0, 1.0, 0.0},  // 零值测试
        {-0.0, -1.0, 0.0}, // 负零测试
        {INFINITY, 1.0, INFINITY}, // 无穷大测试
        {-INFINITY, 1.0, -INFINITY},
        {NAN, 1.0, NAN},  // NaN测试
        {1.0, NAN, NAN},
        {1.7976931348623157e+308, 2.0, INFINITY}, // 溢出测试
        {-1.7976931348623157e+308, 2.0, -INFINITY},
        {2.2250738585072014e-308, 0.5, 1.1125369292536007e-308} // 下溢测试
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double result = 0;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmulsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(test_cases[i].a), "m"(test_cases[i].b)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input A: %.17g\n", test_cases[i].a);
        printf("Input B: %.17g\n", test_cases[i].b);
        printf("Result: %.17g\n", result);
        printf("Expected: %.17g\n", test_cases[i].expected);

        if (isnan(test_cases[i].expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (isinf(test_cases[i].expected) && isinf(result)) {
            if (signbit(test_cases[i].expected) != signbit(result)) {
                printf("Mismatch: sign differs for infinity\n");
            }
        } else if (!double_equal(result, test_cases[i].expected, 0.0001)) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result, test_cases[i].expected);
        }
        printf("\n");
    }
}

int main() {
    test_vmulsd();
    printf("VMULSD tests completed\n");
    return 0;
}
