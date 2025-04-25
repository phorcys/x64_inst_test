#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fabs() {
    long double test_values[] = {
        POS_ZERO, NEG_ZERO,            // 零值
        POS_ONE, NEG_ONE,              // 正常值
        0.5L, -0.5L,                   // 小数
        1e-10L, -1e-10L,               // 接近零的小数
        1e10L, -1e10L,                 // 大数
        1e100L, -1e100L,               // 极大数
        1e-100L, -1e-100L,             // 极小数
        POS_DENORM, NEG_DENORM,        // 非正规数
        POS_INF, NEG_INF,              // 无穷大
        POS_NAN, NEG_NAN               // NaN
    };

    printf("Testing FABS instruction\n");
    printf("FABS computes absolute value of ST(0)\n");

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        long double x = test_values[i];
        long double result;

        // 测试FABS指令
        asm volatile (
            "fldt %[x]\n\t"      // 加载x到ST(0)
            "fabs\n\t"           // ST(0) = |ST(0)|
            "fstpt %[res]\n\t"   // 存储结果
            : [res] "=m" (result)
            : [x] "m" (x)
            : "memory"
        );

        printf("\nTest case %zu:\n", i+1);
        printf("Input: ");
        print_float80(x);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (isnan(x)) {
            if (!isnan(result)) {
                printf("FAIL: Expected NaN but got ");
                print_float80(result);
                passed = 0;
            }
        } else {
            long double expected = fabsl(x);
            if (result != expected) {
                printf("FAIL: Expected ");
                print_float80(expected);
                printf("       Got ");
                print_float80(result);
                passed = 0;
            }
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nFABS test completed\n");
}

int main() {
    test_fabs();
    return 0;
}
