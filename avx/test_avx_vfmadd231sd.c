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
        // 使用128位双精度测试用例的第一个元素作为标量值
        double a = fma_cases_128[t].a[0];
        double b = fma_cases_128[t].b[0];
        double c = fma_cases_128[t].c[0];
        
        __m128d va = _mm_load_sd(&a);
        __m128d vb = _mm_load_sd(&b);
        __m128d vc = _mm_load_sd(&c);
        
        __asm__ __volatile__(
            "vfmadd231sd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        // 计算预期值: a + (b * c)
        double expected = fma(a, b, c);
        
        printf("Test Case: %s\n", fma_cases_128[t].desc);
        printf("A     : %.18g\n", a);
        printf("B     : %.18g\n", b);
        printf("C     : %.18g\n", c);
        printf("Expected: %.18g\n", expected);
        printf("Result  : %.18g\n\n", res);
    }
    
    printf("VFMADD231SD Register-Register Tests Completed\n\n");
    
    // 添加特殊值测试
    printf("Special Value Tests for VFMADD231SD\n");
    printf("===================================\n");
    
    // 测试 +/-0.0, +/-INF, NaN 等边界情况
    struct {
        double a, b, c;
        const char* desc;
    } special_cases[] = {
        {0.0, 0.0, 0.0, "All zeros"},
        {1.0, INFINITY, 1.0, "Infinity * finite + finite"},
        {INFINITY, 0.0, INFINITY, "Infinity + (0 * Infinity)"},
        {NAN, 1.0, 1.0, "NaN operand"},
        {-0.0, -0.0, -0.0, "Negative zeros"},
        {DBL_MIN, DBL_MIN, DBL_MIN, "Denormal values"},
        {DBL_MAX, 2.0, DBL_MAX, "Overflow case"},
    };
    
    for (int i = 0; i < sizeof(special_cases)/sizeof(special_cases[0]); i++) {
        __m128d va = _mm_load_sd(&special_cases[i].a);
        __m128d vb = _mm_load_sd(&special_cases[i].b);
        __m128d vc = _mm_load_sd(&special_cases[i].c);
        
        __asm__ __volatile__(
            "vfmadd231sd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        double expected = fma(special_cases[i].a, special_cases[i].b, special_cases[i].c);
        
        printf("Test Case: %s\n", special_cases[i].desc);
        printf("A: %.18g, B: %.18g, C: %.18g\n", 
               special_cases[i].a, special_cases[i].b, special_cases[i].c);
        printf("Expected: %.18g, Result: %.18g\n\n", expected, res);
    }
    
    printf("Special Value Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 使用128位双精度测试用例的第一个元素作为标量值
        double a = fma_cases_128[t].a[0];
        double b = fma_cases_128[t].b[0];
        double c = fma_cases_128[t].c[0];
        
        __m128d va = _mm_load_sd(&a);
        __m128d vb = _mm_load_sd(&b);
        
        // 测试内存操作数
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfmadd231sd %[c], %[b], %[a]"
            : [a] "+x" (va1)
            : [b] "x" (vb), [c] "m" (c)
        );
        
        double res;
        _mm_store_sd(&res, va1);
        
        // 计算预期值: (b * c) + a
        double expected = fma(b, c, a);
        
        printf("Memory Operand Test: %s\n", fma_cases_128[t].desc);
        printf("A     : %.18g\n", a);
        printf("B     : %.18g\n", b);
        printf("C     : %.18g\n", c);
        printf("Expected: %.18g\n", expected);
        printf("Result  : %.18g\n\n", res);
    }
    
    printf("VFMADD231SD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADD231SD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFMADD231SD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
