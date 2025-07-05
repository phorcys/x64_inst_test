#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256_pd[t].a);
        __m256d vb = _mm256_loadu_pd(fma_cases_256_pd[t].b);
        __m256d vc = _mm256_loadu_pd(fma_cases_256_pd[t].c);
        
        // 保存原始值用于比较
        double original_a[4];
        _mm256_storeu_pd(original_a, va);
        
        __asm__ __volatile__(
            "vfmsub132pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Test Case: %s (256-bit)\n", fma_cases_256_pd[t].desc);
        printf("A     :: %f %f %f %f\n", 
               original_a[0], original_a[1], original_a[2], original_a[3]);
        printf("B     :: %f %f %f %f\n", 
               fma_cases_256_pd[t].b[0], fma_cases_256_pd[t].b[1],
               fma_cases_256_pd[t].b[2], fma_cases_256_pd[t].b[3]);
        printf("C     :: %f %f %f %f\n", 
               fma_cases_256_pd[t].c[0], fma_cases_256_pd[t].c[1],
               fma_cases_256_pd[t].c[2], fma_cases_256_pd[t].c[3]);
        printf("Result:: %f %f %f %f\n", 
               res[0], res[1], res[2], res[3]);
        printf("\n");
    }
    printf("VFMSUB132PD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256_pd[t].a);
        __m256d vb = _mm256_loadu_pd(fma_cases_256_pd[t].b);
        
        // 保存原始值用于比较
        double original_a[4];
        _mm256_storeu_pd(original_a, va);
        
        // 对齐内存操作数
        __attribute__((aligned(32))) double aligned_c[4] = {
            fma_cases_256_pd[t].c[0],
            fma_cases_256_pd[t].c[1],
            fma_cases_256_pd[t].c[2],
            fma_cases_256_pd[t].c[3]
        };
        
        __asm__ __volatile__(
            "vfmsub132pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*aligned_c)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Memory Operand Test: %s (256-bit)\n", fma_cases_256_pd[t].desc);
        printf("A     :: %f %f %f %f\n", 
               original_a[0], original_a[1], original_a[2], original_a[3]);
        printf("B     :: %f %f %f %f\n", 
               fma_cases_256_pd[t].b[0], fma_cases_256_pd[t].b[1],
               fma_cases_256_pd[t].b[2], fma_cases_256_pd[t].b[3]);
        printf("C     :: %f %f %f %f\n", 
               aligned_c[0], aligned_c[1], aligned_c[2], aligned_c[3]);
        printf("Result:: %f %f %f %f\n", 
               res[0], res[1], res[2], res[3]);
        printf("\n");
    }
    printf("VFMSUB132PD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMSUB132PD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMSUB132PD tests completed.\n");
    
    return 0;
}
