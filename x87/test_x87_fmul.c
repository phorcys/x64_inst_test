#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

void test_fmul() {
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
    
    printf("Testing FMUL ST,Msr instruction\n");
    
    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        for (size_t j = 0; j < sizeof(test_values)/sizeof(test_values[0]); j++) {
            double a = test_values[i];
            double b = test_values[j];
            double result;
            uint16_t fpu_status;
            
            asm volatile (
                "fldl %[b]\n\t"      // 加载b到ST(0)
                "fldl %[a]\n\t"      // 加载a到ST(0), b移到ST(1)
                "fmulp %%st(1)\n\t"  // ST(0) = ST(0) * ST(1)
                "fstpl %[res]\n\t"   // 存储结果
                "fnstsw %[status]"   // 存储FPU状态字
                : [res] "=m" (result),
                  [status] "=m" (fpu_status)
                : [a] "m" (a),
                  [b] "m" (b)
                : "memory"
            );
            
            printf("\nTest case %zu * %zu:\n", i+1, j+1);
            printf("Operands: %.15g * %.15g\n", a, b);
            printf("Result: %.15g\n", result);
            printf("FPU Status: 0x%04x\n", fpu_status);
            
            // 详细检查FPU状态标志
            if (fpu_status & 0x3f) {
                printf("FPU Exceptions: ");
                if (fpu_status & 0x01) printf("Invalid ");
                if (fpu_status & 0x02) printf("Denormal ");
                if (fpu_status & 0x04) printf("Zero-divide ");
                if (fpu_status & 0x08) printf("Overflow ");
                if (fpu_status & 0x10) printf("Underflow ");
                if (fpu_status & 0x20) printf("Precision ");
                printf("\n");
            }
            
            // 验证结果
            int passed = 1;
            if (isnan(a) || isnan(b)) {
                if (!isnan(result)) {
                    printf("FAIL: Expected NaN but got %.15g\n", result);
                    passed = 0;
                }
            } else if (isinf(a) || isinf(b)) {
                if (a == 0 || b == 0) {
                    if (!isnan(result)) {
                        printf("FAIL: Expected NaN for inf*0 but got %.15g\n", result);
                        passed = 0;
                    }
                } else if (!isinf(result)) {
                    printf("FAIL: Expected Infinity but got %.15g\n", result);
                    passed = 0;
                }
            } else {
                double expected = a * b;
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
    
    printf("FMUL test completed\n");
}

int main() {
    test_fmul();
    return 0;
}
