#include "../x87/x87.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// 测试FXCH4alias指令
void test_fxch4alias() {
    printf("=== Testing FXCH4alias ===\n");
    
    init_x87_env();
    
    // 测试用例数组: {值1, 值2, 值3, 值4, 值5}
    double test_cases[][5] = {
        {1.0, 2.0, 3.0, 4.0, 5.0},    // 正常值交换
        {3.14, -2.71, 0.0, INFINITY, NAN}, // 特殊值交换
        {1e300, 1e-300, 0.0, -1e300, 1e-300}, // 边界值交换
        {0.0, -0.0, 1e-308, 1e-309, INFINITY}, // 零和非规格化数
        {NAN, INFINITY, -INFINITY, 0.0/0.0, 1.0} // 各种NaN情况
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        // 加载5个测试值到x87栈
        double val1 = test_cases[i][0];
        double val2 = test_cases[i][1]; 
        double val3 = test_cases[i][2];
        double val4 = test_cases[i][3];
        double val5 = test_cases[i][4];
        
        double result1, result2, result3, result4, result5;
        uint16_t sw;
        
        __asm__ __volatile__ (
            "fninit\n\t"
            "fldl %[v5]\n\t"    // ST(0)=v5
            "fldl %[v4]\n\t"    // ST(0)=v4, ST(1)=v5
            "fldl %[v3]\n\t"    // ST(0)=v3, ST(1)=v4, ST(2)=v5
            "fldl %[v2]\n\t"    // ST(0)=v2, ST(1)=v3, ST(2)=v4, ST(3)=v5
            "fldl %[v1]\n\t"    // ST(0)=v1, ST(1)=v2, ST(2)=v3, ST(3)=v4, ST(4)=v5
            "fxch %%st(4)\n\t"  // 交换ST(0)和ST(4)
            "fstpl %[r1]\n\t"   // 保存ST(0)交换后值
            "fstpl %[r2]\n\t"   // 保存ST(1)
            "fstpl %[r3]\n\t"   // 保存ST(2)
            "fstpl %[r4]\n\t"   // 保存ST(3)
            "fstpl %[r5]\n\t"   // 保存ST(4)交换后值
            "fnstsw %%ax"       // 保存状态字
            : [r1] "=m" (result1), [r2] "=m" (result2), 
              [r3] "=m" (result3), [r4] "=m" (result4),
              [r5] "=m" (result5), "=a" (sw)
            : [v1] "m" (val1), [v2] "m" (val2),
              [v3] "m" (val3), [v4] "m" (val4),
              [v5] "m" (val5)
            : "cc", "st", "st(1)", "st(2)", "st(3)", "st(4)"
        );
        
        printf("Original: %.6f, %.6f, %.6f, %.6f, %.6f\n",
               val1, val2, val3, val4, val5);
        printf("After FXCH4alias:\n");
        printf("  ST(0): %.6f (expected %.6f)\n", result1, val5);
        printf("  ST(1): %.6f (expected %.6f)\n", result2, val2);
        printf("  ST(2): %.6f (expected %.6f)\n", result3, val3);
        printf("  ST(3): %.6f (expected %.6f)\n", result4, val4);
        printf("  ST(4): %.6f (expected %.6f)\n", result5, val1);
        print_x87_status();
        
        // 验证交换结果(考虑NaN和无穷大情况)
        int pass = 1;
        
        // 检查ST(0)和val5
        if (isnan(result1)) {
            // 比较NaN的二进制表示
            uint64_t r1, v5;
            memcpy(&r1, &result1, sizeof(r1));
            memcpy(&v5, &val5, sizeof(v5));
            pass = pass && isnan(val5) && (r1 == v5);
        } else if (isinf(result1) && isinf(val5)) {
            pass = pass && (signbit(result1) == signbit(val5));
        } else {
            pass = pass && (result1 == val5);
        }
        
        // 检查ST(1)和val2
        if (isinf(result2) && isinf(val2)) {
            pass = pass && (signbit(result2) == signbit(val2));
        } else {
            pass = pass && (result2 == val2);
        }
        
        // 检查ST(2)和val3
        if (isinf(result3) && isinf(val3)) {
            pass = pass && (signbit(result3) == signbit(val3));
        } else {
            pass = pass && (result3 == val3);
        }
        
        // 检查ST(3)和val4
        if (isnan(result4)) {
            // 比较NaN的二进制表示
            uint64_t r4, v4;
            memcpy(&r4, &result4, sizeof(r4));
            memcpy(&v4, &val4, sizeof(v4));
            pass = pass && isnan(val4) && (r4 == v4);
        } else if (isinf(result4) && isinf(val4)) {
            pass = pass && (signbit(result4) == signbit(val4));
        } else {
            pass = pass && (result4 == val4);
        }
        
        // 检查ST(4)和val1
        if (isnan(result5)) {
            // 比较NaN的二进制表示
            uint64_t r5, v1;
            memcpy(&r5, &result5, sizeof(r5));
            memcpy(&v1, &val1, sizeof(v1));
            pass = pass && isnan(val1) && (r5 == v1);
        } else if (isinf(result5) && isinf(val1)) {
            pass = pass && (signbit(result5) == signbit(val1));
        } else {
            pass = pass && (result5 == val1);
        }
        
        if (pass) {
            printf("  [PASS] FXCH4alias correct\n");
        } else {
            printf("  [FAIL] FXCH4alias failed\n");
            // 输出所有值的详细比较信息
            printf("  Detailed comparison:\n");
            
            // 比较ST(0)和val5
            printf("  ST(0) vs val5:\n");
            if (isnan(result1) || isnan(val5)) {
                uint64_t r1, v5;
                memcpy(&r1, &result1, sizeof(r1));
                memcpy(&v5, &val5, sizeof(v5));
                printf("    result1 bits: 0x%016lx\n", r1);
                printf("    val5 bits:     0x%016lx\n", v5);
            } else {
                printf("    values: %.6f vs %.6f\n", result1, val5);
            }
            
            // 比较ST(1)和val2
            printf("  ST(1) vs val2:\n");
            if (isinf(result2) && isinf(val2)) {
                printf("    signs: %d vs %d\n", signbit(result2), signbit(val2));
            } else {
                printf("    values: %.6f vs %.6f\n", result2, val2);
            }
            
            // 比较ST(2)和val3
            printf("  ST(2) vs val3:\n");
            if (isinf(result3) && isinf(val3)) {
                printf("    signs: %d vs %d\n", signbit(result3), signbit(val3));
            } else {
                printf("    values: %.6f vs %.6f\n", result3, val3);
            }
            
            // 比较ST(3)和val4
            printf("  ST(3) vs val4:\n");
            if (isnan(result4) || isnan(val4)) {
                uint64_t r4, v4;
                memcpy(&r4, &result4, sizeof(r4));
                memcpy(&v4, &val4, sizeof(v4));
                printf("    result4 bits: 0x%016lx\n", r4);
                printf("    val4 bits:     0x%016lx\n", v4);
            } else if (isinf(result4) && isinf(val4)) {
                printf("    signs: %d vs %d\n", signbit(result4), signbit(val4));
            } else {
                printf("    values: %.6f vs %.6f\n", result4, val4);
            }
            
            // 比较ST(4)和val1
            printf("  ST(4) vs val1:\n");
            if (isinf(result5) && isinf(val1)) {
                printf("    signs: %d vs %d\n", signbit(result5), signbit(val1));
            } else {
                printf("    values: %.6f vs %.6f\n", result5, val1);
            }
        }
    }
}

int main() {
    test_fxch4alias();
    return 0;
}
