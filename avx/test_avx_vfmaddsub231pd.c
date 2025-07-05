#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <float.h>
#include "avx.h"
#include "fma.h"

static void test_256bit_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256_pd[t].a);
        __m256d vb = _mm256_loadu_pd(fma_cases_256_pd[t].b);
        __m256d vc = _mm256_loadu_pd(fma_cases_256_pd[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        printf("Test Case: %s\n", fma_cases_256_pd[t].desc);
        print_double_vec("A     :", fma_cases_256_pd[t].a, 4);
        print_double_vec("B     :", fma_cases_256_pd[t].b, 4);
        print_double_vec("C     :", fma_cases_256_pd[t].c, 4);
        print_double_vec("Result:", res, 4);
        printf("\n");
    }
    printf("vfmaddsub231PD 256-bit Register-Register Tests Completed\n\n");
}

static void test_128bit_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(fma_cases_256_pd[t].a);
        __m128d vb = _mm_loadu_pd(fma_cases_256_pd[t].b);
        __m128d vc = _mm_loadu_pd(fma_cases_256_pd[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_storeu_pd(res, va);
        
        printf("Test Case: %s\n", fma_cases_256_pd[t].desc);
        print_double_vec("A     :", fma_cases_256_pd[t].a, 2);
        print_double_vec("B     :", fma_cases_256_pd[t].b, 2);
        print_double_vec("C     :", fma_cases_256_pd[t].c, 2);
        print_double_vec("Result:", res, 2);
        printf("\n");
    }
    printf("vfmaddsub231PD 128-bit Register-Register Tests Completed\n\n");
}

static void test_256bit_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256_pd[t].a);
        __m256d vc = _mm256_loadu_pd(fma_cases_256_pd[t].c);
        
        double *b_ptr = fma_cases_256_pd[t].b;
        __m256d va1 = va;
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m256d*)b_ptr), [c] "x" (vc)
        );

        double res[4];
        _mm256_storeu_pd(res, va1);
        
        printf("Memory Operand Test: %s\n", fma_cases_256_pd[t].desc);
        print_double_vec("A     :", fma_cases_256_pd[t].a, 4);
        print_double_vec("B     :", fma_cases_256_pd[t].b, 4);
        print_double_vec("C     :", fma_cases_256_pd[t].c, 4);
        print_double_vec("Aligned memory:", res, 4);
        printf("\n");
    }
    printf("vfmaddsub231PD 256-bit Register-Memory Tests Completed\n\n");
}

static void test_128bit_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(fma_cases_256_pd[t].a);
        __m128d vc = _mm_loadu_pd(fma_cases_256_pd[t].c);
        
        double *b_ptr = fma_cases_256_pd[t].b;
        
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m128d*)b_ptr), [c] "x" (vc)
        );
        
        double res[2];
        _mm_storeu_pd(res, va1);
        
        printf("Memory Operand Test: %s\n", fma_cases_256_pd[t].desc);
        print_double_vec("A     :", fma_cases_256_pd[t].a, 2);
        print_double_vec("B     :", fma_cases_256_pd[t].b, 2);
        print_double_vec("C     :", fma_cases_256_pd[t].c, 2);
        print_double_vec("Aligned memory:", res, 2);
        printf("\n");
    }
    printf("vfmaddsub231PD 128-bit Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("vfmaddsub231PD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_128bit_reg_reg_operand();
    test_256bit_reg_reg_operand();
    test_128bit_reg_mem_operand();
    test_256bit_reg_mem_operand();
    
    printf("vfmaddsub231PD normal values tests completed.\n");
    
    return 0;
}
