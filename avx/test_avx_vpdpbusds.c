#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// VPDPBUSDS测试函数
void test_vpdpbusds() {
    printf("=== Testing VPDPBUSDS ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        uint8_t src1[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0x01,
                            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        int8_t src2[16] = {0x7F, 0x7F, 0x7F, 0x7F, 0x80, 0x00, 0x7F, 0x01,
                           -0x11, -0x22, -0x33, -0x44, -0x55, -0x66, -0x77, -0x78};
        int32_t dst[4] = {0x7FFFFFFF, 0x80000000, 0, 0};

        // 预期结果计算（考虑饱和）
        int32_t expected[4] = {0x7FFFFFFF, 0x80000000, 0, 0};
        for (int i = 0; i < 4; i++) {
            int64_t sum = dst[i];
            for (int j = 0; j < 4; j++) {
                int idx = i*4 + j;
                sum += (int32_t)src1[idx] * (int32_t)src2[idx];
            }
            // 饱和处理
            if (sum > 0x7FFFFFFF) expected[i] = 0x7FFFFFFF;
            else if (sum < -0x80000000) expected[i] = 0x80000000;
            else expected[i] = sum;
        }

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vmovdqu %3, %%xmm2\n\t"
            "vpdpbusds %%xmm1, %%xmm0, %%xmm2\n\t"
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

    printf("\n=== VPDPBUSDS test completed ===\n");
}

int main() {
    test_vpdpbusds();
    return 0;
}
