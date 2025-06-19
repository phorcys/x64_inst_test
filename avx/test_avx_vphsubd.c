#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// VPHSUBD测试函数
void test_vphsubd() {
    printf("=== Testing VPHSUBD ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        int32_t src1[4] = {0x7FFFFFFF, -0x80000000, 0x12345678, -0x56789ABC};
        int32_t src2[4] = {0x7FFFFFFF, -0x80000000, -0x12345678, 0x56789ABC};
        int32_t dst[4] = {0};

        // 预期结果计算
        int32_t expected[4] = {
            (int32_t)(src1[0] - src1[1]),
            (int32_t)(src1[2] - src1[3]),
            (int32_t)(src2[0] - src2[1]),
            (int32_t)(src2[2] - src2[3])
        };

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vphsubd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印结果
        print_xmm("Result", *(__m128i*)dst);
        printf("Expected: ");
        for(int i=0; i<4; i++) {
            printf("%d ", expected[i]);
        }
        printf("\n");

        // 验证结果
        for (int i = 0; i < 4; i++) {
            if (dst[i] != expected[i]) {
                printf("Mismatch at element %d: got %d, expected %d\n", 
                      i, dst[i], expected[i]);
            }
        }
    }

    printf("\n=== VPHSUBD test completed ===\n");
}

int main() {
    test_vphsubd();
    return 0;
}
