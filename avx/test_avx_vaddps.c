#include "avx.h"
#include "avxfloat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


static int run_test_case(const char *desc, int width, int op_type, 
                         const float *src1, const float *src2, 
                         int count) {
    printf("--- %s ---\n", desc);
    
    // 对齐分配源数据和结果内存
    int align = (width == 256) ? 32 : 16;
    float *aligned_src1 = (float*)aligned_alloc(align, count * sizeof(float));
    float *aligned_src2 = (float*)aligned_alloc(align, count * sizeof(float));
    float *result = (float*)aligned_alloc(align, count * sizeof(float));
    
    if (!aligned_src1 || !aligned_src2 || !result) {
        perror("Memory allocation failed");
        free(aligned_src1);
        free(aligned_src2);
        free(result);
        return 0;
    }
    
    // 复制源数据到对齐内存
    memcpy(aligned_src1, src1, count * sizeof(float));
    memcpy(aligned_src2, src2, count * sizeof(float));
    memset(result, 0, count * sizeof(float));
#ifdef __loongarch__
    if (width == 128) {
        if (op_type == 0) { // reg-reg
            __asm__ __volatile__(
                "vld $vr4, %1\n\t"
                "vld $vr5, %2\n\t"
                "vfadd.s $vr6, $vr4, $vr5\n\t"
                "vst $vr6, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : 
            );
        } else { // reg-mem
            __asm__ __volatile__(
                "vld $vr4, %1\n\t"
                "vld $vr5, %2\n\t"
                "vfadd.s $vr6, $vr4, $vr5\n\t"
                "vst $vr6, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : 
            );
        }
    } else { // 256-bit
        if (op_type == 0) { // reg-reg
            __asm__ __volatile__(
                "xvld $vr4, %1\n\t"
                "xvld $vr5, %2\n\t"
                "xvfadd.s $vr6, $vr4, $vr5\n\t"
                "xvst $vr6, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : 
            );
        } else { // reg-mem
            __asm__ __volatile__(
                "xvld $vr4, %1\n\t"
                "xvld $vr5, %2\n\t"
                "xvfadd.s $vr6, $vr4, $vr5\n\t"
                "xvst $vr6, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : 
            );
        }
    }
#else
    if (width == 128) {
        if (op_type == 0) { // reg-reg
            __asm__ __volatile__(
                "vmovaps %1, %%xmm0\n\t"
                "vmovaps %2, %%xmm1\n\t"
                "vaddps %%xmm1, %%xmm0, %%xmm2\n\t"
                "vmovaps %%xmm2, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "xmm0", "xmm1", "xmm2"
            );
        } else { // reg-mem
            __asm__ __volatile__(
                "vmovaps %1, %%xmm0\n\t"
                "vaddps %2, %%xmm0, %%xmm1\n\t"
                "vmovaps %%xmm1, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "xmm0", "xmm1"
            );
        }
    } else { // 256-bit
        if (op_type == 0) { // reg-reg
            __asm__ __volatile__(
                "vmovaps %1, %%ymm0\n\t"
                "vmovaps %2, %%ymm1\n\t"
                "vaddps %%ymm1, %%ymm0, %%ymm2\n\t"
                "vmovaps %%ymm2, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "ymm0", "ymm1", "ymm2"
            );
        } else { // reg-mem
            __asm__ __volatile__(
                "vmovaps %1, %%ymm0\n\t"
                "vaddps %2, %%ymm0, %%ymm1\n\t"
                "vmovaps %%ymm1, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "ymm0", "ymm1"
            );
        }
    }
#endif

    // 打印测试信息
    print_float_vec("Input1", aligned_src1, count);
    print_float_vec("Input2", aligned_src2, count);
    print_float_vec("Result ", result, count);
    print_hex_float_vec("Hex   ", result, count);
    
    // 释放内存
    free(aligned_src1);
    free(aligned_src2);
    free(result);
    printf("--- End of test ---\n\n");
    
    return 0;
}

static void  test_vaddps() {
    printf("--- Testing vaddps (packed single-precision addition) ---\n");
    
    // 测试128位用例
    int num_128 = sizeof(float_tests_128) / sizeof(float_tests_128[0]);
    for (int i = 0; i < num_128; i++) {
        FloatTest128 *test = &float_tests_128[i];
        // 测试寄存器-寄存器操作
        run_test_case(test->name, 128, 0, 
                                     test->input1, test->input2, 
                                     4);
       
        // 测试寄存器-内存操作
        run_test_case(test->name, 128, 1, 
                                     test->input1, test->input2, 
                                     4);
    }
    
    // 测试256位用例
    int num_256 = sizeof(float_tests_256) / sizeof(float_tests_256[0]);
    for (int i = 0; i < num_256; i++) {
        FloatTest256 *test = &float_tests_256[i];
        // 测试寄存器-寄存器操作
        run_test_case(test->name, 256, 0, 
                                     test->input1, test->input2, 
                                     8);
        
        // 测试寄存器-内存操作
        run_test_case(test->name, 256, 1, 
                                     test->input1, test->input2, 
                                     8);
    }
    
    return;
}

int main() {
    test_vaddps();
    return 0;
}
