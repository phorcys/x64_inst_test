#include "../x87/x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// 测试FXAM指令
void test_fxam() {
    printf("=== Testing FXAM ===\n");
    
    init_x87_env();
    
    // 测试用例数组
    struct {
        const char *desc;
        double value;
        uint16_t expected_flags;
    } test_cases[] = {
        {"正数", 1.0, 0},  // C0=0, C1=0, C2=0, C3=0
        {"负数", -1.0, X87_SW_C1}, // C1=1
        {"正零", 0.0, X87_SW_C3}, // C3=1
        {"负零", -0.0, X87_SW_C1|X87_SW_C3}, // C1=1, C3=1
        {"正无穷", INFINITY, X87_SW_C0}, // C0=1
        {"负无穷", -INFINITY, X87_SW_C0|X87_SW_C1}, // C0=1, C1=1
        {"NaN", NAN, X87_SW_C0|X87_SW_C2}, // C0=1, C2=1
        {"非规格化数", 1e-308, 0}, // C0=0
        {"更小的非规格化数", 1e-320, 0}, // C0=0
        {"正规格化数", 1e-300, 0}, // C0=0
        {"负规格化数", -1e-300, X87_SW_C1}, // C1=1
        {"SNaN", 0.0/0.0, X87_SW_C0|X87_SW_C1|X87_SW_C2}, // C0=1, C1=1, C2=1
        {"QNaN", NAN, X87_SW_C0|X87_SW_C2} // C0=1, C2=1
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        printf("\nTest case %zu: %s (%.2f)\n", i+1, 
               test_cases[i].desc, test_cases[i].value);
        
        // 加载测试值到x87栈
        uint16_t sw;
        __asm__ __volatile__ (
            "fldl %[val]\n\t"   // 加载测试值
            "fxam\n\t"          // 检查值类型
            "fnstsw %%ax"       // 保存状态字
            : "=a" (sw)
            : [val] "m" (test_cases[i].value)
            : "cc", "st"
        );
        
        // 检查关键标志位(C0,C1,C3)，忽略C2
        uint16_t mask = X87_SW_C0|X87_SW_C1|X87_SW_C3;
        uint16_t result = sw & mask;
        uint16_t expected = test_cases[i].expected_flags & mask;
        
        printf("Expected flags: 0x%04X, Actual flags: 0x%04X\n", 
               expected, result);
        print_x87_status();
        
        if (result != expected) {
            printf("  [FAIL] Flags mismatch!\n");
        } else {
            printf("  [PASS]\n");
        }
        
        // 弹出栈顶值
        __asm__ __volatile__ ("fstp %%st(0)" ::: "st");
    }
}

int main() {
    test_fxam();
    return 0;
}
