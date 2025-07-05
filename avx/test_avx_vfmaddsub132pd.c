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
        
        // 执行指令 (VFMADDSUB132PD: 操作顺序为 dst = dst * src1 ± src2)
        // 修正操作数绑定：实际行为是 dst = dst * src2 ± src1
        __asm__ __volatile__(
            "vfmaddsub132pd %[a], %[c], %[b]"
            : [b] "+x" (vb)
            : [c] "x" (vc), [a] "x" (va)
        );
        
        // 提取结果
        double result[4];
        _mm256_storeu_pd(result, vb);
        
        // 计算预期结果 (正确顺序: dst = dst * src1 ± src2)
        double expected[4];
        for (int i = 0; i < 4; i++) {
            double b_val = test.b[i]; // dst (被修改)
            double a_val = test.a[i]; // src1
            double c_val = test.c[i]; // src2
            
            if (i % 2 == 0) { // 偶数索引：乘加
                expected[i] = b_val * a_val + c_val;
            } else { // 奇数索引：乘减
                expected[i] = b_val * a_val - c_val;
            }
        }
        
        printf("Test Case: %s\n", desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: B=%.6f, A=%.6f, C=%.6f => Result=%.6f, Expected=%.6f\n",
                   i, test.b[i], test.a[i], test.c[i], result[i], expected[i]);
        }
        printf("\n");
    }
    printf("VFMADDSUB132PD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        // 加载测试用例
        fma_test_case_256_pd test = fma_cases_256_pd[t];
        const char *desc = test.desc;
        
        // 加载到寄存器
        __m256d va = _mm256_loadu_pd(test.a);
        __m256d vb = _mm256_loadu_pd(test.b);
        
        // 内存操作数
        double c_vals[4];
        for (int i = 0; i < 4; i++) {
            c_vals[i] = test.c[i];
        }
        
        // 执行指令
        // 修正操作数绑定：实际行为是 dst = dst * src2 ± src1
        __asm__ __volatile__(
            "vfmaddsub132pd %[a], %[c], %[b]"
            : [b] "+x" (vb)
            : [c] "x" (va), [a] "m" (*c_vals)
        );
        
        // 提取结果
        double result[4];
        _mm256_storeu_pd(result, vb);
        
        // 计算预期结果 (正确顺序: dst = dst * src1 ± src2)
        double expected[4];
        for (int i = 0; i < 4; i++) {
            double b_val = test.b[i]; // dst (被修改)
            double a_val = test.a[i]; // src1
            double c_val = test.c[i]; // src2
            
            if (i % 2 == 0) { // 偶数索引：乘加
                expected[i] = b_val * a_val + c_val;
            } else { // 奇数索引：乘减
                expected[i] = b_val * a_val - c_val;
            }
        }
        
        printf("Memory Operand Test: %s\n", desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: B=%.6f, A=%.6f, C=%.6f => Result=%.6f, Expected=%.6f\n",
                   i, test.b[i], test.a[i], test.c[i], result[i], expected[i]);
        }
        printf("\n");
    }
    printf("VFMADDSUB132PD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("===============================\n");
    printf("VFMADDSUB132PD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("VFMADDSUB132PD tests completed.\n");
    
    return 0;
}
