#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSUBSD - Subtract Scalar Double-Precision Floating-Point Values
// Performs subtraction on scalar double-precision floating-point values

static void test_vsubsd() {
    printf("Testing VSUBSD\n");
    
    // 测试用例
    struct TestCase {
        double input1;
        double input2;
        double expected;
    } test_cases[] = {
        {1.0, 0.5, 0.5},       // 基本减法
        {4.0, 2.0, 2.0},       // 整数减法
        {1.5e300, 0.5e300, 1.0e300},  // 大数减法
        {1.5e-300, 0.5e-300, 1.0e-300}, // 小数减法
        {0.0, 0.0, 0.0},       // 零减零
        {-1.0, 0.5, -1.5},     // 负数减法
        {INFINITY, 1.0, INFINITY}, // 无穷大减有限数
        {NAN, 1.0, NAN},       // NaN减有限数
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double result = 0.0;
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vsubsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(test_cases[i].input1), "m"(test_cases[i].input2)
            : "xmm0", "xmm1", "xmm2"
        );

        printf("Test case %zu:\n", i+1);
        printf("Input1: %.17g\n", test_cases[i].input1);
        printf("Input2: %.17g\n", test_cases[i].input2);
        printf("Result: %.17g\n", result);
        printf("Expected: %.17g\n", test_cases[i].expected);
        
        if (isnan(test_cases[i].expected)) {
            if (!isnan(result)) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (fabs(result - test_cases[i].expected) > 0.0000001) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result, test_cases[i].expected);
        }
        printf("\n");
    }
}

int main() {
    test_vsubsd();
    printf("VSUBSD tests completed\n");
    return 0;
}
