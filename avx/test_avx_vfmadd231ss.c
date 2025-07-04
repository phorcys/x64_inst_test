#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 使用128位单精度测试用例的第一个元素作为标量值
        float a = fma_cases_128_ps[t].a[0];
        float b = fma_cases_128_ps[t].b[0];
        float c = fma_cases_128_ps[t].c[0];
        
        __m128 va = _mm_load_ss(&a);
        __m128 vb = _mm_load_ss(&b);
        __m128 vc = _mm_load_ss(&c);
        
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va);
        
        // 计算预期值: a + (b * c)
        float expected = fmaf(a, b, c);
        
        printf("Test Case: %s\n", fma_cases_128_ps[t].desc);
        printf("A     : %.9g\n", a);
        printf("B     : %.9g\n", b);
        printf("C     : %.9g\n", c);
        printf("Expected: %.9g\n", expected);
        printf("Result  : %.9g\n\n", res);
    }
    
    printf("VFMADD231SS Register-Register Tests Completed\n\n");
    
    // 添加特殊值测试
    printf("Special Value Tests for VFMADD231SS\n");
    printf("===================================\n");
    
    // 测试 +/-0.0, +/-INF, NaN 等边界情况
    struct {
        float a, b, c;
        const char* desc;
    } special_cases[] = {
        {0.0f, 0.0f, 0.0f, "All zeros"},
        {1.0f, INFINITY, 1.0f, "Infinity * finite + finite"},
        {INFINITY, 0.0f, INFINITY, "Infinity + (0 * Infinity)"},
        {NAN, 1.0f, 1.0f, "NaN operand"},
        {-0.0f, -0.0f, -0.0f, "Negative zeros"},
        {FLT_MIN, FLT_MIN, FLT_MIN, "Denormal values"},
        {FLT_MAX, 2.0f, FLT_MAX, "Overflow case"},
    };
    
    for (int i = 0; i < sizeof(special_cases)/sizeof(special_cases[0]); i++) {
        __m128 va = _mm_load_ss(&special_cases[i].a);
        __m128 vb = _mm_load_ss(&special_cases[i].b);
        __m128 vc = _mm_load_ss(&special_cases[i].c);
        
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va);
        float expected = fmaf(special_cases[i].a, special_cases[i].b, special_cases[i].c);
        
        printf("Test Case: %s\n", special_cases[i].desc);
        printf("A: %.9g, B: %.9g, C: %.9g\n", 
               special_cases[i].a, special_cases[i].b, special_cases[i].c);
        printf("Expected: %.9g, Result: %.9g\n\n", expected, res);
    }
    
    printf("Special Value Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 使用128位单精度测试用例的第一个元素作为标量值
        float a = fma_cases_128_ps[t].a[0];
        float b = fma_cases_128_ps[t].b[0];
        float c = fma_cases_128_ps[t].c[0];
        
        __m128 va = _mm_load_ss(&a);
        __m128 vb = _mm_load_ss(&b);
        
        // 测试内存操作数
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[b], %[a]"
            : [a] "+x" (va1)
            : [b] "x" (vb), [c] "m" (c)
        );
        
        float res;
        _mm_store_ss(&res, va1);
        
        // 计算预期值: (b * c) + a
        float expected = fmaf(b, c, a);
        
        printf("Memory Operand Test: %s\n", fma_cases_128_ps[t].desc);
        printf("A     : %.9g\n", a);
        printf("B     : %.9g\n", b);
        printf("C     : %.9g\n", c);
        printf("Expected: %.9g\n", expected);
        printf("Result  : %.9g\n\n", res);
    }
    
    printf("VFMADD231SS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADD231SS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFMADD231SS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
