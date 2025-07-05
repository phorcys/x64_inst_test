#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例到寄存器
        __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
        __m256 vc = _mm256_loadu_ps(fma_cases_256_ps[t].c);
        
        // 保存原始值用于比较
        float original_a[8], original_b[8], original_c[8];
        _mm256_storeu_ps(original_a, va);
        _mm256_storeu_ps(original_b, vb);
        _mm256_storeu_ps(original_c, vc);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmaddsub213ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Test Case: %s (packed single precision)\n", fma_cases_256_ps[t].desc);
        printf("A     :: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", original_a[i]);
        printf("\nB     :: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", original_b[i]);
        printf("\nC     :: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", original_c[i]);
        printf("\nResult:: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", res[i]);
        printf("\n\n");
    }
    printf("vfmaddsub213PS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例到寄存器
        __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
        
        // 保存原始值用于比较
        float original_a[8], original_b[8], original_c[8];
        _mm256_storeu_ps(original_a, va);
        _mm256_storeu_ps(original_b, vb);
        memcpy(original_c, fma_cases_256_ps[t].c, sizeof(original_c));
        
        // 对齐的内存操作数
        __attribute__((aligned(32))) float aligned_c[8];
        memcpy(aligned_c, fma_cases_256_ps[t].c, sizeof(aligned_c));
        
        // 执行指令
        __asm__ __volatile__(
            "vfmaddsub213ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (aligned_c)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Memory Operand Test: %s (packed single precision)\n", fma_cases_256_ps[t].desc);
        printf("A     :: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", original_a[i]);
        printf("\nB     :: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", original_b[i]);
        printf("\nC     :: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", original_c[i]);
        printf("\nResult:: ");
        for (int i = 0; i < 8; i++) printf("%.6f ", res[i]);
        printf("\n\n");
    }
    printf("vfmaddsub213PS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("vfmaddsub213PS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("vfmaddsub213PS tests completed.\n");
    
    return 0;
}
