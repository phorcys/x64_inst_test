#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand(int is_256bit) {
    const int count = FMA_TEST_CASE_COUNT;
    
    for (int t = 0; t < count; t++) {
        if (is_256bit) {
            __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
            __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
            __m256 vc = _mm256_loadu_ps(fma_cases_256_ps[t].c);
            
            __asm__ __volatile__(
                "vfmadd132ps %[b], %[c], %[a]"
                : [a] "+x" (va)
                : [b] "x" (vb), [c] "x" (vc)
            );
            
            float res[8];
            _mm256_storeu_ps(res, va);
            printf("Test Case: %s\n", fma_cases_256_ps[t].desc);
            print_float_vec("A     :", fma_cases_256_ps[t].a, 8);
            print_float_vec("B     :", fma_cases_256_ps[t].b, 8);
            print_float_vec("C     :", fma_cases_256_ps[t].c, 8);
            print_float_vec("Result:", res, 8);
        } else {
            __m128 va = _mm_loadu_ps(fma_cases_128_ps[t].a);
            __m128 vb = _mm_loadu_ps(fma_cases_128_ps[t].b);
            __m128 vc = _mm_loadu_ps(fma_cases_128_ps[t].c);
            
            __asm__ __volatile__(
                "vfmadd132ps %[b], %[c], %[a]"
                : [a] "+x" (va)
                : [b] "x" (vb), [c] "x" (vc)
            );
            
            float res[4];
            _mm_storeu_ps(res, va);
            printf("Test Case: %s\n", fma_cases_128_ps[t].desc);
            print_float_vec("A     :", fma_cases_128_ps[t].a, 4);
            print_float_vec("B     :", fma_cases_128_ps[t].b, 4);
            print_float_vec("C     :", fma_cases_128_ps[t].c, 4);
            print_float_vec("Result:", res, 4);
        }
        printf("\n");
    }
    
    printf("VFMADD132PS %d-bit Register-Register Tests Completed\n\n", 
           is_256bit ? 256 : 128);
}

static void test_reg_mem_operand(int is_256bit) {
    const int count = FMA_TEST_CASE_COUNT;
    
    for (int t = 0; t < count; t++) {
        if (is_256bit) {
            __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
            __m256 vc = _mm256_loadu_ps(fma_cases_256_ps[t].c);
            
            // 测试未对齐内存
            __m256 va_unaligned = va;
            __asm__ __volatile__(
                "vfmadd132ps %[b], %[c], %[a]"
                : [a] "+x" (va_unaligned)
                : [b] "m" (*(const __m256*)fma_cases_256_ps[t].b), 
                  [c] "x" (vc)
            );
            
            // 测试对齐内存
            float b_aligned[8] ALIGNED(32);
            memcpy(b_aligned, fma_cases_256_ps[t].b, sizeof(b_aligned));
            
            __m256 va_aligned = va;
            __asm__ __volatile__(
                "vfmadd132ps %[b], %[c], %[a]"
                : [a] "+x" (va_aligned)
                : [b] "m" (*(const __m256*)b_aligned), 
                  [c] "x" (vc)
            );
            
            float res_unaligned[8], res_aligned[8];
            _mm256_storeu_ps(res_unaligned, va_unaligned);
            _mm256_storeu_ps(res_aligned, va_aligned);
            
            printf("Memory Test: %s\n", fma_cases_256_ps[t].desc);
            print_float_vec("Unaligned result:", res_unaligned, 8);
            print_float_vec("Aligned result:  ", res_aligned, 8);
        } else {
            __m128 va = _mm_loadu_ps(fma_cases_128_ps[t].a);
            __m128 vc = _mm_loadu_ps(fma_cases_128_ps[t].c);
            
            // 测试未对齐内存
            __m128 va_unaligned = va;
            __asm__ __volatile__(
                "vfmadd132ps %[b], %[c], %[a]"
                : [a] "+x" (va_unaligned)
                : [b] "m" (*(const __m128*)fma_cases_128_ps[t].b), 
                  [c] "x" (vc)
            );
            
            // 测试对齐内存
            float b_aligned[4] ALIGNED(16);
            memcpy(b_aligned, fma_cases_128_ps[t].b, sizeof(b_aligned));
            
            __m128 va_aligned = va;
            __asm__ __volatile__(
                "vfmadd132ps %[b], %[c], %[a]"
                : [a] "+x" (va_aligned)
                : [b] "m" (*(const __m128*)b_aligned), 
                  [c] "x" (vc)
            );
            
            float res_unaligned[4], res_aligned[4];
            _mm_storeu_ps(res_unaligned, va_unaligned);
            _mm_storeu_ps(res_aligned, va_aligned);
            
            printf("Memory Test: %s\n", fma_cases_128_ps[t].desc);
            print_float_vec("Unaligned result:", res_unaligned, 4);
            print_float_vec("Aligned result:  ", res_aligned, 4);
        }
        printf("\n");
    }
    
    printf("VFMADD132PS %d-bit Register-Memory Tests Completed\n\n", 
           is_256bit ? 256 : 128);
}




int main() {
    printf("==================================\n");
    printf("VFMADD132PS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 128位测试
    test_reg_reg_operand(0);
    test_reg_mem_operand(0);
    
    // 256位测试
    test_reg_reg_operand(1);
    test_reg_mem_operand(1);
    
    printf("All tests completed. Verify results on physical CPU vs box64.\n");
    return 0;
}
