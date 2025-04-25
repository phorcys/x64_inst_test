#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fprem() {
    printf("Testing FPREM instruction\n");
    printf("FPREM computes partial remainder\n");

    struct {
        long double dividend;
        long double divisor;
    } test_cases[] = {
        {10.0L, 3.0L},   // 10 % 3 = 1
        {15.0L, 4.0L},   // 15 % 4 = 3
        {7.0L, 2.5L},    // 7 % 2.5 = 2.0
        {1.0L, 0.3L}     // 1 % 0.3 ≈ 0.1
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double dividend = test_cases[i].dividend;
        long double divisor = test_cases[i].divisor;
        long double result;
        long double expected = fmodl(dividend, divisor);

        // 加载操作数并执行FPREM
        asm volatile ("fldt %2\n\t"   // 加载除数到ST(0)
                     "fldt %1\n\t"   // 加载被除数到ST(0), 除数移动到ST(1)
                     "fprem\n\t"     // 计算部分余数
                     "fstpt %0\n\t"  // 存储结果
                     : "=m" (result)
                     : "m" (dividend), "m" (divisor)
                     : "memory", "st", "st(1)");

        printf("\nTest case %zu:\n", i+1);
        printf("Dividend: ");
        print_float80(dividend);
        printf("Divisor: ");
        print_float80(divisor);
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

    printf("\nFPREM test completed\n");
}

int main() {
    test_fprem();
    return 0;
}
