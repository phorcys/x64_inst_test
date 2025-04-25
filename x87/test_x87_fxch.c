#include "../x87/x87.h"
#include <stdio.h>
#include <stdint.h>

// 测试FXCH指令
void test_fxch() {
    printf("=== Testing FXCH ===\n");
    
    init_x87_env();
    
    // 测试用例数组: {值1, 值2}
    double test_cases[][2] = {
        {1.0, 2.0},        // 普通值交换
        {3.14, -2.71},     // 正负值交换
        {0.0, 1.0},        // 零值交换
        {-0.0, 1.0},       // 负零交换
        {1e300, 1e-300},   // 极大极小值交换
        {INFINITY, -INFINITY}, // 无穷大交换
        {NAN, 1.0},        // NaN交换
        {1e-308, 1e-309},  // 非规格化数交换
        {0.0/0.0, 1.0}     // SNaN交换
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double val1 = test_cases[i][0];
        double val2 = test_cases[i][1];
        
        printf("\nTest case %zu: %.2f <-> %.2f (ST(0) <-> ST(1))\n", 
               i+1, val1, val2);
        
        // 加载初始值到x87栈并执行交换
        double result1, result2;
        uint16_t sw;
        
        __asm__ __volatile__ (
            "fldl %[v2]\n\t"    // 加载val2到ST(0)
            "fldl %[v1]\n\t"    // 加载val1到ST(0), val2变为ST(1)
            "fxch %%st(1)\n\t"  // 交换ST(0)和ST(1)
            "fstpl %[r1]\n\t"   // 保存ST(0)交换后值
            "fstpl %[r2]\n\t"   // 保存ST(1)交换后值
            "fnstsw %%ax"       // 保存状态字
            : [r1] "=m" (result1), [r2] "=m" (result2), "=a" (sw)
            : [v1] "m" (val1), [v2] "m" (val2)
            : "cc", "st", "st(1)"
        );
        
        printf("Exchange results:\n");
        printf("  ST(0): %.6f (expected %.6f)\n", result1, val2);
        printf("  ST(1): %.6f (expected %.6f)\n", result2, val1);
        print_x87_status();
        
        // 验证交换结果
        int pass = 0;
        if (isnan(val1)) {
            pass = isnan(result2) && (result1 == val2);
        } else if (isnan(val2)) {
            pass = isnan(result1) && (result2 == val1);
        } else {
            pass = (result1 == val2) && (result2 == val1);
        }
        
        if (pass) {
            printf("  [PASS] Values exchanged correctly\n");
        } else {
            printf("  [FAIL] Exchange failed\n");
        }
    }
}

int main() {
    test_fxch();
    return 0;
}
