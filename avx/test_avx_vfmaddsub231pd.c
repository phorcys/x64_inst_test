#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <string.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void test_128bit_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(fma_cases_128[t].a);
        __m128d vb = _mm_loadu_pd(fma_cases_128[t].b);
        __m128d vc = _mm_loadu_pd(fma_cases_128[t].c);
        
        // 内联汇编实现 VFMADDSUB231PD (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_storeu_pd(res, va);
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128[t].desc);
        print_double_vec("A     :", fma_cases_128[t].a, 2);
        print_double_vec("B     :", fma_cases_128[t].b, 2);
        print_double_vec("C     :", fma_cases_128[t].c, 2);
        print_double_vec("Result:", res, 2);
        printf("\n");
    }
    printf("VFMADDSUB231PD 128-bit Register-Register Tests Completed\n\n");
}

static void test_128bit_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(fma_cases_128[t].a);
        __m128d vc = _mm_loadu_pd(fma_cases_128[t].c);
        
        double *b_ptr = fma_cases_128[t].b;
        double b_aligned[2] __attribute__((aligned(16)));
        memcpy(b_aligned, fma_cases_128[t].b, sizeof(b_aligned));
        
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m128d*)b_ptr), [c] "x" (vc)
        );
        
        __m128d va2 = va;
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*(const __m128d*)b_aligned), [c] "x" (vc)
        );
        
        double res1[2], res2[2];
        _mm_storeu_pd(res1, va1);
        _mm_storeu_pd(res2, va2);
        
        printf("Test Case: %s (128-bit reg-mem)\n", fma_cases_128[t].desc);
        print_double_vec("A     :", fma_cases_128[t].a, 2);
        print_double_vec("B     :", fma_cases_128[t].b, 2);
        print_double_vec("C     :", fma_cases_128[t].c, 2);
        print_double_vec("Unaligned memory:", res1, 2);
        print_double_vec("Aligned memory  :", res2, 2);
        printf("\n");
    }
    printf("VFMADDSUB231PD 128-bit Register-Memory Tests Completed\n\n");
}

static void test_256bit_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256[t].a);
        __m256d vb = _mm256_loadu_pd(fma_cases_256[t].b);
        __m256d vc = _mm256_loadu_pd(fma_cases_256[t].c);
        
        // 内联汇编实现 VFMADDSUB231PD (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256[t].desc);
        print_double_vec("A     :", fma_cases_256[t].a, 4);
        print_double_vec("B     :", fma_cases_256[t].b, 4);
        print_double_vec("C     :", fma_cases_256[t].c, 4);
        print_double_vec("Result:", res, 4);
        printf("\n");
    }
    printf("VFMADDSUB231PD 256-bit Register-Register Tests Completed\n\n");
}

static void test_256bit_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256[t].a);
        __m256d vc = _mm256_loadu_pd(fma_cases_256[t].c);
        
        double *b_ptr = fma_cases_256[t].b;
        double b_aligned[4] __attribute__((aligned(32)));
        memcpy(b_aligned, fma_cases_256[t].b, sizeof(b_aligned));
        
        __m256d va1 = va;
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m256d*)b_ptr), [c] "x" (vc)
        );
        
        __m256d va2 = va;
        __asm__ __volatile__(
            "vfmaddsub231pd %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*(const __m256d*)b_aligned), [c] "x" (vc)
        );
        
        double res1[4], res2[4];
        _mm256_storeu_pd(res1, va1);
        _mm256_storeu_pd(res2, va2);
        
        printf("Test Case: %s (256-bit reg-mem)\n", fma_cases_256[t].desc);
        print_double_vec("A     :", fma_cases_256[t].a, 4);
        print_double_vec("B     :", fma_cases_256[t].b, 4);
        print_double_vec("C     :", fma_cases_256[t].c, 4);
        print_double_vec("Unaligned memory:", res1, 4);
        print_double_vec("Aligned memory  :", res2, 4);
        printf("\n");
    }
    printf("VFMADDSUB231PD 256-bit Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADDSUB231PD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_128bit_reg_reg_operand();
    test_128bit_reg_mem_operand();
    test_256bit_reg_reg_operand();
    test_256bit_reg_mem_operand();

    printf("All VFMADDSUB231PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
