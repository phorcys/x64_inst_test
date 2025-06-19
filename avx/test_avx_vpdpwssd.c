#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// VPDPWSSD测试函数
void test_vpdpwssd() {
    printf("=== Testing VPDPWSSD ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        int16_t src1[8] = {0x7FFF, -0x8000, 0x1234, -0x5678, 
                           0x1111, 0x2222, 0x3333, 0x4444};
        int16_t src2[8] = {0x7FFF, -0x8000, 0x5678, -0x1234,
                           -0x1111, -0x2222, -0x3333, -0x4444};
        int32_t dst[4] = {0};

        // 预期结果计算
        int32_t expected[4] = {0};
        for (int i = 0; i < 4; i++) {
            expected[i] = (int32_t)src1[i*2] * (int32_t)src2[i*2] + 
                          (int32_t)src1[i*2+1] * (int32_t)src2[i*2+1] + 
                          dst[i];
        }

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vmovdqu %3, %%xmm2\n\t"
            "vpdpwssd %%xmm1, %%xmm0, %%xmm2\n\t"
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

    printf("\n=== VPDPWSSD test completed ===\n");
}

int main() {
    test_vpdpwssd();
    return 0;
}
