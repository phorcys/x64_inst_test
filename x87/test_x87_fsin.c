#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fsin() {
    printf("Testing FSIN instruction\n");
    printf("FSIN computes sine of ST(0)\n");

    long double test_values[] = {
        0.0L,
        M_PI_2,
        M_PI,
        3.0L * M_PI_2
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        long double angle = test_values[i];
        long double result;
        long double expected = sinl(angle);

        // 加载操作数并执行FSIN
        asm volatile ("fldt %1\n\t"   // 加载角度到ST(0)
                     "fsin\n\t"      // 计算正弦
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

    printf("\nFSIN test completed\n");
}

int main() {
    test_fsin();
    return 0;
}
