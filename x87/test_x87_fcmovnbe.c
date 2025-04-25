#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fcmovnbe() {
    printf("Testing FCMOVNBE instruction\n");
    printf("FCMOVNBE moves ST(i) to ST(0) if CF=0 and ZF=0\n");

    // 测试用例: 数值对和预期的CF/ZF标志状态
    struct {
        long double src;
        long double dst;
        int cf;  // 预期的CF标志状态
        int zf;  // 预期的ZF标志状态
    } test_cases[] = {
        {1.0L, 2.0L, 0, 0},  // CF=0,ZF=0
        {1.0L, 1.0L, 0, 1},  // CF=0,ZF=1
        {1.0L, 2.0L, 1, 0},  // CF=1,ZF=0
        {POS_INF, NEG_INF, 0, 0},
        {POS_NAN, NEG_NAN, 1, 1},
        {1e100L, -1e100L, 0, 0},
        {1e-100L, -1e-100L, 1, 1},
        {POS_DENORM, NEG_DENORM, 0, 0},
        {3.14159265358979323846L, -3.14159265358979323846L, 1, 0},
        {0.0L, -0.0L, 0, 1},
        {1.0000000000000001L, -1.0000000000000001L, 1, 0}
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double src = test_cases[i].src;
        long double dst = test_cases[i].dst;
        int expected_cf = test_cases[i].cf;
        int expected_zf = test_cases[i].zf;
        long double result;

        // 加载操作数到x87栈 (ST(0)=src, ST(1)=dst)
        asm volatile ("fldt %0\n\t" : : "m" (src));
        asm volatile ("fldt %0\n\t" : : "m" (dst));
        
        // 设置标志位并执行条件移动
        if (!expected_cf && !expected_zf) {
            asm volatile (
                "fcmovnbe %%st(1), %%st(0)\n\t"  // 条件移动
                "fstpt %0\n\t"   // 存储结果
                : "=m" (result)
                :
                : "memory", "st", "st(1)"
            );
        } else {
            asm volatile (
                "fstp %%st(0)\n\t" // 弹出ST(0)保留ST(1)
                "fldt %1\n\t"    // 重新加载原始值到ST(0)
                "fstpt %0\n\t"   // 存储结果
                : "=m" (result)
                : "m" (src)
                : "memory", "st", "st(1)"
            );
        }

        printf("\nTest case %zu:\n", i+1);
        printf("Source: ");
        print_float80(src);
        printf("Destination: ");
        print_float80(dst);
        printf("Expected CF: %d, ZF: %d\n", expected_cf, expected_zf);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (!expected_cf && !expected_zf) {
            if (isnan(dst)) {
                if (!isnan(result)) {
                    printf("FAIL: Expected NaN but got ");
                    print_float80(result);
                    passed = 0;
                }
            } else if (result != dst) {
                printf("FAIL: Expected ");
                print_float80(dst);
                printf("       Got ");
                print_float80(result);
                passed = 0;
            }
        } else {
            if (isnan(src)) {
                if (!isnan(result)) {
                    printf("FAIL: Expected NaN but got ");
                    print_float80(result);
                    passed = 0;
                }
            } else if (result != src) {
                printf("FAIL: Expected ");
                print_float80(src);
                printf("       Got ");
                print_float80(result);
                passed = 0;
            }
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nFCMOVNBE test completed\n");
}

int main() {
    test_fcmovnbe();
    return 0;
}
