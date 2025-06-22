#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 14

typedef struct {
    float a;
    float b;
    float c;
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {1.0f, 2.0f, 3.0f, "Normal values"},
    // 零值
    {0.0f, -0.0f, 0.0f, "Zero values"},
    // 无穷大
    {INFINITY, -INFINITY, 1.0f, "Infinity values"},
    // NaN
    {NAN, 1.0f, 2.0f, "NaN values"},
    // 边界值
    {FLT_MIN, FLT_MAX, -FLT_MIN, "Boundary values"},
    // 混合值
    {1.0f, INFINITY, NAN, "Mixed special values"},
    // 小值
    {1e-30f, 2e-30f, 3e-30f, "Very small values"},
    // a为特殊值
    {INFINITY, 2.0f, 3.0f, "Special value in a"},
    // b为特殊值
    {1.0f, NAN, 2.0f, "Special value in b"},
    // c为特殊值
    {1.0f, 2.0f, -INFINITY, "Special value in c"},
    // a和b为特殊值
    {INFINITY, NAN, 1.0f, "Special values in a and b"},
    // a和c为特殊值
    {NAN, 1.0f, INFINITY, "Special values in a and c"},
    // 所有特殊值
    {INFINITY, NAN, -INFINITY, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ss(&cases[t].a);
        __m128 vb = _mm_load_ss(&cases[t].b);
        __m128 vc = _mm_load_ss(&cases[t].c);
        
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va);
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : %.9g\n", cases[t].a);
        printf("B     : %.9g\n", cases[t].b);
        printf("C     : %.9g\n", cases[t].c);
        printf("Result: %.9g\n\n", res);
    }
    
    printf("VFMADD132SS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ss(&cases[t].a);
        __m128 vc = _mm_load_ss(&cases[t].c);
        
        // 测试内存操作数
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (cases[t].b), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va1);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        printf("A     : %.9g\n", cases[t].a);
        printf("B     : %.9g\n", cases[t].b);
        printf("C     : %.9g\n", cases[t].c);
        printf("Result: %.9g\n\n", res);
    }
    
    printf("VFMADD132SS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADD132SS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFMADD132SS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
