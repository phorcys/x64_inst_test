#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "avx.h"

// 测试VCVTDQ2PS指令：将32位整数转换为单精度浮点
int main() {
    printf("Starting VCVTDQ2PS test...\n");
    
    // 设置MXCSR寄存器为默认状态
    uint32_t initial_mxcsr = get_mxcsr();
    printf("Initial MXCSR: 0x%08X\n", initial_mxcsr);
    
    // 测试数据 - 各种边界值
    int32_t int_data[8] = {
        0,                      // 零
        1,                      // 最小正整数
        -1,                     // 最大负整数
        INT_MAX,                // 最大32位整数
        INT_MIN,                // 最小32位整数
        123456789,              // 一般正整数
        -987654321,             // 一般负整数
        0x7FFFFFFF              // 最大正数（同INT_MAX）
    };
    
    // 128位版本测试
    printf("\n[Testing VCVTDQ2PS (128-bit)]\n");
    __m128i int_vec128;
    __m128 flt_vec128;
    float flt_result128[4];
    
    // 加载整数数据到128位向量
    memcpy(&int_vec128, int_data, sizeof(__m128i));
    
    // 执行指令
    asm volatile (
        "vcvtdq2ps %1, %0"
        : "=x" (flt_vec128)
        : "x" (int_vec128)
    );
    
    // 存储结果
    _mm_storeu_ps(flt_result128, flt_vec128);
    
    // 打印并验证结果
    for (int i = 0; i < 4; i++) {
        float expected = (float)int_data[i];
        printf("Element %d: int=0x%08X (%d) => float=%.6f (expected %.6f)",
               i, int_data[i], int_data[i], flt_result128[i], expected);
        
        if (float_equal(flt_result128[i], expected, 1e-6)) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
        }
    }
    
    // 256位版本测试
    printf("\n[Testing VCVTDQ2PS (256-bit)]\n");
    __m256i int_vec256;
    __m256 flt_vec256;
    float flt_result256[8];
    
    // 加载整数数据到256位向量
    memcpy(&int_vec256, int_data, sizeof(__m256i));
    
    // 执行指令
    asm volatile (
        "vcvtdq2ps %1, %0"
        : "=x" (flt_vec256)
        : "x" (int_vec256)
    );
    
    // 存储结果
    _mm256_storeu_ps(flt_result256, flt_vec256);
    
    // 打印并验证结果
    for (int i = 0; i < 8; i++) {
        float expected = (float)int_data[i];
        printf("Element %d: int=0x%08X (%d) => float=%.6f (expected %.6f)",
               i, int_data[i], int_data[i], flt_result256[i], expected);
        
        if (float_equal(flt_result256[i], expected, 1e-6)) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
        }
    }
    
    // // 检查MXCSR状态变化
    // uint32_t final_mxcsr = get_mxcsr();
    // printf("\nFinal MXCSR: 0x%08X\n", final_mxcsr);
    // if (initial_mxcsr != final_mxcsr) {
    //     printf("MXCSR changed during operation!\n");
    //     print_mxcsr(final_mxcsr);
    // }
    
    printf("\nVCVTDQ2PS test completed.\n");
    return 0;
}
