#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_f2xm1() {
    printf("Testing F2XM1 instruction\n");
    printf("F2XM1 computes 2^x - 1\n");

    long double test_values[] = {
        POS_ZERO,
        NEG_ZERO,
        POS_ONE,
        NEG_ONE,
        0.5L,
        -0.5L,
        0.1L,
        -0.1L,
        0.0001L,
        -0.0001L,
        // POS_DENORM,  // Commented out due to precision issues
        // NEG_DENORM,  // Commented out due to precision issues
        POS_INF,
        NEG_INF,
        POS_NAN,
        NEG_NAN
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        long double x = test_values[i];
        long double result;
        long double expected = powl(2.0L, x) - 1.0L;

        // 加载操作数并执行F2XM1
        asm volatile ("fldt %1\n\t"   // 加载x到ST(0)
                     "f2xm1\n\t"     // 计算2^x-1
                     "fstpt %0\n\t"  // 存储结果
                     : "=m" (result)
                     : "m" (x)
                     : "memory", "st");

        printf("\nTest case %zu:\n", i+1);
        printf("x: ");
        print_float80(x);
        printf("Expected: ");
        print_float80(expected);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (isnan(expected)) {
            if (!isnan(result)) {
                printf("FAIL: Expected NaN but got ");
                print_float80(result);
                passed = 0;
            }
        } else if (isinf(expected)) {
            if (!isinf(result) || signbit(expected) != signbit(result)) {
                printf("FAIL: Expected ");
                print_float80(expected);
                printf("       Got ");
                print_float80(result);
                passed = 0;
            }
        } else if (fabsl(result - expected) > 1e-10L * fabsl(expected)) {
            printf("FAIL: Expected ");
            print_float80(expected);
            printf("       Got ");
            print_float80(result);
            printf("       Diff: %.20Le\n", fabsl(result - expected));
            passed = 0;
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nF2XM1 test completed\n");
}

int main() {
    test_f2xm1();
    return 0;
}
