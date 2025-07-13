#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cpuid.h>

// 饱和减法辅助函数
static int16_t sat_sub(int16_t a, int16_t b) {
    int32_t res = (int32_t)a - (int32_t)b;
    if (res > 0x7FFF) return 0x7FFF;
    if (res < -0x8000) return -0x8000;
    return (int16_t)res;
}

// VPHSUBSW测试函数
void test_vphsubsw() {
    printf("=== Testing VPHSUBSW ===\n");

    // 测试128位操作
    {
       
        
        // 初始化测试数据
        int16_t src1[16] = {0x7FFF, -0x8000, 0x7FFF, -0x8000,
                           0x1111, 0x2222, 0x3333, 0x4444,
                        0x7FFF, -0x8000, 0x7FFF, -0x8000,
                           0x1111, 0x2222, 0x3333, 0x4444};
        int16_t src2[16] = {0x7FFF, -0x8000, -0x7FFF, 0x8000,
                          -0x1111, -0x2222, -0x3333, -0x4444,
                        0x7FFF, -0x8000, -0x7FFF, 0x8000,
                          -0x1111, -0x2222, -0x3333, -0x4444};
        int16_t dst[16] = {0};

        // 预期结果计算（考虑饱和）
        int16_t expected[16] = {
            sat_sub(src1[0], src1[1]),
            sat_sub(src1[2], src1[3]),
            sat_sub(src1[4], src1[5]),
            sat_sub(src1[6], src1[7]),
            sat_sub(src2[0], src2[1]),
            sat_sub(src2[2], src2[3]),
            sat_sub(src2[4], src2[5]),
            sat_sub(src2[6], src2[7]),
            sat_sub(src1[8], src1[9]),
            sat_sub(src1[10], src1[11]),
            sat_sub(src1[12], src1[13]),
            sat_sub(src1[14], src1[15]),
            sat_sub(src2[8], src2[9]),
            sat_sub(src2[10], src2[11]),
            sat_sub(src2[12], src2[13]),
            sat_sub(src2[14], src2[15])
        };

         printf("-- Testing 128-bit operation --\n");
        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vphsubsw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印结果
        print_xmm("Result", *(__m128i*)dst);
        print_xmm("Expect", *(__m128i*)expected);
        printf("\n");

         printf("-- Testing 256-bit operation --\n");
        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vphsubsw %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印结果
        print_ymm("Result", *(__m256i*)dst);
        print_ymm("Expect", *(__m256i*)expected);
        printf("\n");

    }

    printf("\n=== VPHSUBSW test completed ===\n");
}

int main() {
    test_vphsubsw();
    return 0;
}
