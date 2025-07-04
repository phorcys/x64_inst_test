#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void calculate_expected(fma_test_case_128* test_case, double expected[2]) {
    for (int i = 0; i < 2; i++) {
        expected[i] = fma(test_case->a[i], test_case->b[i], -test_case->c[i]);
    }
}

static void test_reg_reg_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_pd(fma_cases_128[t].a);
        __m128d vb = _mm_load_pd(fma_cases_128[t].b);
        __m128d vc = _mm_load_pd(fma_cases_128[t].c);
        
        // 内联汇编实现 VFMSUB132PD (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmsub132pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_store_pd(res, va);
        
        double expected[2];
        calculate_expected(&fma_cases_128[t], expected);
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_pd(fma_cases_128[t].a);
        double* b_ptr = fma_cases_128[t].b;
        __m128d vc = _mm_load_pd(fma_cases_128[t].c);
        
        // 内联汇编实现 VFMSUB132PD (128位寄存器-内存)
        __asm__ __volatile__(
            "vfmsub132pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        double res[2];
        _mm_store_pd(res, va);
        
        double expected[2];
        calculate_expected(&fma_cases_128[t], expected);
        
        printf("Test Case: %s (128-bit reg-mem)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void calculate_expected_256(fma_test_case_256* test_case, double expected[4]) {
    for (int i = 0; i < 4; i++) {
        expected[i] = fma(test_case->a[i], test_case->b[i], -test_case->c[i]);
    }
}

static void test_reg_reg_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_load_pd(fma_cases_256[t].a);
        __m256d vb = _mm256_load_pd(fma_cases_256[t].b);
        __m256d vc = _mm256_load_pd(fma_cases_256[t].c);
        
        // 内联汇编实现 VFMSUB132PD (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmsub132pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_store_pd(res, va);
        
        double expected[4];
        calculate_expected_256(&fma_cases_256[t], expected);
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_load_pd(fma_cases_256[t].a);
        double* b_ptr = fma_cases_256[t].b;
        __m256d vc = _mm256_load_pd(fma_cases_256[t].c);
        
        // 内联汇编实现 VFMSUB132PD (256位寄存器-内存)
        __asm__ __volatile__(
            "vfmsub132pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_store_pd(res, va);
        
        double expected[4];
        calculate_expected_256(&fma_cases_256[t], expected);
        
        printf("Test Case: %s (256-bit reg-mem)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

int main() {
    printf("===============================\n");
    printf("VFMSUB132PD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_128();
    test_reg_mem_128();
    test_reg_reg_256();
    test_reg_mem_256();
    
    printf("All VFMSUB132PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
