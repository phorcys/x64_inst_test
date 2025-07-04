#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

// 使用公共测试用例，只取每个向量的第一个元素
static double get_scalar_value_pd(const double* vec) {
    return vec[0];
}

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        double a = get_scalar_value_pd(fma_cases_128[t].a);
        double b = get_scalar_value_pd(fma_cases_128[t].b);
        double c = get_scalar_value_pd(fma_cases_128[t].c);
        const char *desc = fma_cases_128[t].desc;
        
        __m128d va = _mm_load_sd(&a);
        __m128d vb = _mm_load_sd(&b);
        __m128d vc = _mm_load_sd(&c);
        
        __asm__ __volatile__(
            "vfmadd132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        printf("Test Case: %s\n", desc);
        printf("A     : %.17g\n", a);
        printf("B     : %.17g\n", b);
        printf("C     : %.17g\n", c);
        printf("Result: %.17g\n\n", res);
    }
    
    // 添加特定测试：负零处理
    {
        double a = -0.0;
        double b = 1.0;
        double c = 2.0;
        
        __m128d va = _mm_load_sd(&a);
        __m128d vb = _mm_load_sd(&b);
        __m128d vc = _mm_load_sd(&c);
        
        __asm__ __volatile__(
            "vfmadd132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        printf("Special Test: Negative Zero Handling\n");
        printf("A     : %.17g\n", a);
        printf("B     : %.17g\n", b);
        printf("C     : %.17g\n", c);
        printf("Result: %.17g\n\n", res);
    }
    
    printf("VFMADD132SD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        double a = get_scalar_value_pd(fma_cases_128[t].a);
        double b = get_scalar_value_pd(fma_cases_128[t].b);
        double c = get_scalar_value_pd(fma_cases_128[t].c);
        const char *desc = fma_cases_128[t].desc;
        
        __m128d va = _mm_load_sd(&a);
        __m128d vc = _mm_load_sd(&c);
        
        // 测试内存操作数
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfmadd132sd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (b), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va1);
        
        printf("Memory Operand Test: %s\n", desc);
        printf("A     : %.17g\n", a);
        printf("B     : %.17g\n", b);
        printf("C     : %.17g\n", c);
        printf("Result: %.17g\n\n", res);
    }
    
    // 添加特定测试：负零处理
    {
        double a = -0.0;
        double b = 1.0;
        double c = 2.0;
        
        __m128d va = _mm_load_sd(&a);
        __m128d vc = _mm_load_sd(&c);
        
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfmadd132sd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (b), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va1);
        
        printf("Special Memory Test: Negative Zero Handling\n");
        printf("A     : %.17g\n", a);
        printf("B     : %.17g\n", b);
        printf("C     : %.17g\n", c);
        printf("Result: %.17g\n\n", res);
    }
    
    printf("VFMADD132SD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADD132SD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFMADD132SD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
