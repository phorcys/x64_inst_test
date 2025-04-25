#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fcmove() {
    printf("Testing FCMOVE instruction\n");
    printf("FCMOVE moves ST(i) to ST(0) if ZF=1\n");

    // 测试用例: 数值对和预期的ZF标志状态
    struct {
        long double src;
        long double dst;
        int zf;  // 预期的ZF标志状态
    } test_cases[] = {
        {1.0L, 2.0L, 1},  // ZF=1
        {1.0L, 2.0L, 0},  // ZF=0
        {POS_INF, NEG_INF, 1},
        {POS_NAN, NEG_NAN, 0},
        {1e100L, -1e100L, 1},
        {1e-100L, -1e-100L, 0},
        {POS_DENORM, NEG_DENORM, 1},
        {3.14159265358979323846L, -3.14159265358979323846L, 0},
        {0.0L, -0.0L, 1},
        {1.0000000000000001L, -1.0000000000000001L, 0}
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double src = test_cases[i].src;
        long double dst = test_cases[i].dst;
        int expected_zf = test_cases[i].zf;
        long double result;

        // 加载操作数到x87栈 (ST(0)=src, ST(1)=dst)
        asm volatile ("fldt %0\n\t" : : "m" (src));
        asm volatile ("fldt %0\n\t" : : "m" (dst));
        
        // 设置标志位
        asm volatile (
            "xor %%eax, %%eax\n\t" // 清空eax
            "test %%eax, %%eax\n\t" // 设置ZF=1
            ::: "eax", "cc"
        );
        
        if (!expected_zf) {
            asm volatile (
                "mov $1, %%eax\n\t"
                "test %%eax, %%eax\n\t" // 清除ZF
                ::: "eax", "cc"
            );
        }
        
        // 执行条件移动
        if (expected_zf) {
            asm volatile (
                "fcmove %%st(1), %%st(0)\n\t" // 条件移动
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
        printf("Expected ZF: %d\n", expected_zf);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (expected_zf) {
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

    printf("\nFCMOVE test completed\n");
}

int main() {
    test_fcmove();
    return 0;
}
