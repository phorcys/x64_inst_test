#include "avx.h"
#include "avxfloat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static int run_test_case(const char *desc, int width, int op_type, 
                         const double *src1, const double *src2, 
                         int count) {
    printf("--- %s ---\n", desc);
    
    int align = (width == 256) ? 32 : 16;
    double *aligned_src1 = (double*)aligned_alloc(align, count * sizeof(double));
    double *aligned_src2 = (double*)aligned_alloc(align, count * sizeof(double));
    double *result = (double*)aligned_alloc(align, count * sizeof(double));
    
    if (!aligned_src1 || !aligned_src2 || !result) {
        perror("Memory allocation failed");
        free(aligned_src1);
        free(aligned_src2);
        free(result);
        return 0;
    }
    
    memcpy(aligned_src1, src1, count * sizeof(double));
    memcpy(aligned_src2, src2, count * sizeof(double));
    memset(result, 0, count * sizeof(double));
    
    if (width == 128) {
        if (op_type == 0) { // reg-reg
            __asm__ __volatile__(
                "vmovapd %1, %%xmm0\n\t"
                "vmovapd %2, %%xmm1\n\t"
                "vaddpd %%xmm1, %%xmm0, %%xmm2\n\t"
                "vmovapd %%xmm2, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "xmm0", "xmm1", "xmm2"
            );
        } else { // reg-mem
            __asm__ __volatile__(
                "vmovapd %1, %%xmm0\n\t"
                "vaddpd %2, %%xmm0, %%xmm1\n\t"
                "vmovapd %%xmm1, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "xmm0", "xmm1"
            );
        }
    } else { // 256-bit
        if (op_type == 0) { // reg-reg
            __asm__ __volatile__(
                "vmovapd %1, %%ymm0\n\t"
                "vmovapd %2, %%ymm1\n\t"
                "vaddpd %%ymm1, %%ymm0, %%ymm2\n\t"
                "vmovapd %%ymm2, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "ymm0", "ymm1", "ymm2"
            );
        } else { // reg-mem
            __asm__ __volatile__(
                "vmovapd %1, %%ymm0\n\t"
                "vaddpd %2, %%ymm0, %%ymm1\n\t"
                "vmovapd %%ymm1, %0\n\t"
                : "=m"(*result)
                : "m"(*aligned_src1), "m"(*aligned_src2)
                : "ymm0", "ymm1"
            );
        }
    }
    
    print_double_vec("Input1", aligned_src1, count);
    print_double_vec("Input2", aligned_src2, count);
    print_double_vec("Result", result, count);
    print_double_vec_hex("Hex   ", result, count);
    
    free(aligned_src1);
    free(aligned_src2);
    free(result);
    printf("--- End of test ---\n\n");
    
    return 0;
}

static void test_vaddpd() {
    printf("--- Testing vaddpd (packed double-precision addition) ---\n");
    
    
    // 测试128位用例
    int num_128 = sizeof(double_tests_128) / sizeof(double_tests_128[0]);
    for (int i = 0; i < num_128; i++) {
        DoubleTest128 *test = &double_tests_128[i];
        run_test_case(test->name, 128, 0, 
                                     test->input1, test->input2, 
                                     2);
               
        run_test_case(test->name, 128, 1, 
                                     test->input1, test->input2, 
                                     2);
        
    }
    
    // 测试256位用例
    int num_256 = sizeof(double_tests_256) / sizeof(double_tests_256[0]);
    for (int i = 0; i < num_256; i++) {
        DoubleTest256 *test = &double_tests_256[i];
        run_test_case(test->name, 256, 0, 
                                     test->input1, test->input2, 
                                     4);
        
        
        run_test_case(test->name, 256, 1, 
                                     test->input1, test->input2, 
                                     4);
        
    }
    

    return;
}

int main() {
    test_vaddpd();
    return 0 ;
}
