#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fcos() {
    printf("Testing FCOS instruction\n");
    printf("FCOS computes cosine of ST(0)\n");

    long double test_values[] = {
        0.0L,
        M_PI_2,
        M_PI,
        2.0L * M_PI
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        long double angle = test_values[i];
        long double result;
        long double expected = cosl(angle);

        // 加载操作数并执行FCOS
        asm volatile ("fldt %1\n\t"   // 加载角度到ST(0)
                     "fcos\n\t"      // 计算余弦
                     "fstpt %0\n\t"  // 存储结果
                     : "=m" (result)
                     : "m" (angle)
                     : "memory", "st");

        printf("\nTest case %zu:\n", i+1);
        printf("Angle: ");
        print_float80(angle);
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

    printf("\nFCOS test completed\n");
}

int main() {
    test_fcos();
    return 0;
}
