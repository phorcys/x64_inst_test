#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void test_reg_reg() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ps(fma_cases_128_ps[t].a);
        __m128 vb = _mm_load_ps(fma_cases_128_ps[t].b);
        __m128 vc = _mm_load_ps(fma_cases_128_ps[t].c);
        
        // 保存原始高位值
        float original_high[3] = {va[1], va[2], va[3]};
        
        // 内联汇编实现 VFMSUB132SS (标量单精度)
        __asm__ __volatile__(
            "vfmsub132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res = va[0];
        float expected = fmaf(fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].b[0], -fma_cases_128_ps[t].c[0]);
        
        printf("Test Case: %s (reg-reg)\n", fma_cases_128_ps[t].desc);
        printf("A=%.9g, B=%.9g, C=%.9g\n",
               fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].b[0], fma_cases_128_ps[t].c[0]);
        printf("Expected: %.9g, Result: %.9g\n", expected, res);
        printf("High elements unchanged: %.9g, %.9g, %.9g (original: %.9g, %.9g, %.9g)\n",
               va[1], va[2], va[3], original_high[0], original_high[1], original_high[2]);
        printf("\n");
    }
}

static void test_reg_mem() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ps(fma_cases_128_ps[t].a);
        float b_val = fma_cases_128_ps[t].b[0];  // 标量值
        __m128 vc = _mm_load_ps(fma_cases_128_ps[t].c);
        
        // 保存原始高位值
        float original_high[3] = {va[1], va[2], va[3]};
        
        // 内联汇编实现 VFMSUB132SS (寄存器-内存)
        __asm__ __volatile__(
            "vfmsub132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (b_val), [c] "x" (vc)
        );
        
        float res = va[0];
        float expected = fmaf(fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].b[0], -fma_cases_128_ps[t].c[0]);
        
        printf("Test Case: %s (reg-mem)\n", fma_cases_128_ps[t].desc);
        printf("A=%.9g, B=%.9g, C=%.9g\n",
               fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].b[0], fma_cases_128_ps[t].c[0]);
        printf("Expected: %.9g, Result: %.9g\n", expected, res);
        printf("High elements unchanged: %.9g, %.9g, %.9g (original: %.9g, %.9g, %.9g)\n",
               va[1], va[2], va[3], original_high[0], original_high[1], original_high[2]);
        printf("\n");
    }
}

int main() {
    printf("==============================\n");
    printf("VFMSUB132SS Comprehensive Tests\n");
    printf("==============================\n\n");
    
    test_reg_reg();
    test_reg_mem();
    
    printf("All VFMSUB132SS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
