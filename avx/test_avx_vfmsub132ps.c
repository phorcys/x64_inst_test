#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void calculate_expected(fma_test_case_128_ps* test_case, float expected[4]) {
    for (int i = 0; i < 4; i++) {
        expected[i] = fmaf(test_case->a[i], test_case->b[i], -test_case->c[i]);
    }
}

static void test_reg_reg_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ps(fma_cases_128_ps[t].a);
        __m128 vb = _mm_load_ps(fma_cases_128_ps[t].b);
        __m128 vc = _mm_load_ps(fma_cases_128_ps[t].c);
        
        // 内联汇编实现 VFMSUB132PS (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, va);
        
        float expected[4];
        calculate_expected(&fma_cases_128_ps[t], expected);
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128_ps[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_128_ps[t].a[i], fma_cases_128_ps[t].b[i], fma_cases_128_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ps(fma_cases_128_ps[t].a);
        float* b_ptr = fma_cases_128_ps[t].b;
        __m128 vc = _mm_load_ps(fma_cases_128_ps[t].c);
        
        // 内联汇编实现 VFMSUB132PS (128位寄存器-内存)
        __asm__ __volatile__(
            "vfmsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, va);
        
        float expected[4];
        calculate_expected(&fma_cases_128_ps[t], expected);
        
        printf("Test Case: %s (128-bit reg-mem)\n", fma_cases_128_ps[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_128_ps[t].a[i], fma_cases_128_ps[t].b[i], fma_cases_128_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void calculate_expected_256(fma_test_case_256_ps* test_case, float expected[8]) {
    for (int i = 0; i < 8; i++) {
        expected[i] = fmaf(test_case->a[i], test_case->b[i], -test_case->c[i]);
    }
}

static void test_reg_reg_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_load_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_load_ps(fma_cases_256_ps[t].b);
        __m256 vc = _mm256_load_ps(fma_cases_256_ps[t].c);
        
        // 内联汇编实现 VFMSUB132PS (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, va);
        
        float expected[8];
        calculate_expected_256(&fma_cases_256_ps[t], expected);
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256_ps[t].desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_256_ps[t].a[i], fma_cases_256_ps[t].b[i], fma_cases_256_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_load_ps(fma_cases_256_ps[t].a);
        float* b_ptr = fma_cases_256_ps[t].b;
        __m256 vc = _mm256_load_ps(fma_cases_256_ps[t].c);
        
        // 内联汇编实现 VFMSUB132PS (256位寄存器-内存)
        __asm__ __volatile__(
            "vfmsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, va);
        
        float expected[8];
        calculate_expected_256(&fma_cases_256_ps[t], expected);
        
        printf("Test Case: %s (256-bit reg-mem)\n", fma_cases_256_ps[t].desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_256_ps[t].a[i], fma_cases_256_ps[t].b[i], fma_cases_256_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

int main() {
    printf("===============================\n");
    printf("VFMSUB132PS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_128();
    test_reg_mem_128();
    test_reg_reg_256();
    test_reg_mem_256();
    
    printf("All VFMSUB132PS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
