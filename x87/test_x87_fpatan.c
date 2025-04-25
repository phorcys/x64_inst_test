#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fpatan() {
    printf("Testing FPATAN instruction\n");
    printf("FPATAN computes arctangent of ST(1)/ST(0)\n");

    struct {
        long double y;
        long double x;
    } test_cases[] = {
        {0.0L, 1.0L},  // atan(0/1)
        {1.0L, 1.0L},  // atan(1/1)
        {1.0L, 0.0L},  // atan(1/0) = π/2
        {-1.0L, -1.0L} // atan(-1/-1)
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double y = test_cases[i].y;
        long double x = test_cases[i].x;
        long double result;
        long double expected = atan2l(y, x);

        // 加载操作数并执行FPATAN
        asm volatile ("fldt %2\n\t"   // 加载y到ST(0)
                     "fldt %1\n\t"   // 加载x到ST(0), y移动到ST(1)
                     "fpatan\n\t"    // 计算atan2(y,x)
                     "fstpt %0\n\t"  // 存储结果
                     : "=m" (result)
                     : "m" (x), "m" (y)
                     : "memory", "st", "st(1)");

        printf("\nTest case %zu:\n", i+1);
        printf("y: ");
        print_float80(y);
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

    printf("\nFPATAN test completed\n");
}

int main() {
    test_fpatan();
    return 0;
}
