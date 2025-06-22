#include "avx.h"
#include <stdio.h>
#include <float.h>
#include <math.h>

#define TEST_CASE_COUNT 12

typedef struct {
    double a;
    double b;
    double c;
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {2.0, 3.0, 1.5, "Normal values"},
    // 零值
    {0.0, 1.0, 0.0, "Zero values"},
    // 无穷大
    {INFINITY, -1.0, 1.0, "Infinity values"},
    // NaN
    {NAN, 1.0, 1.0, "NaN values"},
    // 边界值
    {DBL_MAX, 2.0, DBL_MIN, "Max boundary values"},
    {DBL_MIN, 0.5, DBL_MAX, "Min boundary values"},
    // 特殊组合
    {INFINITY, 0.0, NAN, "Inf * 0 + NaN"},
    {NAN, INFINITY, 0.0, "NaN * Inf + 0"},
    {0.0, NAN, INFINITY, "0 * NaN + Inf"},
    // 极小值
    {1e-300, 1e-300, 1e-300, "Very small values"},
    // 符号变化
    {-1.0, -1.0, -1.0, "Negative values"},
    {1.0, -1.0, 1.0, "Mixed sign values"}
};

static void test_reg_reg_operand() {
    printf("=================================\n");
    printf("VFNMADD132SD Register-Register Tests\n");
    printf("=================================\n\n");
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        double a = cases[t].a;
        double b = cases[t].b;
        double c = cases[t].c;
        
        double result;
        __asm__ __volatile__(
            "vmovsd %[a], %%xmm0\n\t"
            "vmovsd %[b], %%xmm1\n\t"
            "vmovsd %[c], %%xmm2\n\t"
            "vfnmadd132sd %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovsd %%xmm0, %[result]\n\t"
            : [result] "=m" (result)
            : [a] "m" (a), [b] "m" (b), [c] "m" (c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : %.15e\n", a);
        printf("B     : %.15e\n", b);
        printf("C     : %.15e\n", c);
        printf("Result: %.15e\n", result);
        printf("Expect: %.15e\n", -(a * b) + c);
        printf("\n");
    }
    
    printf("VFNMADD132SD Register-Register Tests Completed\n\n");
}

int main() {
    test_reg_reg_operand();
    return 0;
}
