#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例 (使用256位单精度测试用例)
        fma_test_case_256_ps test = fma_cases_256_ps[t];
        const char *desc = test.desc;
        
        // 加载到寄存器 (只使用第一个元素)
        __m128 va = _mm_set_ss(test.a[0]); // 低32位 = a[0], 高32位 = 0
        __m128 vb = _mm_set_ss(test.b[0]); // 低32位 = b[0], 高32位 = 0
        __m128 vc = _mm_set_ss(test.c[0]); // 低32位 = c[0], 高32位 = 0
        
        // 执行指令 (只影响低32位)
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        float result;
        _mm_store_ss(&result, vb);
        
        printf("Test Case: %s (scalar single precision)\n", desc);
        printf("A     :: %.6f\n", test.a[0]);
        printf("B     :: %.6f\n", test.b[0]);
        printf("C     :: %.6f\n", test.c[0]);
        printf("Result:: %.6f\n", result);
        printf("Expected:: %.6f\n\n", test.a[0] * test.c[0] + test.b[0]);
    }
    printf("VFMADD231SS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_ps test = fma_cases_256_ps[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m128 va = _mm_set_ss(test.a[0]); // 低32位 = a[0]
        __m128 vb = _mm_set_ss(test.b[0]); // 低32位 = b[0]
        
        // 内存操作数 (只需要一个单精度值)
        float c_val = test.c[0];
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231ss %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "m" (c_val)
        );
        
        float result;
        _mm_store_ss(&result, vb);
        
        printf("Memory Operand Test: %s (scalar single precision)\n", desc);
        printf("A     :: %.6f\n", test.a[0]);
        printf("B     :: %.6f\n", test.b[0]);
        printf("C     :: %.6f\n", test.c[0]);
        printf("Result:: %.6f\n", result);
        printf("Expected:: %.6f\n\n", test.a[0] * test.c[0] + test.b[0]);
    }
    printf("VFMADD231SS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADD231SS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADD231SS tests completed.\n");
    
    return 0;
}
