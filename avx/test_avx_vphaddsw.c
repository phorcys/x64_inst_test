#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 饱和加法辅助函数
static int16_t sat_add(int16_t a, int16_t b) {
    int32_t sum = (int32_t)a + (int32_t)b;
    if (sum > 0x7FFF) return 0x7FFF;
    if (sum < -0x8000) return -0x8000;
    return (int16_t)sum;
}

// VPHADDSW测试函数
void test_vphaddsw() {
    printf("=== Testing VPHADDSW ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        int16_t src1[8] = {0x7FFF, -0x8000, 0x7FFF, -0x8000,
                           0x1111, 0x2222, 0x3333, 0x4444};
        int16_t src2[8] = {0x7FFF, -0x8000, -0x8000, 0x7FFF,
                           -0x1111, -0x2222, -0x3333, -0x4444};
        int16_t dst[8] = {0};

        // 预期结果计算（考虑饱和）
        int16_t expected[8] = {
            sat_add(src1[0], src1[1]),
            sat_add(src1[2], src1[3]),
            sat_add(src1[4], src1[5]),
            sat_add(src1[6], src1[7]),
            sat_add(src2[0], src2[1]),
            sat_add(src2[2], src2[3]),
            sat_add(src2[4], src2[5]),
            sat_add(src2[6], src2[7])
        };

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vphaddsw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印结果
        print_xmm("Result", *(__m128i*)dst);
        printf("Expected: ");
        for(int i=0; i<8; i++) {
            printf("%d ", expected[i]);
        }
        printf("\n");

        // 验证结果
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                printf("Mismatch at element %d: got %d, expected %d\n", 
                      i, dst[i], expected[i]);
            }
        }
    }

    printf("\n=== VPHADDSW test completed ===\n");
}

int main() {
    test_vphaddsw();
    return 0;
}
