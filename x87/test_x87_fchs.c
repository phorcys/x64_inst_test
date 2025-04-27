#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include "x87.h"

void test_fchs() {
    printf("Testing FCHS instruction\n");
    printf("FCHS changes the sign of ST(0)\n");

    struct {
        long double input;
        long double expected;
    } test_cases[] = {
        {1.0L, -1.0L},          // 正数变负数
        {-1.0L, 1.0L},          // 负数变正数
        {POS_ZERO, NEG_ZERO},   // 正零变负零
        {NEG_ZERO, POS_ZERO},   // 负零变正零
        {POS_INF, NEG_INF},     // 正无穷变负无穷
        {NEG_INF, POS_INF},     // 负无穷变正无穷
        {POS_NAN, NEG_NAN},     // NaN符号取反
        {POS_DENORM, NEG_DENORM}, // 正非正规数变负
        {NEG_DENORM, POS_DENORM}  // 负非正规数变正
    };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        long double input = test_cases[i].input;
        long double result;

        // 测试FCHS指令
        asm volatile (
            "fwait\n\t"          // 确保同步
            "fldt %[input]\n\t"  // 加载到ST(0)
            "fchs\n\t"           // 改变符号
            "fstpt %[res]\n\t"   // 存储结果
            "fwait\n\t"          // 确保完成
            : [res] "=m" (result)
            : [input] "m" (input)
        );

        printf("\nTest case %zu:\n", i+1);
        printf("Input: ");
        print_float80(input);
        printf("Result: ");
        print_float80(result);
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (isnan(test_cases[i].expected)) {
            if (!isnan(result) || signbit(result) == signbit(input)) {
                printf("FAIL: Expected ");
                print_float80(test_cases[i].expected);
                printf("       Got ");
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

    printf("\nFCHS test completed\n");
}

int main() {
    init_x87_env();
    test_fchs();
    return 0;
}
