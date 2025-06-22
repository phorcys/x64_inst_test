#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 14

typedef struct {
    double a;
    double b;
    double c;
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {1.0, 5.0, 9.0, "Normal values"},
    // 零值
    {0.0, 5.0, 9.0, "Zero values"},
    // 无穷大
    {INFINITY, 1.0, 1.0, "Infinity values"},
    // NaN
    {NAN, 1.0, 2.0, "NaN values"},
    // 边界值
    {DBL_MIN, 2.0, DBL_MIN, "Boundary values"},
    // 混合值
    {1.0, INFINITY, NAN, "Mixed special values"},
    // 小值
    {1e-300, 2.0, 1e-300, "Very small values"},
    // a为特殊值
    {INFINITY, 2.0, 1.0, "Special values in a"},
    // b为特殊值
    {1.0, INFINITY, 5.0, "Special values in b"},
    // c为特殊值
    {1.0, 5.0, INFINITY, "Special values in c"},
    // a和b为特殊值
    {INFINITY, NAN, 1.0, "Special values in a and b"},
    // a和c为特殊值
    {INFINITY, 1.0, NAN, "Special values in a and c"},
    // b和c为特殊值
    {1.0, INFINITY, NAN, "Special values in b and c"},
    // 所有特殊值
    {INFINITY, NAN, INFINITY, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_sd(&cases[t].a);
        __m128d vb = _mm_load_sd(&cases[t].b);
        __m128d vc = _mm_load_sd(&cases[t].c);
        
        __asm__ __volatile__(
            "vfnmsub132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : %.17g\n", cases[t].a);
        printf("B     : %.17g\n", cases[t].b);
        printf("C     : %.17g\n", cases[t].c);
        printf("Result: %.17g\n\n", res);
    }
    
    printf("VFNMSUB132SD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_sd(&cases[t].a);
        __m128d vc = _mm_load_sd(&cases[t].c);
        
        // 测试内存操作数
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfnmsub132sd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (cases[t].b), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va1);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        printf("A     : %.17g\n", cases[t].a);
        printf("B     : %.17g\n", cases[t].b);
        printf("C     : %.17g\n", cases[t].c);
        printf("Result: %.17g\n\n", res);
    }
    
    printf("VFNMSUB132SD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFNMSUB132SD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFNMSUB132SD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
