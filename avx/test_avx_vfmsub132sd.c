#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 从256位测试用例中取第一个元素作为标量测试
        double a = fma_cases_256_pd[t].a[0];
        double b = fma_cases_256_pd[t].b[0];
        double c = fma_cases_256_pd[t].c[0];
        const char *desc = fma_cases_256_pd[t].desc;
        
        // 加载到寄存器（只使用低64位）
        __m128d va = _mm_load_sd(&a);
        __m128d vb = _mm_load_sd(&b);
        __m128d vc = _mm_load_sd(&c);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmsub132sd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        printf("Test Case: %s (scalar double precision)\n", desc);
        printf("A     :: %.6f\n", a);
        printf("B     :: %.6f\n", b);
        printf("C     :: %.6f\n", c);
        printf("Result:: %.6f\n\n", res);
    }
    printf("VFMSUB132SD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 从256位测试用例中取第一个元素作为标量测试
        double a = fma_cases_256_pd[t].a[0];
        double b = fma_cases_256_pd[t].b[0];
        double c = fma_cases_256_pd[t].c[0];
        const char *desc = fma_cases_256_pd[t].desc;
        
        // 加载到寄存器（只使用低64位）
        __m128d va = _mm_load_sd(&a);
        __m128d vb = _mm_load_sd(&b);
        
        // 对齐的内存操作数
        __attribute__((aligned(16))) double aligned_c = c;
        
        // 执行指令
        __asm__ __volatile__(
            "vfmsub132sd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (aligned_c)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        printf("Memory Operand Test: %s (scalar double precision)\n", desc);
        printf("A     :: %.6f\n", a);
        printf("B     :: %.6f\n", b);
        printf("C     :: %.6f\n", aligned_c);
        printf("Result:: %.6f\n\n", res);
    }
    printf("VFMSUB132SD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMSUB132SD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMSUB132SD tests completed.\n");
    
    return 0;
}
