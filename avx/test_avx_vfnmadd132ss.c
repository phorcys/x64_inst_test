#include "avx.h"
#include <stdio.h>
#include <float.h>
#include <math.h>

#define TEST_CASE_COUNT 12

typedef struct {
    float a;
    float b;
    float c;
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {2.0f, 3.0f, 1.5f, "Normal values"},
    // 零值
    {0.0f, 1.0f, 0.0f, "Zero values"},
    // 无穷大
    {INFINITY, -1.0f, 1.0f, "Infinity values"},
    // NaN
    {NAN, 1.0f, 1.0f, "NaN values"},
    // 边界值
    {FLT_MAX, 2.0f, FLT_MIN, "Max boundary values"},
    {FLT_MIN, 0.5f, FLT_MAX, "Min boundary values"},
    // 特殊组合
    {INFINITY, 0.0f, NAN, "Inf * 0 + NaN"},
    {NAN, INFINITY, 0.0f, "NaN * Inf + 0"},
    {0.0f, NAN, INFINITY, "0 * NaN + Inf"},
    // 极小值
    {1e-30f, 1e-30f, 1e-30f, "Very small values"},
    // 符号变化
    {-1.0f, -1.0f, -1.0f, "Negative values"},
    {1.0f, -1.0f, 1.0f, "Mixed sign values"}
};

static void test_reg_reg_operand() {
    printf("=================================\n");
    printf("VFNMADD132SS Register-Register Tests\n");
    printf("=================================\n\n");
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        float a = cases[t].a;
        float b = cases[t].b;
        float c = cases[t].c;
        
        float result;
        __asm__ __volatile__(
            "vmovss %[a], %%xmm0\n\t"
            "vmovss %[b], %%xmm1\n\t"
            "vmovss %[c], %%xmm2\n\t"
            "vfnmadd132ss %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovss %%xmm0, %[result]\n\t"
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
    
    printf("VFNMADD132SS Register-Register Tests Completed\n\n");
}

int main() {
    test_reg_reg_operand();
    return 0;
}
