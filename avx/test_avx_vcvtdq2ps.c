#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "avx.h"

// vcvtdq2ps 指令测试
static int test_vcvtdq2ps() {
    printf("--- Testing vcvtdq2ps ---\n");
    
    int total_errors = 0;
    
    // 测试数据 - 各种边界值
    ALIGNED(32) int32_t int_data[8] = {
        0,                      // 零
        1,                      // 最小正整数
        -1,                     // 最大负整数
        INT_MAX,                // 最大32位整数
        INT_MIN,                // 最小32位整数
        123456789,              // 一般正整数
        -987654321,             // 一般负整数
        0x7FFFFFFF              // 最大正数（同INT_MAX）
    };
    
    // 内存操作数测试
    ALIGNED(32) int32_t mem_data[4] = {100, -200, 300, -400};
    
    ALIGNED(32) float flt_result128[4] = {0};
    ALIGNED(32) float flt_result256[8] = {0};
    
    // 128位版本测试
    uint32_t mxcsr_after_128 = 0;
    __asm__ __volatile__(
        "vmovdqa %2, %%xmm0\n\t"        // 加载整数数据到xmm0
        "vcvtdq2ps %%xmm0, %%xmm1\n\t"  // 转换为单精度浮点数
        "vmovups %%xmm1, %0\n\t"        // 存储结果
        "stmxcsr %1\n\t"                // 保存MXCSR状态
        : "=m" (flt_result128), "=m"(mxcsr_after_128)
        : "m" (int_data)
        : "xmm0", "xmm1"
    );
    
    printf("\n[Testing VCVTDQ2PS (128-bit)]\n");
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
    printf("--- MXCSR State After 128-bit Operation ---\n");
    print_mxcsr(mxcsr_after_128);
    printf("\n");
    
    // 256位版本测试
    uint32_t mxcsr_after_256 = 0;
    __asm__ __volatile__(
        "vmovdqa %2, %%ymm0\n\t"        // 加载整数数据到ymm0
        "vcvtdq2ps %%ymm0, %%ymm1\n\t"  // 转换为单精度浮点数
        "vmovups %%ymm1, %0\n\t"        // 存储结果
        "stmxcsr %1\n\t"                // 保存MXCSR状态
        : "=m" (flt_result256), "=m"(mxcsr_after_256)
        : "m" (int_data)
        : "ymm0", "ymm1"
    );
    
    printf("\n[Testing VCVTDQ2PS (256-bit)]\n");
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
    printf("--- MXCSR State After 256-bit Operation ---\n");
    print_mxcsr(mxcsr_after_256);
    printf("\n");
    
    // // // 测试MXCSR寄存器状态
    // // uint32_t initial_mxcsr = get_mxcsr();
    // // printf("\nInitial MXCSR: 0x%08X\n", initial_mxcsr);
    
    // // // 执行转换后检查MXCSR
    // // uint32_t final_mxcsr = get_mxcsr();
    // // printf("Final MXCSR: 0x%08X\n", final_mxcsr);
    
    // if (initial_mxcsr != final_mxcsr) {
    //     printf("MXCSR changed during operation!\n");
    //     print_mxcsr(final_mxcsr);
    //     total_errors++;
    // }

    // 测试内存操作数
    ALIGNED(32) float mem_result[4] = {0};
    uint32_t mxcsr_after_mem = 0;
    __asm__ __volatile__(
        "vcvtdq2ps %2, %%xmm0\n\t"
        "vmovups %%xmm0, %0\n\t"
        "stmxcsr %1\n\t"                // 保存MXCSR状态
        : "=m" (mem_result), "=m"(mxcsr_after_mem)
        : "m" (*mem_data)
        : "xmm0"
    );

    printf("\n[Testing Memory Operand]\n");
    for (int i = 0; i < 4; i++) {
        float expected = (float)mem_data[i];
        printf("Element %d: int=%d => float=%.6f (expected %.6f)",
               i, mem_data[i], mem_result[i], expected);
        
        if (float_equal(mem_result[i], expected, 1e-6)) {
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
    int errors = test_vcvtdq2ps();
    printf("\nTotal errors: %d\n", errors);
    return errors;
}
