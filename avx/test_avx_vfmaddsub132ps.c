#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
        __m256 vc = _mm256_loadu_ps(fma_cases_256_ps[t].c);
        
        // 保存原始值用于比较
        float original_a[8];
        _mm256_storeu_ps(original_a, va);
        
        __asm__ __volatile__(
            "vfmaddsub132ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Test Case: %s\n", fma_cases_256_ps[t].desc);
        printf("A     :: %f %f %f %f %f %f %f %f\n", 
               original_a[0], original_a[1], original_a[2], original_a[3],
               original_a[4], original_a[5], original_a[6], original_a[7]);
        printf("B     :: %f %f %f %f %f %f %f %f\n", 
               fma_cases_256_ps[t].b[0], fma_cases_256_ps[t].b[1],
               fma_cases_256_ps[t].b[2], fma_cases_256_ps[t].b[3],
               fma_cases_256_ps[t].b[4], fma_cases_256_ps[t].b[5],
               fma_cases_256_ps[t].b[6], fma_cases_256_ps[t].b[7]);
        printf("C     :: %f %f %f %f %f %f %f %f\n", 
               fma_cases_256_ps[t].c[0], fma_cases_256_ps[t].c[1],
               fma_cases_256_ps[t].c[2], fma_cases_256_ps[t].c[3],
               fma_cases_256_ps[t].c[4], fma_cases_256_ps[t].c[5],
               fma_cases_256_ps[t].c[6], fma_cases_256_ps[t].c[7]);
        printf("Result:: %f %f %f %f %f %f %f %f\n", 
               res[0], res[1], res[2], res[3],
               res[4], res[5], res[6], res[7]);
        printf("\n");
    }
    printf("VFMADDSUB132PS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
        
        // 保存原始值用于比较
        float original_a[8];
        _mm256_storeu_ps(original_a, va);
        
        // 对齐内存操作数
        __attribute__((aligned(32))) float aligned_c[8] = {
            fma_cases_256_ps[t].c[0], fma_cases_256_ps[t].c[1],
            fma_cases_256_ps[t].c[2], fma_cases_256_ps[t].c[3],
            fma_cases_256_ps[t].c[4], fma_cases_256_ps[t].c[5],
            fma_cases_256_ps[t].c[6], fma_cases_256_ps[t].c[7]
        };
        
        __asm__ __volatile__(
            "vfmaddsub132ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*aligned_c)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Memory Operand Test: %s\n", fma_cases_256_ps[t].desc);
        printf("A     :: %f %f %f %f %f %f %f %f\n", 
               original_a[0], original_a[1], original_a[2], original_a[3],
               original_a[4], original_a[5], original_a[6], original_a[7]);
        printf("B     :: %f %f %f %f %f %f %f %f\n", 
               fma_cases_256_ps[t].b[0], fma_cases_256_ps[t].b[1],
               fma_cases_256_ps[t].b[2], fma_cases_256_ps[t].b[3],
               fma_cases_256_ps[t].b[4], fma_cases_256_ps[t].b[5],
               fma_cases_256_ps[t].b[6], fma_cases_256_ps[t].b[7]);
        printf("C     :: %f %f %f %f %f %f %f %f\n", 
               aligned_c[0], aligned_c[1], aligned_c[2], aligned_c[3],
               aligned_c[4], aligned_c[5], aligned_c[6], aligned_c[7]);
        printf("Result:: %f %f %f %f %f %f %f %f\n", 
               res[0], res[1], res[2], res[3],
               res[4], res[5], res[6], res[7]);
        printf("\n");
    }
    printf("VFMADDSUB132PS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("================================\n");
    printf("VFMADDSUB132PS Comprehensive Tests\n");
    printf("================================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADDSUB132PS tests completed.\n");
    
    return 0;
}
