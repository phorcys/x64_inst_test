#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_pd test = fma_cases_256_pd[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m256d va = _mm256_loadu_pd(test.a);
        __m256d vb = _mm256_loadu_pd(test.b);
        __m256d vc = _mm256_loadu_pd(test.c);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Test Case: %s (packed double precision)\n", desc);
        printf("A     :: %.6f, %.6f, %.6f, %.6f\n", test.a[0], test.a[1], test.a[2], test.a[3]);
        printf("B     :: %.6f, %.6f, %.6f, %.6f\n", test.b[0], test.b[1], test.b[2], test.b[3]);
        printf("C     :: %.6f, %.6f, %.6f, %.6f\n", test.c[0], test.c[1], test.c[2], test.c[3]);
        printf("Result:: %.6f, %.6f, %.6f, %.6f\n\n", res[0], res[1], res[2], res[3]);
    }
    printf("VFMADD231PD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_pd test = fma_cases_256_pd[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m256d va = _mm256_loadu_pd(test.a);
        __m256d vb = _mm256_loadu_pd(test.b);
        
        // 对齐的内存操作数
        __attribute__((aligned(32))) double aligned_c[4] = {test.c[0], test.c[1], test.c[2], test.c[3]};
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (aligned_c)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        printf("Memory Operand Test: %s (packed double precision)\n", desc);
        printf("A     :: %.6f, %.6f, %.6f, %.6f\n", test.a[0], test.a[1], test.a[2], test.a[3]);
        printf("B     :: %.6f, %.6f, %.6f, %.6f\n", test.b[0], test.b[1], test.b[2], test.b[3]);
        printf("C     :: %.6f, %.6f, %.6f, %.6f\n", test.c[0], test.c[1], test.c[2], test.c[3]);
        printf("Result:: %.6f, %.6f, %.6f, %.6f\n\n", res[0], res[1], res[2], res[3]);
    }
    printf("VFMADD231PD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADD231PD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADD231PD tests completed.\n");
    
    return 0;
}
