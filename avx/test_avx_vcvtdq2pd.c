#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "avx.h"

// 测试VCVTDQ2PD指令：将32位整数转换为双精度浮点
int main() {
    printf("Starting VCVTDQ2PD test...\n");
    
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
    printf("\n[Testing VCVTDQ2PD (128-bit)]\n");
    __m128i int_vec128;
    __m128d dbl_vec128;
    double dbl_result128[2];
    
    // 加载整数数据到128位向量
    memcpy(&int_vec128, int_data, sizeof(__m128i));
    
    // 提取低64位作为源操作数
    int64_t src64;
    _mm_storel_epi64((__m128i*)&src64, int_vec128);
    
    // 执行指令（128位版本）
    asm volatile (
        "vcvtdq2pd %1, %0"
        : "=x" (dbl_vec128)
        : "m" (src64)
        : 
    );
    
    // 存储结果
    _mm_storeu_pd(dbl_result128, dbl_vec128);
    
    // 打印并验证结果
    for (int i = 0; i < 2; i++) {
        double expected = (double)int_data[i];
        printf("Element %d: int=0x%08X (%d) => double=%.6f (expected %.6f)",
               i, int_data[i], int_data[i], dbl_result128[i], expected);
        
        if (double_equal(dbl_result128[i], expected, 1e-9)) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
        }
    }
    
    // 256位版本测试
    printf("\n[Testing VCVTDQ2PD (256-bit)]\n");
    __m256i int_vec256;
    __m256d dbl_vec256;
    double dbl_result256[4];
    
    // 加载整数数据到256位向量
    memcpy(&int_vec256, int_data, sizeof(__m256i));
    
    // 提取低128位作为源操作数
    __m128i src128 = _mm256_castsi256_si128(int_vec256);
    
    // 执行指令（256位版本）
    asm volatile (
        "vcvtdq2pd %1, %0"
        : "=x" (dbl_vec256)
        : "x" (src128)
        : 
    );
    
    // 存储结果
    _mm256_storeu_pd(dbl_result256, dbl_vec256);
    
    // 打印并验证结果
    for (int i = 0; i < 4; i++) {
        double expected = (double)int_data[i];
        printf("Element %d: int=0x%08X (%d) => double=%.6f (expected %.6f)",
               i, int_data[i], int_data[i], dbl_result256[i], expected);
        
        if (double_equal(dbl_result256[i], expected, 1e-9)) {
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
    
    printf("\nVCVTDQ2PD test completed.\n");
    return 0;
}
