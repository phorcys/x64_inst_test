#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include "x87.h"

void test_fcmovb() {
    printf("Testing FCMOVB instruction\n");
    printf("FCMOVB moves ST(i) to ST(0) if CF=1\n");

    struct {
        long double st0;
        long double sti;
        uint8_t cf;
        long double expected;
    } test_cases[] = {
        {1.0L, 2.0L, 1, 2.0L},    // CF=1, 应该移动
        {1.0L, 2.0L, 0, 1.0L},    // CF=0, 不应该移动
        {POS_INF, NEG_INF, 1, NEG_INF}, 
        {NEG_INF, POS_INF, 0, NEG_INF},
        {POS_NAN, 1.0L, 1, 1.0L},
        {1.0L, POS_NAN, 0, 1.0L}
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double st0 = test_cases[i].st0;
        long double sti = test_cases[i].sti;
        long double result;
        uint8_t cf = test_cases[i].cf;

        // 设置CF标志
        asm volatile (
            "pushfq\n\t"
            "andl $0xfffffffe, (%%rsp)\n\t"  // 清除CF
            "orb %[cf], (%%rsp)\n\t"         // 设置CF
            "popfq"
            : 
            : [cf] "r" (cf)
            : "memory"
        );

        // 测试FCMOVB指令
        asm volatile (
            "fwait\n\t"          // 确保同步
            "fldt %[sti]\n\t"    // 加载到ST(1)
            "fldt %[st0]\n\t"    // 加载到ST(0)
            "fcmovb %%st(1), %%st(0)\n\t" // 条件移动
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
        printf("CF: %d\n", cf);
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

    printf("\nFCMOVB test completed\n");
}

int main() {
    init_x87_env();
    test_fcmovb();
    return 0;
}
