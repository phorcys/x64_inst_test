#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpgtd 指令测试
static void test_vpcmpgtd() {
    printf("--- Testing vpcmpgtd ---\n");
    
    // 测试数据（有符号32位整数）
    ALIGNED(32) int32_t src1[8] = {
        0,
        -2147483648,
        2147483647,
        -1,
        100,
        -100,
        0x7FFFFFFF,
        0x80000000
    };
    
    ALIGNED(32) int32_t src2[8] = {
        0,
        -2147483647,
        2147483646,
        0,
        99,
        -101,
        0x7FFFFFFE,
        0x7FFFFFFF
    };
    
    ALIGNED(32) int32_t dst[8] = {0};
    
    // 预期结果
    int32_t expected[8] = {
        0,           // 0 == 0 -> 不成立
        0,           // -2147483648 < -2147483647 -> 不成立
        0xFFFFFFFF,  // 2147483647 > 2147483646 -> 成立
        0,           // -1 < 0 -> 不成立
        0xFFFFFFFF,  // 100 > 99 -> 成立
        0xFFFFFFFF,  // -100 > -101 -> 成立
        0xFFFFFFFF,  // 0x7FFFFFFF > 0x7FFFFFFE -> 成立
        0            // 0x80000000 < 0x7FFFFFFF -> 不成立
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpgtd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 8; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%08X, expected 0x%08X\n", i, (uint32_t)dst[i], (uint32_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtd (256-bit) test passed!\n");
    } else {
        printf("vpcmpgtd (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) int32_t dst128[4] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpgtd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 4; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%08X, expected 0x%08X\n", i, (uint32_t)dst128[i], (uint32_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtd (128-bit) test passed!\n");
    } else {
        printf("vpcmpgtd (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpgtd();
    return 0;
}
