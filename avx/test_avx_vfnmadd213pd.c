#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

#define TEST_CASE_COUNT 12

typedef struct {
    double a[4];
    double b[4];
    double c[4];
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {{1.5, 2.5, 3.5, 4.5}, {0.5, 1.5, 2.5, 3.5}, {1.0, 2.0, 3.0, 4.0}, "Normal values"},
    // 零值
    {{0.0, 0.0, 0.0, 0.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 0.0, 0.0}, "Zero values"},
    // 无穷大
    {{INFINITY, -INFINITY, INFINITY, -INFINITY}, {1.0, 1.0, -1.0, -1.0}, {1.0, 1.0, 1.0, 1.0}, "Infinity values"},
    // NaN
    {{NAN, NAN, NAN, NAN}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, "NaN values"},
    // 边界值
    {{DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, {2.0, 2.0, 2.0, 2.0}, {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, "Max boundary values"},
    {{DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, {0.5, 0.5, 0.5, 0.5}, {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, "Min boundary values"},
    // 特殊组合
    {{INFINITY, INFINITY, INFINITY, INFINITY}, {0.0, 0.0, 0.0, 0.0}, {NAN, NAN, NAN, NAN}, "Inf * 0 + NaN"},
    {{NAN, NAN, NAN, NAN}, {INFINITY, INFINITY, INFINITY, INFINITY}, {0.0, 0.0, 0.0, 0.0}, "NaN * Inf + 0"},
    {{0.0, 0.0, 0.0, 0.0}, {NAN, NAN, NAN, NAN}, {INFINITY, INFINITY, INFINITY, INFINITY}, "0 * NaN + Inf"},
    // 极小值
    {{1e-300, 1e-300, 1e-300, 1e-300}, {1e-300, 1e-300, 1e-300, 1e-300}, {1e-300, 1e-300, 1e-300, 1e-300}, "Very small values"},
    // 符号变化
    {{-1.0, -1.0, -1.0, -1.0}, {-1.0, -1.0, -1.0, -1.0}, {-1.0, -1.0, -1.0, -1.0}, "Negative values"},
    {{1.0, 1.0, 1.0, 1.0}, {-1.0, -1.0, -1.0, -1.0}, {1.0, 1.0, 1.0, 1.0}, "Mixed sign values"}
};

static void test_reg_reg_operand() {
    printf("=================================\n");
    printf("VFNMADD213PD Register-Register Tests\n");
    printf("=================================\n\n");
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        double a[4] ALIGNED(32) = {cases[t].a[0], cases[t].a[1], cases[t].a[2], cases[t].a[3]};
        double b[4] ALIGNED(32) = {cases[t].b[0], cases[t].b[1], cases[t].b[2], cases[t].b[3]};
        double c[4] ALIGNED(32) = {cases[t].c[0], cases[t].c[1], cases[t].c[2], cases[t].c[3]};
        double res[4] ALIGNED(32) = {0};
        
        double expected[4] = {
            -(a[0]*b[0]) + c[0],
            -(a[1]*b[1]) + c[1],
            -(a[2]*b[2]) + c[2],
            -(a[3]*b[3]) + c[3]
        };
        
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vmovapd %3, %%ymm2\n\t"
            "vfnmadd213pd %%ymm2, %%ymm0, %%ymm1\n\t"
            "vmovapd %%ymm1, %0\n\t"
            : "=m"(res[0])
            : "m"(a[0]), "m"(b[0]), "m"(c[0])
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Test Case: %s\n", cases[t].desc);
        print_double_vec("A", a, 4);
        print_double_vec("B", b, 4);
        print_double_vec("C", c, 4);
        print_double_vec("Result", res, 4);
        print_double_vec("Expected", expected, 4);
        
        for(int i=0; i<4; i++) {
            if(!double_equal(res[i], expected[i], 1e-10)) {
                printf("Mismatch at index %d: got %.15e, expected %.15e\n", i, res[i], expected[i]);
            }
        }
        printf("\n");
    }
    
    printf("VFNMADD213PD Register-Register Tests Completed\n\n");
}

int main() {
    test_reg_reg_operand();
    return 0;
}
