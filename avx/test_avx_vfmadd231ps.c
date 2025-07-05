#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_ps test = fma_cases_256_ps[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m256 va = _mm256_loadu_ps(test.a);
        __m256 vb = _mm256_loadu_ps(test.b);
        __m256 vc = _mm256_loadu_ps(test.c);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Test Case: %s (packed single precision)\n", desc);
        printf("A     :: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", 
               test.a[0], test.a[1], test.a[2], test.a[3],
               test.a[4], test.a[5], test.a[6], test.a[7]);
        printf("B     :: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", 
               test.b[0], test.b[1], test.b[2], test.b[3],
               test.b[4], test.b[5], test.b[6], test.b[7]);
        printf("C     :: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", 
               test.c[0], test.c[1], test.c[2], test.c[3],
               test.c[4], test.c[5], test.c[6], test.c[7]);
        printf("Result:: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n\n", 
               res[0], res[1], res[2], res[3], res[4], res[5], res[6], res[7]);
    }
    printf("VFMADD231PS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_ps test = fma_cases_256_ps[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m256 va = _mm256_loadu_ps(test.a);
        __m256 vb = _mm256_loadu_ps(test.b);
        
        // 对齐的内存操作数
        __attribute__((aligned(32))) float aligned_c[8] = {
            test.c[0], test.c[1], test.c[2], test.c[3],
            test.c[4], test.c[5], test.c[6], test.c[7]
        };
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231ps %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (aligned_c)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        printf("Memory Operand Test: %s (packed single precision)\n", desc);
        printf("A     :: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", 
               test.a[0], test.a[1], test.a[2], test.a[3],
               test.a[4], test.a[5], test.a[6], test.a[7]);
        printf("B     :: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", 
               test.b[0], test.b[1], test.b[2], test.b[3],
               test.b[4], test.b[5], test.b[6], test.b[7]);
        printf("C     :: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n", 
               test.c[0], test.c[1], test.c[2], test.c[3],
               test.c[4], test.c[5], test.c[6], test.c[7]);
        printf("Result:: %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n\n", 
               res[0], res[1], res[2], res[3], res[4], res[5], res[6], res[7]);
    }
    printf("VFMADD231PS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADD231PS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADD231PS tests completed.\n");
    
    return 0;
}
