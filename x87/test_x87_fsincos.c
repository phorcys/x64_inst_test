#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fsincos() {
    printf("Testing FSINCOS instruction\n");
    printf("FSINCOS computes both sine and cosine of ST(0)\n");

    long double test_values[] = {
        0.0L,
        M_PI_4,
        M_PI_2,
        M_PI
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        long double angle = test_values[i];
        long double sin_result, cos_result;
        long double sin_expected = sinl(angle);
        long double cos_expected = cosl(angle);

        // 加载操作数并执行FSINCOS
        asm volatile ("fldt %2\n\t"   // 加载角度到ST(0)
                     "fsincos\n\t"   // 计算正弦和余弦
                     "fstpt %0\n\t"  // 存储余弦结果
                     "fstpt %1\n\t"  // 存储正弦结果
                     : "=m" (cos_result), "=m" (sin_result)
                     : "m" (angle)
                     : "memory", "st", "st(1)");

        printf("\nTest case %zu:\n", i+1);
        printf("Angle: ");
        print_float80(angle);
        
        printf("Expected SIN: ");
        print_float80(sin_expected);
        printf("Result SIN: ");
        print_float80(sin_result);
        
        printf("Expected COS: ");
        print_float80(cos_expected);
        printf("Result COS: ");
        print_float80(cos_result);
        
        print_x87_status();

        // 验证结果
        int passed = 1;
        if (fabsl(sin_result - sin_expected) > 1e-10L || 
            fabsl(cos_result - cos_expected) > 1e-10L) {
            printf("FAIL: Expected (SIN: ");
            print_float80(sin_expected);
            printf(", COS: ");
            print_float80(cos_expected);
            printf(") Got (SIN: ");
            print_float80(sin_result);
            printf(", COS: ");
            print_float80(cos_result);
            printf(")\n");
            passed = 0;
        }

        if (passed) {
            printf("PASS\n");
        }
    }

    printf("\nFSINCOS test completed\n");
}

int main() {
    test_fsincos();
    return 0;
}
