#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 从256位测试用例中取第一个元素作为标量测试
        float a = fma_cases_256_ps[t].a[0];
        float b = fma_cases_256_ps[t].b[0];
        float c = fma_cases_256_ps[t].c[0];
        const char *desc = fma_cases_256_ps[t].desc;
        
        // 加载到寄存器（只使用低32位）
        __m128 va = _mm_load_ss(&a);
        __m128 vb = _mm_load_ss(&b);
        __m128 vc = _mm_load_ss(&c);
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res;
        _mm_store_ss(&res, va);
        
        printf("Test Case: %s (scalar single precision)\n", desc);
        printf("A     :: %.6f\n", a);
        printf("B     :: %.6f\n", b);
        printf("C     :: %.6f\n", c);
        printf("Result:: %.6f\n\n", res);
    }
    printf("vfmadd231SS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 从256位测试用例中取第一个元素作为标量测试
        float a = fma_cases_256_ps[t].a[0];
        float b = fma_cases_256_ps[t].b[0];
        float c = fma_cases_256_ps[t].c[0];
        const char *desc = fma_cases_256_ps[t].desc;
        
        // 加载到寄存器（只使用低32位）
        __m128 va = _mm_load_ss(&a);
        __m128 vb = _mm_load_ss(&b);
        
        // 对齐的内存操作数
        __attribute__((aligned(16))) float aligned_c = c;
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (aligned_c)
        );
        
        float res;
        _mm_store_ss(&res, va);
        
        printf("Memory Operand Test: %s (scalar single precision)\n", desc);
        printf("A     :: %.6f\n", a);
        printf("B     :: %.6f\n", b);
        printf("C     :: %.6f\n", aligned_c);
        printf("Result:: %.6f\n\n", res);
    }
    printf("vfmadd231SS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("vfmadd231SS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("vfmadd231SS tests completed.\n");
    
    return 0;
}
