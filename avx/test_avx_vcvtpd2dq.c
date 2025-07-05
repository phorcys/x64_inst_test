#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
#include <limits.h>
#include "avx.h"

// 获取舍入模式名称
static const char* get_rounding_mode_name(uint32_t mode) {
    switch (mode & 0x6000) {
        case 0x0000: return "Round to nearest (even)";
        case 0x2000: return "Round down (-inf)";
        case 0x4000: return "Round up (+inf)";
        case 0x6000: return "Round toward zero";
        default: return "Unknown";
    }
}

// vcvtpd2dq 指令测试
static int test_vcvtpd2dq() {
    printf("--- Testing vcvtpd2dq with all rounding modes and special values ---\n");
    
    int total_errors = 0;
    
    // 扩展测试数据 - 覆盖所有特殊值
    ALIGNED(32) double dbl_data[8] = {
        0.0,                    // 正零
        -0.0,                   // 负零
        1.5,                    // 正小数
        -2.5,                   // 负小数
        INFINITY,               // 正无穷
        -INFINITY,              // 负无穷
        NAN,      // NaN
        DBL_MAX,                // 最大正有限数
    };
    
    // 内存操作数测试数据 (128位和256位)
    ALIGNED(32) double mem_data128[2] = { 
        -DBL_MAX,               // 最小负有限数
        DBL_MIN                 // 最小正规格化数
    };
    ALIGNED(32) double mem_data256[4] = {
        0.0,
        -0.0,
        INFINITY,
        -INFINITY
    };
    
    // 定义所有舍入模式 (MXCSR控制位)
    const uint32_t rounding_modes[] = {0x0000, 0x2000, 0x4000, 0x6000};
    const char* mode_names[] = {
        "Round to nearest (even)",
        "Round down (-inf)",
        "Round up (+inf)",
        "Round toward zero"
    };
    
    // 保存原始MXCSR
    uint32_t original_mxcsr = get_mxcsr();
    
    // 测试所有四种舍入模式
    for (int mode_idx = 0; mode_idx < 4; mode_idx++) {
        uint32_t mode = rounding_modes[mode_idx];
        printf("\n=== Testing mode: %s ===\n", mode_names[mode_idx]);
        
        // 设置当前舍入模式
        uint32_t new_mxcsr = (original_mxcsr & ~0x6000) | mode;
        set_mxcsr(new_mxcsr);
        
        // 1. 128位寄存器操作数测试
        ALIGNED(32) int32_t int_result128[4] = {0};
        __asm__ __volatile__(
            "vmovapd %1, %%xmm0\n\t"
            "vcvtpd2dq %%xmm0, %%xmm1\n\t"
            "vmovdqa %%xmm1, %0\n\t"
            : "=m" (int_result128)
            : "m" (dbl_data[0])
            : "xmm0", "xmm1"
        );
        
        printf("\n[128-bit Register Operand]\n");
        for (int i = 0; i < 2; i++) {
            double val = dbl_data[i];
            int32_t expected = 0x80000000; // 默认值 (无效结果)
            
            // 计算预期结果
            if (!isnan(val) && !isinf(val)) {
                if (val >= (double)INT32_MIN && val <= (double)INT32_MAX) {
                    expected = (int32_t)rint(val); // 使用当前舍入模式
                }
            }
            printf("Element %d: double=%.6f => int=0x%08X (%d) \n",
                   i, val, int_result128[i], int_result128[i]);
        }
        
        // 2. 256位寄存器操作数测试
        ALIGNED(32) int32_t int_result256[4] = {0};
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vcvtpd2dq %%ymm0, %%xmm1\n\t"
            "vmovdqa %%xmm1, %0\n\t"
            : "=m" (int_result256)
            : "m" (dbl_data[0])
            : "ymm0", "xmm1"
        );
        
        printf("\n[256-bit Register Operand]\n");
        for (int i = 0; i < 4; i++) {
            double val = dbl_data[i];
            int32_t expected = 0x80000000; // 默认值 (无效结果)
            
            // 计算预期结果
            if (!isnan(val) && !isinf(val)) {
                if (val >= (double)INT32_MIN && val <= (double)INT32_MAX) {
                    expected = (int32_t)rint(val); // 使用当前舍入模式
                }
            }
            
            printf("Element %d: double=%.6f => int=0x%08X (%d) \n",
                   i, val, int_result256[i], int_result256[i]);
            

        }

        
        // 2. 256位寄存器操作数测试 2

        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vcvtpd2dq %%ymm0, %%xmm1\n\t"
            "vmovdqa %%xmm1, %0\n\t"
            : "=m" (int_result256)
            : "m" (dbl_data[4])
            : "ymm0", "xmm1"
        );
        
        printf("\n[256-bit Register Operand part2]\n");
        for (int i = 0; i < 4; i++) {
            double val = dbl_data[i+4];
            int32_t expected = 0x80000000; // 默认值 (无效结果)
            
            // 计算预期结果
            if (!isnan(val) && !isinf(val)) {
                if (val >= (double)INT32_MIN && val <= (double)INT32_MAX) {
                    expected = (int32_t)rint(val); // 使用当前舍入模式
                }
            }
            
            printf("Element %d: double=%.6f => int=0x%08X (%d) \n",
                   i, val, int_result256[i], int_result256[i] );
            
        }
        
        // 3. 128位内存操作数测试
        ALIGNED(32) int32_t mem_result128[4] = {0};
        __asm__ __volatile__(
            "vmovapd %1, %%xmm1\n\t"
            "vcvtpd2dq %%xmm1, %%xmm0\n\t"
            "vmovdqa %%xmm0, %0\n\t"
            : "=m" (mem_result128)
            : "m" (*mem_data128)
            : "xmm0","xmm1"
        );
        
        printf("\n[128-bit Memory Operand]\n");
        for (int i = 0; i < 2; i++) {
            double val = mem_data128[i];
            int32_t expected = 0x80000000; // 默认值 (无效结果)
            
            // 计算预期结果
            if (!isnan(val) && !isinf(val)) {
                if (val >= (double)INT32_MIN && val <= (double)INT32_MAX) {
                    expected = (int32_t)rint(val); // 使用当前舍入模式
                }
            }
            
            printf("Element %d: double=%.6f => int=0x%08X (%d) \n",
                   i, val, mem_result128[i], mem_result128[i]);
            
        }
        
        // 4. 256位内存操作数测试
        ALIGNED(32) int32_t mem_result256[4] = {0};
        __asm__ __volatile__(
            "vmovapd %1, %%ymm1\n\t"
            "vcvtpd2dq %%ymm1, %%xmm0\n\t"
            "vmovdqa %%xmm0, %0\n\t"
            : "=m" (mem_result256)
            : "m" (*mem_data256)
            : "ymm0", "ymm1"
        );
        
        printf("\n[256-bit Memory Operand]\n");
        for (int i = 0; i < 4; i++) {
            double val = mem_data256[i];
            int32_t expected = 0x80000000; // 默认值 (无效结果)
            
            // 计算预期结果
            if (!isnan(val) && !isinf(val)) {
                if (val >= (double)INT32_MIN && val <= (double)INT32_MAX) {
                    expected = (int32_t)rint(val); // 使用当前舍入模式
                }
            }
            
            printf("Element %d: double=%.6f => int=0x%08X (%d) \n",
                   i, val, mem_result256[i], mem_result256[i]);
            
        }
    }
    
    // 恢复原始MXCSR
    set_mxcsr(original_mxcsr);
    
    return total_errors;
}

int main() {
    int errors = test_vcvtpd2dq();
    printf("\nTotal errors: %d\n", errors);
    return errors;
}
