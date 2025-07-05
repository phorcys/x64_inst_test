#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载256位测试用例
        double a[4] = {fma_cases_256_pd[t].a[0], fma_cases_256_pd[t].a[1], 
                      fma_cases_256_pd[t].a[2], fma_cases_256_pd[t].a[3]};
        double b[4] = {fma_cases_256_pd[t].b[0], fma_cases_256_pd[t].b[1], 
                      fma_cases_256_pd[t].b[2], fma_cases_256_pd[t].b[3]};
        double c[4] = {fma_cases_256_pd[t].c[0], fma_cases_256_pd[t].c[1], 
                      fma_cases_256_pd[t].c[2], fma_cases_256_pd[t].c[3]};
        const char *desc = fma_cases_256_pd[t].desc;
        
        // 加载到寄存器
        __m256d va = _mm256_loadu_pd(a);
        __m256d vb = _mm256_loadu_pd(b);
        __m256d vc = _mm256_loadu_pd(c);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Test Case: %s (packed double precision)\n", desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.6f, B=%.6f, C=%.6f, Result=%.6f\n", 
                   i, a[i], b[i], c[i], res[i]);
        }
        printf("\n");
    }
    printf("VFMADD213PD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载256位测试用例
        double a[4] = {fma_cases_256_pd[t].a[0], fma_cases_256_pd[t].a[1], 
                      fma_cases_256_pd[t].a[2], fma_cases_256_pd[t].a[3]};
        double b[4] = {fma_cases_256_pd[t].b[0], fma_cases_256_pd[t].b[1], 
                      fma_cases_256_pd[t].b[2], fma_cases_256_pd[t].b[3]};
        double c[4] = {fma_cases_256_pd[t].c[0], fma_cases_256_pd[t].c[1], 
                      fma_cases_256_pd[t].c[2], fma_cases_256_pd[t].c[3]};
        const char *desc = fma_cases_256_pd[t].desc;
        
        // 加载到寄存器
        __m256d va = _mm256_loadu_pd(a);
        __m256d vb = _mm256_loadu_pd(b);
        
        // 对齐的内存操作数
        __attribute__((aligned(32))) double aligned_c[4] = {c[0], c[1], c[2], c[3]};
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*aligned_c)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Memory Operand Test: %s (packed double precision)\n", desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.6f, B=%.6f, C=%.6f, Result=%.6f\n", 
                   i, a[i], b[i], c[i], res[i]);
        }
        printf("\n");
    }
    printf("VFMADD213PD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADD213PD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADD213PD tests completed.\n");
    
    return 0;
}
