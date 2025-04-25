#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fyl2xp1() {
    printf("Testing FYL2XP1 instruction\n");
    printf("FYL2XP1 computes y * log2(x + 1)\n");

    struct {
        long double x;
        long double y;
    } test_cases[] = {
        {1.0L, 1.0L},    // log2(1+1) = 1
        {3.0L, 1.0L},    // log2(3+1) = 2
        {0.5L, 1.0L},    // log2(0.5+1) ≈ 0.58496
        {0.0L, 5.0L}     // log2(0+1) = 0
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double x = test_cases[i].x;
        long double y = test_cases[i].y;
        long double result;
        long double expected = y * log2l(x + 1.0L);

        // 加载操作数并执行FYL2XP1
        asm volatile ("fldt %2\n\t"   // 加载y到ST(0)
                     "fldt %1\n\t"   // 加载x到ST(0), y移动到ST(1)
                     "fyl2xp1\n\t"   // 计算y*log2(x+1)
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

    printf("\nFYL2XP1 test completed\n");
}

int main() {
    test_fyl2xp1();
    return 0;
}
