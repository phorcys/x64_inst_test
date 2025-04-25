#include "../x87/x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// 测试FXCH7alias指令
void test_fxch7alias() {
    printf("=== Testing FXCH7alias ===\n");
    
    init_x87_env();
    
    // 测试用例数组: {值1, 值2, 值3, 值4, 值5, 值6, 值7, 值8}
    double test_cases[][8] = {
        {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0},    // 正常值交换
        {3.14, -2.71, 0.0, INFINITY, NAN, 1e300, -1e300, 1e-300}, // 特殊值和边界值交换
        {POS_ZERO, NEG_ZERO, POS_INF, NEG_INF, POS_NAN, NEG_NAN, POS_DENORM, NEG_DENORM}, // 零和特殊值
        {1e308, -1e308, 1e-308, -1e-308, 0.0, -0.0, 1.0, -1.0}, // 边界值
        {PI, E, 1.0/PI, 1.0/E, -PI, -E, -1.0/PI, -1.0/E} // 数学常数
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        // 加载8个测试值到x87栈
        double val1 = test_cases[i][0];
        double val2 = test_cases[i][1];
        double val3 = test_cases[i][2];
        double val4 = test_cases[i][3];
        double val5 = test_cases[i][4];
        double val6 = test_cases[i][5];
        double val7 = test_cases[i][6];
        double val8 = test_cases[i][7];
        
        double result1, result2, result3, result4, result5, result6, result7, result8;
        uint16_t sw;
        
        __asm__ __volatile__ (
            "fldl %[v8]\n\t"    // ST(0)=v8
            "fldl %[v7]\n\t"    // ST(0)=v7, ST(1)=v8
            "fldl %[v6]\n\t"    // ST(0)=v6, ST(1)=v7, ST(2)=v8
            "fldl %[v5]\n\t"    // ST(0)=v5, ST(1)=v6, ST(2)=v7, ST(3)=v8
            "fldl %[v4]\n\t"    // ST(0)=v4, ST(1)=v5, ST(2)=v6, ST(3)=v7, ST(4)=v8
            "fldl %[v3]\n\t"    // ST(0)=v3, ST(1)=v4, ST(2)=v5, ST(3)=v6, ST(4)=v7, ST(5)=v8
            "fldl %[v2]\n\t"    // ST(0)=v2, ST(1)=v3, ST(2)=v4, ST(3)=v5, ST(4)=v6, ST(5)=v7, ST(6)=v8
            "fldl %[v1]\n\t"    // ST(0)=v1, ST(1)=v2, ST(2)=v3, ST(3)=v4, ST(4)=v5, ST(5)=v6, ST(6)=v7, ST(7)=v8
            "fxch %%st(7)\n\t"  // 交换ST(0)和ST(7)
            "fstpl %[r1]\n\t"   // 保存ST(0)交换后值
            "fstpl %[r2]\n\t"   // 保存ST(1)
            "fstpl %[r3]\n\t"   // 保存ST(2)
            "fstpl %[r4]\n\t"   // 保存ST(3)
            "fstpl %[r5]\n\t"   // 保存ST(4)
            "fstpl %[r6]\n\t"   // 保存ST(5)
            "fstpl %[r7]\n\t"   // 保存ST(6)
            "fstpl %[r8]\n\t"   // 保存ST(7)交换后值
            "fnstsw %%ax"       // 保存状态字
            : [r1] "=m" (result1), [r2] "=m" (result2), [r3] "=m" (result3),
              [r4] "=m" (result4), [r5] "=m" (result5), [r6] "=m" (result6),
              [r7] "=m" (result7), [r8] "=m" (result8), "=a" (sw)
            : [v1] "m" (val1), [v2] "m" (val2), [v3] "m" (val3),
              [v4] "m" (val4), [v5] "m" (val5), [v6] "m" (val6),
              [v7] "m" (val7), [v8] "m" (val8)
            : "cc", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)"
        );
        
        printf("Original: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n",
               val1, val2, val3, val4, val5, val6, val7, val8);
        printf("After FXCH7alias:\n");
        printf("  ST(0): %.6f (expected %.6f)\n", result1, val8);
        printf("  ST(1): %.6f (expected %.6f)\n", result2, val2);
        printf("  ST(2): %.6f (expected %.6f)\n", result3, val3);
        printf("  ST(3): %.6f (expected %.6f)\n", result4, val4);
        printf("  ST(4): %.6f (expected %.6f)\n", result5, val5);
        printf("  ST(5): %.6f (expected %.6f)\n", result6, val6);
        printf("  ST(6): %.6f (expected %.6f)\n", result7, val7);
        printf("  ST(7): %.6f (expected %.6f)\n", result8, val1);
        print_x87_status();
        
        // 验证交换结果 - 只检查ST(0)和ST(7)是否正确交换
        int pass = 1;
        
        // 检查ST(0)和val8 - 使用二进制位模式比较
        uint64_t r1, v8;
        memcpy(&r1, &result1, sizeof(r1));
        memcpy(&v8, &val8, sizeof(v8));
        pass = pass && (r1 == v8);
        
        // 检查ST(7)和val1 - 使用二进制位模式比较
        uint64_t r8, v1;
        memcpy(&r8, &result8, sizeof(r8));
        memcpy(&v1, &val1, sizeof(v1));
        pass = pass && (r8 == v1);
        
        if (pass) {
            printf("  [PASS] FXCH7alias correct\n");
        } else {
            printf("  [FAIL] FXCH7alias failed\n");
            // 输出详细比较信息
            printf("  Detailed comparison:\n");
            
            // 比较ST(0)和val8
            printf("  ST(0) vs val8:\n");
            uint64_t r1, v8;
            memcpy(&r1, &result1, sizeof(r1));
            memcpy(&v8, &val8, sizeof(v8));
            printf("    result1 bits: 0x%016lx\n", r1);
            printf("    val8 bits:     0x%016lx\n", v8);
            printf("    values: %.6f vs %.6f\n", result1, val8);
            printf("    signs: %d vs %d\n", signbit(result1), signbit(val8));
            
            // 比较ST(7)和val1
            printf("  ST(7) vs val1:\n");
            uint64_t r8, v1;
            memcpy(&r8, &result8, sizeof(r8));
            memcpy(&v1, &val1, sizeof(v1));
            printf("    result8 bits: 0x%016lx\n", r8);
            printf("    val1 bits:     0x%016lx\n", v1);
            printf("    values: %.6f vs %.6f\n", result8, val1);
            printf("    signs: %d vs %d\n", signbit(result8), signbit(val1));
        }
    }
}

int main() {
    test_fxch7alias();
    return 0;
}
