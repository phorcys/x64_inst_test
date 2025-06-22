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
    {1.0f, 5.0f, 9.0f, "Normal values"},
    // 零值
    {0.0f, 5.0f, 9.0f, "Zero values"},
    // 无穷大
    {INFINITY, 1.0f, 1.0f, "Infinity values"},
    // NaN
    {NAN, 1.0f, 2.0f, "NaN values"},
    // 边界值
    {FLT_MIN, 2.0f, FLT_MIN, "Boundary values"},
    // 混合值
    {1.0f, INFINITY, NAN, "Mixed special values"},
    // 小值
    {1e-30f, 2.0f, 1e-30f, "Very small values"},
    // a为特殊值
    {INFINITY, 2.0f, 1.0f, "Special values in a"},
    // b为特殊值
    {1.0f, INFINITY, 5.0f, "Special values in b"},
    // c为特殊值
    {1.0f, 5.0f, INFINITY, "Special values in c"},
    // a和b为特殊值
    {INFINITY, NAN, 1.0f, "Special values in a and b"},
    // a和c为特殊值
    {INFINITY, 1.0f, NAN, "Special values in a and c"},
    // b和c为特殊值
    {1.0f, INFINITY, NAN, "Special values in b and c"},
    // 所有特殊值
    {INFINITY, NAN, INFINITY, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_set_ss(cases[t].a);
        __m128 vb = _mm_set_ss(cases[t].b);
        __m128 vc = _mm_set_ss(cases[t].c);
        
        __asm__ __volatile__(
            "vfnmadd213ss %[b], %[c], %[a]"
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
    
    printf("VFNMADD213SS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_set_ss(cases[t].a);
        __m128 vc = _mm_set_ss(cases[t].c);
        
        // 测试不同的内存寻址方式
        float b = cases[t].b;
        float b_aligned __attribute__((aligned(16)));
        b_aligned = cases[t].b;
        
        // 测试1: 未对齐内存
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfnmadd213ss %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (b), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m128 va2 = va;
        __asm__ __volatile__(
            "vfnmadd213ss %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (b_aligned), [c] "x" (vc)
        );
        
        float res1, res2;
        _mm_store_ss(&res1, va1);
        _mm_store_ss(&res2, va2);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        printf("A     : %.9g\n", cases[t].a);
        printf("B     : %.9g\n", cases[t].b);
        printf("C     : %.9g\n", cases[t].c);
        printf("Unaligned memory: %.9g\n", res1);
        printf("Aligned memory: %.9g\n\n", res2);
    }
    
    printf("VFNMADD213SS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFNMADD213SS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFNMADD213SS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
