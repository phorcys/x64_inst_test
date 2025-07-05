#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "avx.h"

// vcvtdq2pd 指令测试
static int test_vcvtdq2pd() {
    printf("--- Testing vcvtdq2pd ---\n");
    
    int total_errors = 0;
    
    // 测试数据 - 各种边界值
    ALIGNED(32) int32_t int_data[4] = {
        0,                      // 零
        1,                      // 最小正整数
        -1,                     // 最大负整数
        INT_MAX,                // 最大32位整数
    };
    ALIGNED(32) int32_t int_data_2[4] = {
        INT_MIN,                // 最小32位整数
        123456789,              // 一般正整数
        -987654321,             // 一般负整数
        0x7FFFFFFF,              // 最大正数（同INT_MAX）
    };
    
    // 内存操作数测试
    ALIGNED(32) int32_t mem_data[4] = {100, -200, 300, -400};
    
    ALIGNED(32) double dbl_result128[2] = {0};
    ALIGNED(32) double dbl_result256[4] = {0};
    
    // 128位版本测试
    uint32_t mxcsr_after_128 = 0;
    __asm__ __volatile__(
        "vmovdqa %2, %%xmm0\n\t"        // 加载整数数据到xmm0
        "vcvtdq2pd %%xmm0, %%xmm1\n\t"  // 转换为双精度浮点数
        "vmovupd %%xmm1, %0\n\t"        // 存储结果
        : "=m" (dbl_result128), "=m"(mxcsr_after_128)
        : "m" (int_data)
        : "xmm0", "xmm1"
    );
    
    printf("\n[Testing VCVTDQ2PD (128-bit)]\n");
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
    printf("--- MXCSR State After 128-bit Operation ---\n");
    print_mxcsr(mxcsr_after_128);
    printf("\n");
    
    // 256位版本测试
    uint32_t mxcsr_after_256 = 0;
    __asm__ __volatile__(
        "vmovdqa %2, %%xmm0\n\t"        // 加载整数数据到xmm0
        "vcvtdq2pd %%xmm0, %%ymm1\n\t"  // 转换为双精度浮点数
        "vmovupd %%ymm1, %0\n\t"        // 存储结果
        : "=m" (dbl_result256), "=m"(mxcsr_after_256)
        : "m" (int_data)
        : "xmm0", "ymm1"
    );
    
    printf("\n[Testing VCVTDQ2PD (256-bit)]\n");
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
    __asm__ __volatile__(
        "vmovdqa %2, %%xmm0\n\t"        // 加载整数数据到xmm0
        "vcvtdq2pd %%xmm0, %%ymm1\n\t"  // 转换为双精度浮点数
        "vmovupd %%ymm1, %0\n\t"        // 存储结果
        : "=m" (dbl_result256), "=m"(mxcsr_after_256)
        : "m" (int_data_2) // 使用后半部分数据
        : "xmm0", "ymm1"
    );
    
    printf("\n[Testing VCVTDQ2PD 2 (256-bit)]\n");
    for (int i = 0; i < 4; i++) {
        double expected = (double)int_data_2[i];
        printf("Element %d: int=0x%08X (%d) => double=%.6f (expected %.6f)",
               i, int_data_2[i], int_data_2[i], dbl_result256[i], expected);
        
        if (double_equal(dbl_result256[i], expected, 1e-9)) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
        }
    }

    // 测试内存操作数
    ALIGNED(32) double mem_result[4] = {0};
    uint32_t mxcsr_after_mem = 0;
    __asm__ __volatile__(
        "vcvtdq2pd %2, %%ymm0\n\t"
        "vmovupd %%ymm0, %0\n\t"
        "stmxcsr %1\n\t"                // 保存MXCSR状态
        : "=m" (mem_result), "=m"(mxcsr_after_mem)
        : "m" (*mem_data)
        : "ymm0"
    );

    printf("\n[Testing Memory Operand]\n");
    for (int i = 0; i < 4; i++) {
        double expected = (double)mem_data[i];
        printf("Element %d: int=%d => double=%.6f (expected %.6f)",
               i, mem_data[i], mem_result[i], expected);
        
        if (double_equal(mem_result[i], expected, 1e-9)) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
            total_errors++;
        }
    }
    printf("--- MXCSR State After Memory Operand Operation ---\n");
    print_mxcsr(mxcsr_after_mem);
    printf("\n");

    return total_errors;
}

int main() {
    int errors = test_vcvtdq2pd();
    printf("\nTotal errors: %d\n", errors);
    return errors;
}
