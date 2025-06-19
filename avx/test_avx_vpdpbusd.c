#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// VPDPBUSD测试函数
void test_vpdpbusd() {
    printf("=== Testing VPDPBUSD ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        uint8_t src1[16] = {0x01, 0x02, 0x80, 0x7F, 0xFF, 0x00, 0x55, 0xAA,
                            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        int8_t src2[16] = {0x01, 0x02, 0x7F, 0x80, 0x7F, 0x00, 0x55, 0xAA,
                           -0x11, -0x22, -0x33, -0x44, -0x55, -0x66, -0x77, -0x78};
        int32_t dst[4] = {0};

        // 预期结果计算
        int32_t expected[4] = {0};
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int idx = i*4 + j;
                expected[i] += (int32_t)src1[idx] * (int32_t)src2[idx];
            }
            expected[i] += dst[i]; // 累加初始值
        }

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vmovdqu %3, %%xmm2\n\t"
            "vpdpbusd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2), "m" (dst)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印结果
        print_xmm("Result", *(__m128i*)dst);
        print_int32_vec("Expected", expected, 4);

        // 验证结果
        for (int i = 0; i < 4; i++) {
            if (dst[i] != expected[i]) {
                printf("Mismatch at element %d: got %d, expected %d\n", 
                      i, dst[i], expected[i]);
            }
        }
    }

    // 测试256位操作
    {
        printf("\n-- Testing 256-bit operation --\n");
        
        // 初始化测试数据
        uint8_t src1[32], src2[32];
        int32_t dst[8] = {0};

        // 填充测试数据
        for (int i = 0; i < 32; i++) {
            src1[i] = i + 1;
            src2[i] = (i % 2) ? -(i + 1) : (i + 1);
        }

        // 预期结果计算
        int32_t expected[8] = {0};
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 4; j++) {
                int idx = i*4 + j;
                expected[i] += (int32_t)src1[idx] * (int32_t)src2[idx];
            }
            expected[i] += dst[i]; // 累加初始值
        }

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vmovdqu %3, %%ymm2\n\t"
            "vpdpbusd %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2), "m" (dst)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印结果
        print_ymm("Result", *(__m256i*)dst);
        print_int32_vec("Expected", expected, 8);

        // 验证结果
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                printf("Mismatch at element %d: got %d, expected %d\n", 
                      i, dst[i], expected[i]);
            }
        }
    }

    printf("\n=== VPDPBUSD test completed ===\n");
}

int main() {
    test_vpdpbusd();
    return 0;
}
