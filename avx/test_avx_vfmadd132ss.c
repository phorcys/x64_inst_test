#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

// 使用公共测试用例，只取每个向量的第一个元素
static float get_scalar_value_ps(const float* vec) {
    return vec[0];
}

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        float a = get_scalar_value_ps(fma_cases_128_ps[t].a);
        float b = get_scalar_value_ps(fma_cases_128_ps[t].b);
        float c = get_scalar_value_ps(fma_cases_128_ps[t].c);
        const char *desc = fma_cases_128_ps[t].desc;
        
        __m128 va = _mm_load_ss(&a);
        __m128 vb = _mm_load_ss(&b);
        __m128 vc = _mm_load_ss(&c);
        
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va);
        
        printf("Test Case: %s\n", desc);
        printf("A     : %.9g\n", a);
        printf("B     : %.9g\n", b);
        printf("C     : %.9g\n", c);
        printf("Result: %.9g\n\n", res);
    }
    
    // 添加特定测试：负零处理
    {
        float a = -0.0f;
        float b = 1.0f;
        float c = 2.0f;
        
        __m128 va = _mm_load_ss(&a);
        __m128 vb = _mm_load_ss(&b);
        __m128 vc = _mm_load_ss(&c);
        
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va);
        
        printf("Special Test: Negative Zero Handling\n");
        printf("A     : %.9g\n", a);
        printf("B     : %.9g\n", b);
        printf("C     : %.9g\n", c);
        printf("Result: %.9g\n\n", res);
    }
    
    printf("VFMADD132SS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        float a = get_scalar_value_ps(fma_cases_128_ps[t].a);
        float b = get_scalar_value_ps(fma_cases_128_ps[t].b);
        float c = get_scalar_value_ps(fma_cases_128_ps[t].c);
        const char *desc = fma_cases_128_ps[t].desc;
        
        __m128 va = _mm_load_ss(&a);
        __m128 vc = _mm_load_ss(&c);
        
        // 测试内存操作数
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (b), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va1);
        
        printf("Memory Operand Test: %s\n", desc);
        printf("A     : %.9g\n", a);
        printf("B     : %.9g\n", b);
        printf("C     : %.9g\n", c);
        printf("Result: %.9g\n\n", res);
    }
    
    // 添加特定测试：负零处理
    {
        float a = -0.0f;
        float b = 1.0f;
        float c = 2.0f;
        
        __m128 va = _mm_load_ss(&a);
        __m128 vc = _mm_load_ss(&c);
        
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (b), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va1);
        
        printf("Special Memory Test: Negative Zero Handling\n");
        printf("A     : %.9g\n", a);
        printf("B     : %.9g\n", b);
        printf("C     : %.9g\n", c);
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
