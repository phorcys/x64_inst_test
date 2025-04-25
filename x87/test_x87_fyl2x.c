#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fyl2x() {
    printf("Testing FYL2X instruction\n");
    printf("FYL2X computes y * log2(x)\n");

    struct {
        long double x;
        long double y;
    } test_cases[] = {
        {2.0L, 1.0L},    // log2(2) = 1
        {8.0L, 1.0L},    // log2(8) = 3
        {0.5L, 1.0L},    // log2(0.5) = -1
        {1.0L, 5.0L}     // log2(1) = 0
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double x = test_cases[i].x;
        long double y = test_cases[i].y;
        long double result;
        long double expected = y * log2l(x);

        // 加载操作数并执行FYL2X
        asm volatile ("fldt %2\n\t"   // 加载y到ST(0)
                     "fldt %1\n\t"   // 加载x到ST(0), y移动到ST(1)
                     "fyl2x\n\t"     // 计算y*log2(x)
                     "fstpt %0\n\t"  // 存储结果
                     : "=m" (result)
                     : "m" (x), "m" (y)
                     : "memory", "st", "st(1)");

        printf("\nTest case %zu:\n", i+1);
        printf("x: ");
        print_float80(x);
        printf("y: ");
        print_float80(y);
        printf("Expected: ");
        print_float80(expected);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (fabsl(result - expected) > 1e-10L) {
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

    printf("\nFYL2X test completed\n");
}

int main() {
    test_fyl2x();
    return 0;
}
