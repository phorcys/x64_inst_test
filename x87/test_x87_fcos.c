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
        2.0L * M_PI,
        -0.0L,
        -M_PI_2,
        -M_PI,
        M_PI_4,
        3.0L * M_PI_2,
        1.0e10L,  // Large value within range
        1.0e20L,  // Very large value within range
        9.223372036854775808e18L,  // 2^63-1 (near upper bound)
        -9.223372036854775808e18L, // -2^63+1 (near lower bound)
        POS_INF,
        NEG_INF,
        POS_NAN,
        NEG_NAN,
        POS_DENORM,
        NEG_DENORM
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
        // 检查C2标志(超出范围)
        uint16_t status;
        asm volatile ("fnstsw %0" : "=m" (status));
        int c2 = (status >> 10) & 1;

        printf("Result: ");
        print_float80(result);
        printf("C2 flag (out of range): %d\n", c2);
        print_x87_status();

        // 验证结果
        int passed = 1;
        
        // 对于极大值(>1e18且非无穷大)，检查C2标志
        if (fabsl(angle) > 1e18L && !isinf(angle)) {
            if (!c2) {
                printf("FAIL: C2 flag not set for large angle\n");
                passed = 0;
            } else {
                // C2=1时跳过结果比较(指令规范允许保持原值)
                expected = result;
            }
        }

        if (isnan(expected)) {
            if (!isnan(result)) {
                printf("FAIL: Expected NaN but got ");
                print_float80(result);
                passed = 0;
            }
        } else if (isinf(angle)) {
            if (!isnan(result)) {
                printf("FAIL: Expected NaN for INF input but got ");
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
