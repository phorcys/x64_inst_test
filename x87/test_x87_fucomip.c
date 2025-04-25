#include "../x87/x87.h"
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

// 测试FUCOMIP指令
void test_fucomip() {
    printf("=== Testing FUCOMIP ===\n");
    
    init_x87_env();
    
    // 测试用例数组: {操作数1, 操作数2, 预期结果, 预期EFLAGS}
    struct {
        double op1;
        double op2;
        int expected_result;
        uint32_t expected_eflags;
    } test_cases[] = {
        {1.0, 1.0, 0, X86_EFLAGS_ZF | X86_EFLAGS_PF},        // 相等
        {1.0, 2.0, -1, X86_EFLAGS_NONE},                     // ST(0) < ST(i)
        {2.0, 1.0, 1, X86_EFLAGS_NONE},                      // ST(0) > ST(i)
        {0.0, 0.0, 0, X86_EFLAGS_ZF | X86_EFLAGS_PF},        // 零比较
        {-0.0, 0.0, 0, X86_EFLAGS_ZF | X86_EFLAGS_PF},       // 负零比较
        {-1.0, 1.0, -1, X86_EFLAGS_NONE},                    // 负数比较
        {1.0, -1.0, 1, X86_EFLAGS_NONE},                     // 正负比较
        {1.0, 0.0/0.0, 1, X86_EFLAGS_CF | X86_EFLAGS_PF},    // 与NaN比较
        {0.0/0.0, 1.0, 1, X86_EFLAGS_CF | X86_EFLAGS_PF},     // NaN比较
        {INFINITY, 1.0, 1, X86_EFLAGS_NONE},                 // 无穷大比较
        {1.0, INFINITY, -1, X86_EFLAGS_NONE},                // 与无穷大比较
        {-INFINITY, INFINITY, -1, X86_EFLAGS_NONE},          // 负无穷比较
        {1e-308, 1e-309, 1, X86_EFLAGS_NONE},                // 非规格化数比较
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        double op1 = test_cases[i].op1;
        double op2 = test_cases[i].op2;
        int expected = test_cases[i].expected_result;
        uint32_t expected_eflags = test_cases[i].expected_eflags;
        
        printf("\nTest case %zu: %.6f vs %.6f\n", i+1, op1, op2);
        
        // 加载操作数到x87栈
        uint16_t sw;
        uint16_t control_word = 0x037F;
        // 设置控制字(屏蔽所有异常)
        __asm__ __volatile__ ("fninit");
        __asm__ __volatile__ ("fldcw %0" : : "m" (control_word));
        
        // 使用GCC扩展语法明确指定操作数类型
        __asm__ __volatile__ (
            "fldl %[op2]\n\t"       // 加载op2到ST(0)
            "fldl %[op1]\n\t"       // 加载op1到ST(0), op2变为ST(1)
            "fucomip %%st(1)\n\t"   // 比较ST(0)和ST(1),弹出ST(0)
            "fstsw %%ax\n\t"        // 立即保存状态字(不使用fnstsw)
            : "=a" (sw)
            : [op1] "m" (op1), [op2] "m" (op2)
            : "cc", "st", "st(1)"
        );
        
        // 获取比较结果和EFLAGS
        int result;
        uint32_t eflags;
        uint8_t c0 = (sw & X87_SW_C0) ? 1 : 0;
        uint8_t c3 = (sw & X87_SW_C3) ? 1 : 0;
        
        // 根据x87状态字设置EFLAGS标志位
        eflags = 0;
        if (sw & X87_SW_C0) eflags |= X86_EFLAGS_CF;
        if (sw & X87_SW_C2) eflags |= X86_EFLAGS_PF;
        if (sw & X87_SW_C3) eflags |= X86_EFLAGS_ZF;
        
        // 对于相等比较和无序比较，需要设置PF
        if ((sw & (X87_SW_C0|X87_SW_C2|X87_SW_C3)) != 0) {
            eflags |= X86_EFLAGS_PF;
        }
        
        // 清除未使用的变量警告
        (void)c0;
        (void)c3;
        
        // 根据FUCOMIP指令行为设置结果
        if (sw & X87_SW_C3) {
            if (sw & X87_SW_C0) {
                result = -1;  // 无序(NaN情况)
            } else {
                result = 0;   // ST(0) == ST(i)
            }
        } else {
            if (sw & X87_SW_C0) {
                result = -1;  // ST(0) < ST(i)
            } else {
                result = 1;   // ST(0) > ST(i)
            }
        }
        
        printf("Expected result: %d, Actual: %d\n", expected, result);
        printf("Expected EFLAGS: 0x%08x, Actual: 0x%08x\n", 
               expected_eflags, eflags & (X86_EFLAGS_CF|X86_EFLAGS_PF|X86_EFLAGS_ZF));
        print_x87_status();
        
        int pass = (result == expected) && 
                  ((eflags & (X86_EFLAGS_CF|X86_EFLAGS_PF|X86_EFLAGS_ZF)) == expected_eflags);
        
        if (pass) {
            printf("  [PASS] FUCOMIP correct\n");
        } else {
            printf("  [FAIL] FUCOMIP failed\n");
            if (result != expected) {
                printf("    Result mismatch: expected %d, got %d\n", expected, result);
            }
            if ((eflags & (X86_EFLAGS_CF|X86_EFLAGS_PF|X86_EFLAGS_ZF)) != expected_eflags) {
                printf("    EFLAGS mismatch: expected 0x%08x, got 0x%08x\n",
                      expected_eflags, eflags & (X86_EFLAGS_CF|X86_EFLAGS_PF|X86_EFLAGS_ZF));
            }
        }
    }
}

int main() {
    test_fucomip();
    return 0;
}
