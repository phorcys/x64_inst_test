#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fcmovbe() {
    printf("Testing FCMOVBE instruction\n");
    printf("FCMOVBE moves ST(i) to ST(0) if CF=1 or ZF=1\n");

    // 测试用例: 数值对和预期的标志状态
    struct {
        long double src;
        long double dst;
        int cf;  // 预期的CF标志状态
        int zf;  // 预期的ZF标志状态
    } test_cases[] = {
        {1.0L, 2.0L, 1, 0},  // CF=1, ZF=0
        {1.0L, 2.0L, 0, 1},  // CF=0, ZF=1
        {1.0L, 2.0L, 1, 1},  // CF=1, ZF=1
        {1.0L, 2.0L, 0, 0},  // CF=0, ZF=0
        {POS_INF, NEG_INF, 1, 0},  // 无穷大
        {POS_NAN, NEG_NAN, 0, 1},  // NaN
        {1e100L, -1e100L, 1, 0},   // 极大数
        {1e-100L, -1e-100L, 0, 1}, // 极小数
        {POS_DENORM, NEG_DENORM, 1, 0}, // 非正规数
        {3.14159265358979323846L, -3.14159265358979323846L, 0, 0}, // π值
        {0.0L, -0.0L, 1, 1},       // 零值
        {1.0000000000000001L, -1.0000000000000001L, 0, 0} // 精度边界
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
        
        // 设置标志位
        asm volatile (
            "xor %%eax, %%eax\n\t" // 清空eax
            "test %%eax, %%eax\n\t" // 设置ZF=1, CF=0
            ::: "eax", "cc"
        );
        
        if (expected_cf) {
            asm volatile ("stc\n\t"); // 设置CF=1
        } else {
            asm volatile ("clc\n\t"); // 清除CF=0
        }
        
        if (!expected_zf) {
            asm volatile (
                "mov $1, %%eax\n\t"
                "test %%eax, %%eax\n\t" // 清除ZF
                ::: "eax", "cc"
            );
        }
        
        // 设置标志位后执行指令
        if (expected_cf || expected_zf) {
            asm volatile (
                "fcmovbe %%st(1), %%st\n\t" // 执行移动
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
        if (expected_cf || expected_zf) {
            // 当CF=1或ZF=1时，应移动ST(1)到ST(0)，所以结果应为dst
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
            // 当CF=0且ZF=0时，不应移动，结果应为src
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

    printf("\nFCMOVBE test completed\n");
}

int main() {
    test_fcmovbe();
    return 0;
}
