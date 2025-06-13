#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>  // 添加FLT_MIN/FLT_MAX定义
#include "avx.h"

// 测试VMOVSS指令
void test_vmovss() {
    int errors = 0;
    printf("Testing VMOVSS instruction...\n");

    // 测试128位操作数
    {
        float src_val = 3.14159f;
        __m128 src = _mm_set_ss(src_val);
        __m128 dst = _mm_setzero_ps();
        float mem_val;

        // 寄存器到寄存器传输 (正确语法: vmovss dst, src1, src2)
        asm volatile("vmovss %1, %1, %0" : "=x"(dst) : "x"(src), "x"(src));
        
        float dst_val = _mm_cvtss_f32(dst);
        if (src_val != dst_val) {
            printf("[128-bit] Reg-to-reg error\n");
            printf("  Expected: %f\n", src_val);
            printf("  Got:      %f\n", dst_val);
            errors++;
        }

        // 内存到寄存器传输
        asm volatile("vmovss %1, %0" : "=x"(dst) : "m" (src_val));
        dst_val = _mm_cvtss_f32(dst);
        if (src_val != dst_val) {
            printf("[128-bit] Mem-to-reg error\n");
            printf("  Expected: %f\n", src_val);
            printf("  Got:      %f\n", dst_val);
            errors++;
        }

        // 寄存器到内存传输
        asm volatile("vmovss %1, %0" : "=m" (mem_val) : "x"(src));
        if (src_val != mem_val) {
            printf("[128-bit] Reg-to-mem error\n");
            printf("  Expected: %f\n", src_val);
            printf("  Got:      %f\n", mem_val);
            errors++;
        }
    }

    // 测试寄存器到寄存器传输（使用不同寄存器）
    {
        float src_val = -42.42f;
        __m128 src = _mm_set_ss(src_val);
        __m128 dst = _mm_set_ss(0.0f); // 初始化为非src_val的值

        // 使用内联汇编实现VMOVSS (寄存器到寄存器)
        asm volatile("vmovss %1, %1, %0" : "=x"(dst) : "x"(src), "x"(src));
        
        float dst_val = _mm_cvtss_f32(dst);
        if (src_val != dst_val) {
            printf("[Reg-to-reg with different registers] Error\n");
            printf("  Expected: %f\n", src_val);
            printf("  Got:      %f\n", dst_val);
            errors++;
        }
    }

    // 测试特殊值
    {
        float test_values[] = {
            0.0f, -0.0f, 
            INFINITY, -INFINITY, 
            NAN, -NAN,
            FLT_MIN, FLT_MAX
        };
        int num_tests = sizeof(test_values) / sizeof(test_values[0]);
        
        for (int i = 0; i < num_tests; i++) {
            float src_val = test_values[i];
            __m128 src128 = _mm_set_ss(src_val);
            __m128 dst128;
            float mem_val;

            // 输出MXCSR状态
            uint32_t mxcsr_before = get_mxcsr();
            // 已取消注释print_mxcsr调用
            print_mxcsr(mxcsr_before);

            // 128位寄存器传输
            asm volatile("vmovss %1, %1, %0" : "=x"(dst128) : "x"(src128), "x"(src128));
            float dst_val128 = _mm_cvtss_f32(dst128);
            
            // 不再测试256位寄存器传输，因为VMOVSS只操作128位寄存器
            
            // 寄存器到内存传输
            asm volatile("vmovss %1, %0" : "=m" (mem_val) : "x"(src128));
            
            // 特殊值比较
            if (isnan(src_val)) {
                if (!isnan(dst_val128) || !isnan(mem_val)) {
                    printf("[Special] NaN value error\n");
                    errors++;
                }
            } else if (isinf(src_val)) {
                if (!isinf(dst_val128) || !isinf(mem_val) ||
                    signbit(src_val) != signbit(dst_val128) ||
                    signbit(src_val) != signbit(mem_val)) {
                    printf("[Special] INF value error\n");
                    errors++;
                }
            } else {
                float tolerance = 1e-6;
                if (!float_equal(src_val, dst_val128, tolerance) ||
                    !float_equal(src_val, mem_val, tolerance)) {
                    printf("[Special] Value mismatch: %f\n", src_val);
                    errors++;
                }
            }

            // 输出MXCSR状态变化
            uint32_t mxcsr_after = get_mxcsr();
            if (mxcsr_before != mxcsr_after) {
                printf("MXCSR changed: 0x%08X -> 0x%08X\n", mxcsr_before, mxcsr_after);
            }
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VMOVSS tests passed successfully!\n");
    } else {
        printf("VMOVSS tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vmovss();
    return 0;
}
