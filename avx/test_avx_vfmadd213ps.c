#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载256位测试用例
        float a[8] = {fma_cases_256_ps[t].a[0], fma_cases_256_ps[t].a[1], 
                     fma_cases_256_ps[t].a[2], fma_cases_256_ps[t].a[3],
                     fma_cases_256_ps[t].a[4], fma_cases_256_ps[t].a[5],
                     fma_cases_256_ps[t].a[6], fma_cases_256_ps[t].a[7]};
        float b[8] = {fma_cases_256_ps[t].b[0], fma_cases_256_ps[t].b[1], 
                     fma_cases_256_ps[t].b[2], fma_cases_256_ps[t].b[3],
                     fma_cases_256_ps[t].b[4], fma_cases_256_ps[t].b[5],
                     fma_cases_256_ps[t].b[6], fma_cases_256_ps[t].b[7]};
        float c[8] = {fma_cases_256_ps[t].c[0], fma_cases_256_ps[t].c[1], 
                     fma_cases_256_ps[t].c[2], fma_cases_256_ps[t].c[3],
                     fma_cases_256_ps[t].c[4], fma_cases_256_ps[t].c[5],
                     fma_cases_256_ps[t].c[6], fma_cases_256_ps[t].c[7]};
        const char *desc = fma_cases_256_ps[t].desc;
        
        // 加载到寄存器
        __m256 va = _mm256_loadu_ps(a);
        __m256 vb = _mm256_loadu_ps(b);
        __m256 vc = _mm256_loadu_ps(c);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd213ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Test Case: %s (packed single precision)\n", desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.6f, B=%.6f, C=%.6f, Result=%.6f\n", 
                   i, a[i], b[i], c[i], res[i]);
        }
        printf("\n");
    }
    printf("VFMADD213PS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载256位测试用例
        float a[8] = {fma_cases_256_ps[t].a[0], fma_cases_256_ps[t].a[1], 
                     fma_cases_256_ps[t].a[2], fma_cases_256_ps[t].a[3],
                     fma_cases_256_ps[t].a[4], fma_cases_256_ps[t].a[5],
                     fma_cases_256_ps[t].a[6], fma_cases_256_ps[t].a[7]};
        float b[8] = {fma_cases_256_ps[t].b[0], fma_cases_256_ps[t].b[1], 
                     fma_cases_256_ps[t].b[2], fma_cases_256_ps[t].b[3],
                     fma_cases_256_ps[t].b[4], fma_cases_256_ps[t].b[5],
                     fma_cases_256_ps[t].b[6], fma_cases_256_ps[t].b[7]};
        float c[8] = {fma_cases_256_ps[t].c[0], fma_cases_256_ps[t].c[1], 
                     fma_cases_256_ps[t].c[2], fma_cases_256_ps[t].c[3],
                     fma_cases_256_ps[t].c[4], fma_cases_256_ps[t].c[5],
                     fma_cases_256_ps[t].c[6], fma_cases_256_ps[t].c[7]};
        const char *desc = fma_cases_256_ps[t].desc;
        
        // 加载到寄存器
        __m256 va = _mm256_loadu_ps(a);
        __m256 vb = _mm256_loadu_ps(b);
        
        // 对齐的内存操作数
        __attribute__((aligned(32))) float aligned_c[8] = {
            c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]
        };
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd213ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*aligned_c)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Memory Operand Test: %s (packed single precision)\n", desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.6f, B=%.6f, C=%.6f, Result=%.6f\n", 
                   i, a[i], b[i], c[i], res[i]);
        }
        printf("\n");
    }
    printf("VFMADD213PS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADD213PS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADD213PS tests completed.\n");
    
    return 0;
}
