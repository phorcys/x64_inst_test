/*
 * 注意：此测试文件暂时无法通过编译，需要进一步调试内联汇编语法问题
 * 错误信息：operand type mismatch for `vmovsd'
 * 待解决的问题：需要找到正确的内联汇编语法来测试VMOVSD指令
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <immintrin.h>
#include <float.h>
#include "avx.h"

// 测试VMOVSD指令
void test_vmovsd() {
    int errors = 0;
    printf("Testing VMOVSD instruction...\n");

    // 测试寄存器到寄存器传输
    {
        double src_val = 3.141592653589793;
        __m128d src = _mm_set_sd(src_val);
        __m128d dst;

        // 寄存器形式: vmovsd xmm2, xmm1, xmm1
        asm volatile("vmovsd %[src], %[src], %[dst]" : [dst] "=x"(dst) : [src] "x"(src));
        
        double dst_val = _mm_cvtsd_f64(dst);
        
        if (src_val != dst_val) {
            printf("Error in reg-to-reg transfer\n");
            printf("Expected: %f\n", src_val);
            printf("Got:      %f\n", dst_val);
            errors++;
        }
    }

    // 测试内存到寄存器传输
    {
        double mem_val = -123.456789;
        __m128d dst;

        // 内存到寄存器形式: vmovsd xmm1, m64
        asm volatile("vmovsd %[mem], %[dst]" : [dst] "=x"(dst) : [mem] "m"(mem_val));
        
        double dst_val = _mm_cvtsd_f64(dst);
        
        if (mem_val != dst_val) {
            printf("Error in mem-to-reg transfer\n");
            printf("Expected: %f\n", mem_val);
            printf("Got:      %f\n", dst_val);
            errors++;
        }
    }

    // 测试寄存器到内存传输
    {
        double expected = 789.012345;
        __m128d src = _mm_set_sd(expected);
        double mem_val;

        // 寄存器到内存形式: vmovsd m64, xmm1
        asm volatile("vmovsd %[src], %[mem]" : [mem] "=m"(mem_val) : [src] "x"(src));
        
        if (expected != mem_val) {
            printf("Error in reg-to-mem transfer\n");
            printf("Expected: %f\n", expected);
            printf("Got:      %f\n", mem_val);
            errors++;
        }
    }

    // 测试特殊值
    {
        double test_values[] = {0.0, -0.0, INFINITY, -INFINITY, NAN, -NAN, 2.2250738585072014e-308, 1.7976931348623157e+308};
        int num_tests = sizeof(test_values) / sizeof(test_values[0]);
        
        for (int i = 0; i < num_tests; i++) {
            double src_val = test_values[i];
            __m128d src = _mm_set_sd(src_val);
            __m128d dst;
            double mem_val;

            // 寄存器到寄存器
            asm volatile("vmovsd %[src], %[src], %[dst]" : [dst] "=x"(dst) : [src] "x"(src));
            double dst_val = _mm_cvtsd_f64(dst);
            
            // 寄存器到内存
            asm volatile("vmovsd %[src], %[mem]" : [mem] "=m"(mem_val) : [src] "x"(src));
            
            // 特殊值需要特殊比较
            if (isnan(src_val)) {
                if (!isnan(dst_val) || !isnan(mem_val)) {
                    printf("Error with NaN value\n");
                    errors++;
                }
            } else {
                if (src_val != dst_val || src_val != mem_val) {
                    printf("Error with special value: %f\n", src_val);
                    errors++;
                }
            }
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VMOVSD tests passed successfully!\n");
    } else {
        printf("VMOVSD tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vmovsd();
    return 0;
}
