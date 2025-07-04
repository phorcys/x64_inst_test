#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void test_reg_reg() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_pd(fma_cases_128[t].a);
        __m128d vb = _mm_load_pd(fma_cases_128[t].b);
        __m128d vc = _mm_load_pd(fma_cases_128[t].c);
        
        // 保存原始高位值
        double original_high = va[1];
        
        // 内联汇编实现 VFMSUB132SD (标量双精度)
        __asm__ __volatile__(
            "vfmsub132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res = va[0];
        double expected = fma(fma_cases_128[t].a[0], fma_cases_128[t].b[0], -fma_cases_128[t].c[0]);
        
        printf("Test Case: %s (reg-reg)\n", fma_cases_128[t].desc);
        printf("A=%.18g, B=%.18g, C=%.18g\n",
               fma_cases_128[t].a[0], fma_cases_128[t].b[0], fma_cases_128[t].c[0]);
        printf("Expected: %.18g, Result: %.18g\n", expected, res);
        printf("High element unchanged: %.18g (original: %.18g)\n", va[1], original_high);
        printf("\n");
    }
}

static void test_reg_mem() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_pd(fma_cases_128[t].a);
        double b_val = fma_cases_128[t].b[0];  // 标量值
        __m128d vc = _mm_load_pd(fma_cases_128[t].c);
        
        // 保存原始高位值
        double original_high = va[1];
        
        // 内联汇编实现 VFMSUB132SD (寄存器-内存)
        __asm__ __volatile__(
            "vfmsub132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (b_val), [c] "x" (vc)
        );
        
        double res = va[0];
        double expected = fma(fma_cases_128[t].a[0], fma_cases_128[t].b[0], -fma_cases_128[t].c[0]);
        
        printf("Test Case: %s (reg-mem)\n", fma_cases_128[t].desc);
        printf("A=%.18g, B=%.18g, C=%.18g\n",
               fma_cases_128[t].a[0], fma_cases_128[t].b[0], fma_cases_128[t].c[0]);
        printf("Expected: %.18g, Result: %.18g\n", expected, res);
        printf("High element unchanged: %.18g (original: %.18g)\n", va[1], original_high);
        printf("\n");
    }
}

int main() {
    printf("==============================\n");
    printf("VFMSUB132SD Comprehensive Tests\n");
    printf("==============================\n\n");
    
    test_reg_reg();
    test_reg_mem();
    
    printf("All VFMSUB132SD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
