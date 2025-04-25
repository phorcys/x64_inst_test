#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fadd_reg() {
    printf("\n=== Test 1: FADD with register operands ===\n");
    
    struct {
        long double a;
        long double b;
        long double expected;
    } test_cases[] = {
        {1.0L, 1.0L, 2.0L},          // 正常加法
        {-1.0L, 1.0L, 0.0L},         // 正负相加
        {POS_ZERO, NEG_ZERO, 0.0L},   // 零值相加
        {POS_INF, POS_INF, POS_INF},  // 无穷大相加
        {POS_INF, NEG_INF, POS_NAN},  // 正负无穷相加
        {POS_NAN, 1.0L, POS_NAN},     // NaN相加
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double a = test_cases[i].a;
        long double b = test_cases[i].b;
        long double result;

        // 测试FADD指令(寄存器形式: ST(1) = ST(0) + ST(1))
        asm volatile (
            "fldt %[b]\n\t"      // 加载b到ST(0)
            "fldt %[a]\n\t"      // 加载a到ST(0), b移动到ST(1)
            "faddp\n\t"          // ST(0) = ST(0) + ST(1), 弹出栈
            "fstpt %[res]\n\t"   // 存储结果
            : [res] "=m" (result)
            : [a] "m" (a), [b] "m" (b)
            : "memory"
        );

        printf("\nTest case %zu:\n", i+1);
        printf("Operand A: ");
        print_float80(a);
        printf("Operand B: ");
        print_float80(b);
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
}

void test_fadd_mem() {
    printf("\n=== Test 2: FADD with memory operands ===\n");
    
    struct {
        long double a;
        long double b;
        long double expected;
    } test_cases[] = {
        {0.5L, 0.25L, 0.75L},        // 简单加法
        {1e20L, 1e-20L, 1e20L}       // 大数加小数
        //{POS_DENORM, POS_DENORM, 2*POS_DENORM}, // 非正规数相加(暂时注释)
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double a = test_cases[i].a;
        long double b = test_cases[i].b;
        long double result;

        // 测试FADD指令(内存形式: ST(0) = ST(0) + mem)
        asm volatile (
            "fldt %[a]\n\t"      // 加载a到ST(0)
            "fldt %[b]\n\t"      // 加载b到ST(0)
            "faddp %%st, %%st(1)\n\t"  // ST(1) = ST(0) + ST(1), 弹出栈
            "fstpt %[res]\n\t"   // 存储结果
            : [res] "=m" (result)
            : [a] "m" (a), [b] "m" (b)
            : "st", "st(1)", "memory"
        );

        printf("\nTest case %zu:\n", i+1);
        printf("Operand A: ");
        print_float80(a);
        printf("Operand B: ");
        print_float80(b);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (result != test_cases[i].expected) {
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
}

int main() {
    printf("Testing FADD instruction\n");
    test_fadd_reg();
    test_fadd_mem();
    printf("\nFADD test completed\n");
    return 0;
}
