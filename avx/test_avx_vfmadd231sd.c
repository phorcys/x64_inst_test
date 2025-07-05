#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"
#include "fma.h"

static void test_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例 (使用256位双精度测试用例)
        fma_test_case_256_pd test = fma_cases_256_pd[t];
        const char *desc = test.desc;
        
        // 加载到寄存器 (只使用前两个元素)
        __m128d va = _mm_set_sd(test.a[0]); // 低64位 = a[0], 高64位 = 0
        __m128d vb = _mm_set_sd(test.b[0]); // 低64位 = b[0], 高64位 = 0
        __m128d vc = _mm_set_sd(test.c[0]); // 低64位 = c[0], 高64位 = 0
        
        // 执行指令 (只影响低64位)
        __asm__ __volatile__(
            "vfmadd231sd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        double result;
        _mm_store_sd(&result, vb);
        
        printf("Test Case: %s (scalar double precision)\n", desc);
        printf("A     :: %.6f\n", test.a[0]);
        printf("B     :: %.6f\n", test.b[0]);
        printf("C     :: %.6f\n", test.c[0]);
        printf("Result:: %.6f\n", result);
        printf("Expected:: %.6f\n\n", test.a[0] * test.c[0] + test.b[0]);
    }
    printf("VFMADD231SD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_pd test = fma_cases_256_pd[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m128d va = _mm_set_sd(test.a[0]); // 低64位 = a[0]
        __m128d vb = _mm_set_sd(test.b[0]); // 低64位 = b[0]
        
        // 内存操作数 (只需要一个双精度值)
        double c_val = test.c[0];
        
        // 执行指令
        __asm__ __volatile__(
            "vfmadd231sd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "m" (c_val)
        );
        
        double result;
        _mm_store_sd(&result, vb);
        
        printf("Memory Operand Test: %s (scalar double precision)\n", desc);
        printf("A     :: %.6f\n", test.a[0]);
        printf("B     :: %.6f\n", test.b[0]);
        printf("C     :: %.6f\n", test.c[0]);
        printf("Result:: %.6f\n", result);
        printf("Expected:: %.6f\n\n", test.a[0] * test.c[0] + test.b[0]);
    }
    printf("VFMADD231SD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADD231SD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADD231SD tests completed.\n");
    
    return 0;
}
