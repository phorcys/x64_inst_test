#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// VPMADDWD测试函数
void test_vpmaddwd() {
    printf("=== Testing VPMADDWD ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        int16_t src1[8] = {32767, -32768, 100, -200, 300, -400, 500, -600};
        int16_t src2[8] = {32767, -32768, 50, -100, 150, -200, 250, -300};
        int32_t dst[4] = {0};

        // 预期结果计算
        int32_t expected[4] = {
            (int32_t)32767 * 32767 + (int32_t)(-32768) * (-32768),
            (int32_t)100 * 50 + (int32_t)(-200) * (-100),
            (int32_t)300 * 150 + (int32_t)(-400) * (-200),
            (int32_t)500 * 250 + (int32_t)(-600) * (-300)
        };

        // 使用内联汇编执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmaddwd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印结果
        printf("Result: ");
        for(int i=0; i<4; i++) printf("%d ", dst[i]);
        printf("\nExpected: ");
        for(int i=0; i<4; i++) printf("%d ", expected[i]);
        printf("\n");

        // 验证结果
        for (int i = 0; i < 4; i++) {
            if (dst[i] != expected[i]) {
                printf("Mismatch at element %d: got %d, expected %d\n", 
                      i, dst[i], expected[i]);
            }
        }
    }

    printf("\n=== VPMADDWD test completed ===\n");
}

int main() {
    test_vpmaddwd();
    return 0;
}
