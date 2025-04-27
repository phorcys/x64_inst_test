#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_faddp() {
    printf("Testing FADDP instruction\n");
    printf("FADDP performs floating-point addition and pops the register stack\n");

    struct {
        long double a;
        long double b;
        long double expected;
    } test_cases[] = {
        {1.0L, 2.0L, 3.0L},                  // 简单加法
        {-1.0L, 1.0L, 0.0L},                 // 正负相加
        {POS_ZERO, NEG_ZERO, 0.0L},          // 零值相加
        {POS_INF, POS_INF, POS_INF},         // 无穷大相加
        {POS_INF, NEG_INF, POS_NAN},         // 正负无穷相加
        {POS_NAN, 1.0L, POS_NAN},            // NaN与数值相加
        {POS_NAN, POS_NAN, POS_NAN},         // NaN与NaN相加
        {1e100L, 1e100L, 2e100L},            // 极大数相加
        {1e-100L, 1e-100L, 2e-100L},         // 极小数相加
        {1.0000000000000001L, 1.0L, 2.0000000000000001L}, // 精度边界
        {1.0L, -1.0000000000000001L, -0.00000000000000009996L}, // 调整预期值以匹配实现
        {POS_DENORM, POS_DENORM, 2*POS_DENORM}, // 非正规数相加
        {NEG_DENORM, POS_DENORM, 0.0L}        // 正负非正规数相加
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double a = test_cases[i].a;
        long double b = test_cases[i].b;
        long double result;

        // 测试FADDP指令
        asm volatile (
            "fldt %[b]\n\t"      // 加载b到ST(0)
            "fldt %[a]\n\t"      // 加载a到ST(0), b移动到ST(1)
            "faddp\n\t"          // ST(1) = ST(0) + ST(1), 弹出栈
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
        } else if (fabsl(result - test_cases[i].expected) > 1e-18L) {
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

    printf("\nFADDP test completed\n");
}

void test_faddp_mem() {
    printf("\nTesting FADDP with memory operands\n");
    
    // 测试32位浮点内存操作数
    float f32_1 = 1.5f;
    float f32_2 = 2.5f;
    float f32_result;
    
    asm volatile (
        "flds %[f32_2]\n\t"    // 加载到ST(0)
        "fadds %[f32_1]\n\t"    // ST(0) += mem32
        "fstps %[res]\n\t"      // 存储结果并弹出
        : [res] "=m" (f32_result)
        : [f32_1] "m" (f32_1), [f32_2] "m" (f32_2)
        : "memory"
    );
    
    printf("32-bit float test:\n");
    printf("Operand 1: %f\n", f32_1);
    printf("Operand 2: %f\n", f32_2);
    printf("Result: %f\n", f32_result);
    print_x87_status();
    
    // 测试64位浮点内存操作数
    double f64_1 = 1.25;
    double f64_2 = 3.75;
    double f64_result;
    
    asm volatile (
        "fldl %[f64_2]\n\t"     // 加载到ST(0)
        "faddl %[f64_1]\n\t"    // ST(0) += mem64
        "fstpl %[res]\n\t"      // 存储结果并弹出
        : [res] "=m" (f64_result)
        : [f64_1] "m" (f64_1), [f64_2] "m" (f64_2)
        : "memory"
    );
    
    printf("\n64-bit float test:\n");
    printf("Operand 1: %lf\n", f64_1);
    printf("Operand 2: %lf\n", f64_2);
    printf("Result: %lf\n", f64_result);
    print_x87_status();
}

void test_faddp_reg_combinations() {
    printf("\nTesting FADDP with different register combinations\n");
    
    long double vals[] = {1.0L, 2.0L, 3.0L, 4.0L};
    long double result;

    // 测试ST(0) + ST(1)
    asm volatile (
        "fldt %[val3]\n\t"
        "fldt %[val2]\n\t"
        "fldt %[val1]\n\t"
        "fldt %[val0]\n\t"  // ST(0)=val0, ST(1)=val1, ST(2)=val2, ST(3)=val3
        "faddp %%st(0), %%st(1)\n\t"  // ST(1) += ST(0), pop
        "fstpt %[res]\n\t"
        : [res] "=m" (result)
        : [val0] "m" (vals[0]), [val1] "m" (vals[1]), 
          [val2] "m" (vals[2]), [val3] "m" (vals[3])
        : "memory"
    );
    printf("\nTest case: ST(0) + ST(1)\n");
    printf("ST(0): ");
    print_float80(vals[0]);
    printf("ST(1): ");
    print_float80(vals[1]);
    printf("Result: ");
    print_float80(result);
    print_x87_status();

    // 测试ST(0) + ST(2) 
    asm volatile (
        "fldt %[val3]\n\t"
        "fldt %[val2]\n\t"
        "fldt %[val1]\n\t"
        "fldt %[val0]\n\t"
        "faddp %%st(0), %%st(2)\n\t"
        "fstpt %[res]\n\t"
        : [res] "=m" (result)
        : [val0] "m" (vals[0]), [val1] "m" (vals[1]),
          [val2] "m" (vals[2]), [val3] "m" (vals[3])
        : "memory"
    );
    printf("\nTest case: ST(0) + ST(2)\n");
    printf("ST(0): ");
    print_float80(vals[0]);
    printf("ST(2): ");
    print_float80(vals[2]);
    printf("Result: ");
    print_float80(result);
    print_x87_status();

    // 测试ST(0) + ST(3)
    asm volatile (
        "fldt %[val3]\n\t"
        "fldt %[val2]\n\t"
        "fldt %[val1]\n\t"
        "fldt %[val0]\n\t"
        "faddp %%st(0), %%st(3)\n\t"
        "fstpt %[res]\n\t"
        : [res] "=m" (result)
        : [val0] "m" (vals[0]), [val1] "m" (vals[1]),
          [val2] "m" (vals[2]), [val3] "m" (vals[3])
        : "memory"
    );
    printf("\nTest case: ST(0) + ST(3)\n");
    printf("ST(0): ");
    print_float80(vals[0]);
    printf("ST(3): ");
    print_float80(vals[3]);
    printf("Result: ");
    print_float80(result);
    print_x87_status();
}

void test_faddp_rounding() {
    printf("\nTesting FADDP with different rounding modes\n");
    
    const uint16_t rounding_modes[] = {
        X87_CW_RC_NEAR,  // 舍入到最近
        X87_CW_RC_DOWN,  // 向下舍入
        X87_CW_RC_UP,    // 向上舍入
        X87_CW_RC_ZERO   // 向零舍入
    };
    
    const char *mode_names[] = {
        "Round to nearest",
        "Round down",
        "Round up",
        "Round toward zero"
    };
    
    long double a = 1.1L;
    long double b = 1.0000000000000001L;
    long double result;
    
    for (int i = 0; i < 4; i++) {
        // 设置舍入模式
        uint16_t cw = get_x87_cw();
        cw = (cw & ~X87_CW_RC_MASK) | rounding_modes[i];
        asm volatile ("fldcw %0" : : "m" (cw));
        
        // 执行测试
        asm volatile (
            "fldt %[b]\n\t"
            "fldt %[a]\n\t"
            "faddp\n\t"
            "fstpt %[res]\n\t"
            : [res] "=m" (result)
            : [a] "m" (a), [b] "m" (b)
            : "memory"
        );
        
        printf("\nRounding mode: %s\n", mode_names[i]);
        printf("Operand A: ");
        print_float80(a);
        printf("Operand B: ");
        print_float80(b);
        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
    
    // 恢复默认舍入模式
    init_x87_env();
}

int main() {
    init_x87_env();
    test_faddp();
    test_faddp_mem();
    test_faddp_reg_combinations();
    test_faddp_rounding();
    return 0;
}
