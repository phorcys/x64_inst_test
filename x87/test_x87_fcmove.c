#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include "x87.h"

void test_fcmove() {
    printf("Testing FCMOVE instruction\n");
    printf("FCMOVE moves ST(i) to ST(0) if ZF=1\n");

    struct {
        long double st0;
        long double sti;
        uint8_t zf;
        long double expected;
    } test_cases[] = {
        {1.0L, 2.0L, 1, 2.0L},    // ZF=1, 应该移动
        {1.0L, 2.0L, 0, 1.0L},    // ZF=0, 不应该移动
        {POS_INF, NEG_INF, 1, NEG_INF},
        {NEG_INF, POS_INF, 0, NEG_INF},
        {POS_NAN, 1.0L, 1, 1.0L},
        {1.0L, POS_NAN, 0, 1.0L}
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double st0 = test_cases[i].st0;
        long double sti = test_cases[i].sti;
        long double result;
        uint8_t zf = test_cases[i].zf;

        // 设置ZF标志
        asm volatile (
            "pushfq\n\t"
            "andl $0xffffffbf, (%%rsp)\n\t"  // 清除ZF
            "orb %b[flag], (%%rsp)\n\t"      // 设置ZF
            "popfq"
            : 
            : [flag] "q" (zf << 6)
            : "memory"
        );

        // 测试FCMOVE指令
        asm volatile (
            "fwait\n\t"          // 确保同步
            "fldt %[sti]\n\t"    // 加载到ST(1)
            "fldt %[st0]\n\t"    // 加载到ST(0)
            "fcmove %%st(1), %%st(0)\n\t" // 条件移动
            "fstpt %[res]\n\t"   // 存储结果
            "fstp %%st(0)\n\t"   // 弹出ST(0)保持栈平衡
            "fwait\n\t"          // 确保完成
            : [res] "=m" (result)
            : [st0] "m" (st0), [sti] "m" (sti)
        );

        printf("\nTest case %zu:\n", i+1);
        printf("ST(0): ");
        print_float80(st0);
        printf("ST(i): ");
        print_float80(sti);
        printf("ZF: %d\n", zf);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (isnan(test_cases[i].expected)) {
            if (!isnan(result)) {
                printf("FAIL: Expected NaN but got ");
                print_float80(result);
                passed = 0;
            }
        } else if (result != test_cases[i].expected) {
            printf("FAIL: Expected ");
            print_float80(test_cases[i].expected);
            printf("       Got ");
            print_float80(result);
            passed = 0;
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nFCMOVE test completed\n");
}

int main() {
    init_x87_env();
    test_fcmove();
    return 0;
}
