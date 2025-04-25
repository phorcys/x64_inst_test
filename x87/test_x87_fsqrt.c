#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fsqrt() {
    printf("Testing FSQRT instruction\n");
    printf("FSQRT computes square root of ST(0)\n");

    long double test_values[] = {
        0.0L,
        1.0L,
        2.0L,
        4.0L,
        9.0L,
        -1.0L  // 测试负数情况
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        long double value = test_values[i];
        long double result;
        long double expected = sqrtl(value);

        // 加载操作数并执行FSQRT
        asm volatile ("fldt %1\n\t"
                     "fsqrt\n\t"
                     "fstpt %0\n\t"
                     : "=m" (result)
                     : "m" (value)
                     : "memory", "st");

        printf("\nTest case %zu:\n", i+1);
        printf("Input: ");
        print_float80(value);
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
        } else if (fabsl(result - expected) > 1e-10L) {
            printf("FAIL: Expected ");
            print_float80(expected);
            printf("       Got ");
            print_float80(result);
            passed = 0;
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nFSQRT test completed\n");
}

int main() {
    test_fsqrt();
    return 0;
}
