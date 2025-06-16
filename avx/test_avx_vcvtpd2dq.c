#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "avx.h"

// vcvtpd2dq 指令测试
static int test_vcvtpd2dq() {
    printf("--- Testing vcvtpd2dq ---\n");
    
    int total_errors = 0;
    
    // 测试数据 - 各种边界值
    ALIGNED(32) double dbl_data[4] = {
        0.0,                    // 零
        1.5,                    // 正小数
        -2.5,                   // 负小数
        __builtin_nan(""),      // NaN
    };
    
    // 内存操作数测试
    ALIGNED(32) double mem_data[2] = {100.25, -200.75};
    
    ALIGNED(32) int32_t int_result128[4] = {0};
    ALIGNED(32) int32_t int_result256[4] = {0};
    
    // 设置舍入模式为截断(向零舍入)
    uint32_t old_mxcsr = get_mxcsr();
    set_mxcsr((old_mxcsr & ~0x6000) | 0x6000);
    
    // 128位版本测试
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"        // 加载双精度数据到xmm0
        "vcvtpd2dq %%xmm0, %%xmm1\n\t"  // 转换为32位整数
        "vmovdqa %%xmm1, %0\n\t"        // 存储结果
        : "=m" (int_result128)
        : "m" (dbl_data)
        : "xmm0", "xmm1"
    );
    
    // 恢复原始MXCSR
    set_mxcsr(old_mxcsr);
    
    printf("\n[Testing VCVTPD2DQ (128-bit)]\n");
    for (int i = 0; i < 2; i++) {
        int32_t expected = (int32_t)dbl_data[i];
        printf("Element %d: double=%.6f => int=0x%08X (%d) (expected 0x%08X)",
               i, dbl_data[i], int_result128[i], int_result128[i], expected);
        
        if (int_result128[i] == expected) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
            total_errors++;
        }
    }
    
    // 设置舍入模式为截断(向零舍入)
    uint32_t old_mxcsr2 = get_mxcsr();
    set_mxcsr((old_mxcsr2 & ~0x6000) | 0x6000);
    
    // 256位版本测试
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"        // 加载双精度数据到ymm0
        "vcvtpd2dq %%ymm0, %%xmm1\n\t"  // 转换为32位整数
        "vmovdqa %%xmm1, %0\n\t"        // 存储结果
        : "=m" (int_result256)
        : "m" (dbl_data)
        : "ymm0", "xmm1"
    );
    
    // 恢复原始MXCSR
    set_mxcsr(old_mxcsr2);
    
    printf("\n[Testing VCVTPD2DQ (256-bit)]\n");
    for (int i = 0; i < 4; i++) {
        int32_t expected = (int32_t)dbl_data[i];
        printf("Element %d: double=%.6f => int=0x%08X (%d) (expected 0x%08X)",
               i, dbl_data[i], int_result256[i], int_result256[i], expected);
        
        if (int_result256[i] == expected) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
            total_errors++;
        }
    }
    
    // // 测试MXCSR寄存器状态
    // uint32_t initial_mxcsr = get_mxcsr();
    // printf("\nInitial MXCSR: 0x%08X\n", initial_mxcsr);
    
    // // 执行转换后检查MXCSR
    // uint32_t final_mxcsr = get_mxcsr();
    // printf("Final MXCSR: 0x%08X\n", final_mxcsr);
    
    // if (initial_mxcsr != final_mxcsr) {
    //     printf("MXCSR changed during operation!\n");
    //     print_mxcsr(final_mxcsr);
    //     total_errors++;
    // }

    // 设置舍入模式为截断(向零舍入)
    uint32_t old_mxcsr3 = get_mxcsr();
    set_mxcsr((old_mxcsr3 & ~0x6000) | 0x6000);
    
    // 测试内存操作数
    ALIGNED(32) int32_t mem_result[4] = {0};
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"        // 先加载内存数据到xmm0
        "vcvtpd2dq %%xmm0, %%xmm1\n\t"  // 再进行转换
        "vmovdqa %%xmm1, %0\n\t"        // 存储结果
        : "=m" (mem_result)
        : "m" (*mem_data)
        : "xmm0", "xmm1"
    );
    
    // 恢复原始MXCSR
    set_mxcsr(old_mxcsr3);

    printf("\n[Testing Memory Operand]\n");
    for (int i = 0; i < 2; i++) {
        int32_t expected = (int32_t)mem_data[i];
        printf("Element %d: double=%.6f => int=%d (expected %d)",
               i, mem_data[i], mem_result[i], expected);
        
        if (mem_result[i] == expected) {
            printf(" [PASS]\n");
        } else {
            printf(" [FAIL]\n");
            total_errors++;
        }
    }

    return total_errors;
}

int main() {
    int errors = test_vcvtpd2dq();
    printf("\nTotal errors: %d\n", errors);
    return errors;
}
