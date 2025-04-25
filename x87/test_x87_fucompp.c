#include "../x87/x87.h"
#include <stdio.h>
#include <stdint.h>

// 测试FUCOMPP指令
void test_fucompp() {
    printf("=== Testing FUCOMPP ===\n");
    
    init_x87_env();
    
    // 测试用例数组: {操作数1, 操作数2, 预期结果}
    double test_cases[][3] = {
        {1.0, 1.0, -1},        // 相等
        {1.0, 2.0, 1},         // ST(0) < ST(1)
        {2.0, 1.0, -1},        // ST(0) > ST(1)
        {0.0, 0.0, -1},        // 零比较
        {-0.0, 0.0, -1},       // 负零比较
        {-1.0, 1.0, 1},        // 负数比较
        {1.0, -1.0, -1},       // 正负比较
        {1.0, 0.0/0.0, 1},     // 与NaN比较
        {0.0/0.0, 1.0, 1},     // NaN比较
        {INFINITY, 1.0, -1},   // 无穷大比较
        {1.0, INFINITY, 1},    // 与无穷大比较
        {-INFINITY, INFINITY, 1}, // 负无穷比较
        {1e-308, 1e-309, -1},  // 非规格化数比较
        {1e-309, 1e-308, 1}    // 更小的非规格化数比较
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double op1 = test_cases[i][0];
        double op2 = test_cases[i][1];
        int expected = test_cases[i][2];
        
        printf("\nTest case %zu: %.2f vs %.2f\n", i+1, op1, op2);
        
        // 加载操作数到x87栈
        uint16_t sw;
        __asm__ __volatile__ (
            "fldl %[op2]\n\t"       // 加载op2到ST(0)
            "fldl %[op1]\n\t"       // 加载op1到ST(0), op2变为ST(1)
            "fucompp\n\t"           // 比较并弹出两个元素
            "fnstsw %%ax"           // 保存状态字
            : "=a" (sw)
            : [op1] "m" (*(double(*)[1])&op1), [op2] "m" (*(double(*)[1])&op2)
            : "cc", "st", "st(1)"
        );
        
        // 获取比较结果 (根据x87手册)
        int result = -1; // 默认ST(0) > ST(1)
        
        if ((sw & X87_SW_C3) && !(sw & X87_SW_C0)) {
            result = -1;  // ST(0) == ST(1)
        } else if (!(sw & X87_SW_C3) && (sw & X87_SW_C0)) {
            result = 1;   // ST(0) < ST(1)
        } else if (!(sw & X87_SW_C3) && !(sw & X87_SW_C0)) {
            result = -1;  // ST(0) > ST(1)
        } else {
            result = 1;   // 无序比较(NaN情况)
        }
        
        printf("Expected: %d, Actual: %d\n", expected, result);
        print_x87_status();
        
        if (result != expected) {
            printf("  [FAIL] Result mismatch!\n");
        } else {
            printf("  [PASS]\n");
        }
    }
}

int main() {
    test_fucompp();
    return 0;
}
