#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fcmovb() {
    printf("Testing FCMOVB instruction\n");
    printf("FCMOVB moves ST(i) to ST(0) if CF=1\n");

    // 测试用例: 数值对和预期的CF标志状态
    struct {
        long double src;
        long double dst;
        int cf;  // 预期的CF标志状态
    } test_cases[] = {
        {1.0L, 2.0L, 1},          // CF=1的情况
        {1.0L, 2.0L, 0},          // CF=0的情况
        {POS_INF, NEG_INF, 1},     // 无穷大
        {POS_NAN, NEG_NAN, 0},     // NaN
        {1e100L, -1e100L, 1},      // 极大数
        {1e-100L, -1e-100L, 0},    // 极小数
        {POS_DENORM, NEG_DENORM, 1}, // 非正规数
        {3.14159265358979323846L, -3.14159265358979323846L, 0}, // π值
        {0.0L, -0.0L, 1},          // 零值
        {1.0000000000000001L, -1.0000000000000001L, 0} // 精度边界
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double src = test_cases[i].src;
        long double dst = test_cases[i].dst;
        int expected_cf = test_cases[i].cf;
        long double result;

        // 加载操作数到x87栈
        asm volatile ("fldt %0\n\t" : : "m" (src));
        asm volatile ("fldt %0\n\t" : : "m" (dst));
        
        // 设置CF标志
        asm volatile (
            "mov $0, %%eax\n\t" // 清空eax
            "add $0, %%eax\n\t" // 设置ZF=1, CF=0
            ::: "eax", "cc"
        );
        
        if (expected_cf) {
            asm volatile ("stc\n\t"); // 设置CF=1
        } else {
            asm volatile ("clc\n\t"); // 清除CF=0
        }
        
        // 执行条件移动
        asm volatile (
            "fcmovb %%st(1), %%st(0)\n\t" // 条件移动
            "fstpt %0\n\t"   // 存储结果
            : "=m" (result)
            :
            : "memory", "cc", "st", "st(1)"
        );

        printf("\nTest case %zu:\n", i+1);
        printf("Source: ");
        print_float80(src);
        printf("Destination: ");
        print_float80(dst);
        printf("Expected CF: %d\n", expected_cf);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (expected_cf) {
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
        } else {
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
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nFCMOVB test completed\n");
}

int main() {
    test_fcmovb();
    return 0;
}
