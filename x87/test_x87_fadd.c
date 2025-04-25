#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fadd() {
    double test_values[] = {
        1.0, 2.0, -1.0, 0.0,    // 基本数值
        INFINITY, -INFINITY,      // 无穷大
        NAN, -NAN,                // NaN
        DBL_MAX, -DBL_MAX,        // 最大正/负值
        DBL_MIN, -DBL_MIN,        // 最小正规数
        DBL_TRUE_MIN, -DBL_TRUE_MIN, // 最小非正规数
        1e300, -1e300,            // 大数值
        1e-300, -1e-300,          // 小数值
        0.1, 0.2, -0.1, -0.2      // 小数
    };
    
    printf("Testing FADD ST,Msr instruction\n");
    
    // 测试1: 基本测试
    printf("\n=== Test 1: Basic FADD tests ===\n");
    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        for (size_t j = 0; j < sizeof(test_values)/sizeof(test_values[0]); j++) {
            double a = test_values[i];
            double b = test_values[j];
            double result;
            
            // 测试FADD ST,Msr形式
            asm volatile (
                "fldl %[a]\n\t"      // 加载a到ST(0)
                "faddl %[b]\n\t"     // ST(0) = ST(0) + [b]
                "fstpl %[res]\n\t"   // 存储结果
                : [res] "=m" (result)
                : [a] "m" (a),
                  [b] "m" (b)
                : "memory"
            );
            
            printf("\nTest case %zu + %zu:\n", i+1, j+1);
            printf("Operands: %.15g + %.15g\n", a, b);
            printf("Result: %.15g\n", result);
            print_x87_status();
            
            // 验证结果
            int passed = 1;
            if (isnan(a) || isnan(b)) {
                if (!isnan(result)) {
                    printf("FAIL: Expected NaN but got %.15g\n", result);
                    passed = 0;
                }
            } else if (isinf(a) && isinf(b) && ((a > 0 && b < 0) || (a < 0 && b > 0))) {
                // Special case: inf + -inf should be NaN
                if (!isnan(result)) {
                    printf("FAIL: Expected NaN but got %.15g\n", result);
                    passed = 0;
                }
            } else if (isinf(a) || isinf(b)) {
                if (!isinf(result)) {
                    printf("FAIL: Expected Infinity but got %.15g\n", result);
                    passed = 0;
                }
            } else {
                double expected = a + b;
                if (result != expected && !(isnan(result) && isnan(expected))) {
                    printf("FAIL: Expected %.15g but got %.15g\n", expected, result);
                    passed = 0;
                }
            }
            
            if (passed) {
                printf("PASS\n");
            }
        }
    }
    
    // 测试2: 不同舍入模式
    printf("\n=== Test 2: Rounding modes ===\n");
    const int rounding_modes[] = {FE_TONEAREST, FE_UPWARD, FE_DOWNWARD, FE_TOWARDZERO};
    const char* mode_names[] = {"Nearest", "Up", "Down", "Toward Zero"};
    
    for (int m = 0; m < 4; m++) {
        fesetround(rounding_modes[m]);
        printf("\nRounding mode: %s\n", mode_names[m]);
        
        double a = 1.5;
        double b = 2.5;
        double result;
        
        asm volatile (
            "fldl %[a]\n\t"
            "faddl %[b]\n\t"
            "fstpl %[res]"
            : [res] "=m" (result)
            : [a] "m" (a),
              [b] "m" (b)
            : "memory"
        );
        
        printf("Operands: %.15g + %.15g\n", a, b);
        printf("Result: %.15g\n", result);
        print_x87_status();
    }
    
    // 测试3: 精度控制
    printf("\n=== Test 3: Precision control ===\n");
    unsigned short cw;
    asm volatile ("fnstcw %0" : "=m" (cw));
    
    // 测试不同精度设置
    unsigned short precisions[] = {0x0000, 0x0200, 0x0300}; // 单/双/扩展精度
    const char* prec_names[] = {"Single", "Double", "Extended"};
    
    for (int p = 0; p < 3; p++) {
        unsigned short new_cw = (cw & ~0x0300) | precisions[p];
        asm volatile ("fldcw %0" : : "m" (new_cw));
        
        printf("\nPrecision: %s\n", prec_names[p]);
        double a = 1.0 / 3.0;
        double b = 1.0 / 7.0;
        double result;
        
        asm volatile (
            "fldl %[a]\n\t"
            "faddl %[b]\n\t"
            "fstpl %[res]"
            : [res] "=m" (result)
            : [a] "m" (a),
              [b] "m" (b)
            : "memory"
        );
        
        printf("Operands: %.15g + %.15g\n", a, b);
        printf("Result: %.15g\n", result);
        print_x87_status();
    }
    
    // 恢复默认控制字
    asm volatile ("fldcw %0" : : "m" (cw));
    
    printf("\nFADD test completed\n");
}

int main() {
    test_fadd();
    return 0;
}
