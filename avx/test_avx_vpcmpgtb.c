#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpgtb 指令测试
static void test_vpcmpgtb() {
    printf("--- Testing vpcmpgtb ---\n");
    
    // 测试数据（有符号字节）
    ALIGNED(32) int8_t src1[32] = {
        0, 127, -128, -1, 
        10, -10, 50, -50,
        0x7F, 0x80, 0x00, 0xFF,
        1, 2, 3, 4,
        100, -100, 0, -1,
        127, -128, 64, -64,
        32, -32, 16, -16,
        8, -8, 4, -4
    };
    
    ALIGNED(32) int8_t src2[32] = {
        0, 126, -127, 0, 
        10, -11, 49, -49,
        0x7F, 0x7F, 0x01, 0xFE,
        0, 1, 2, 3,
        99, -101, -1, -2,
        126, -127, 63, -65,
        31, -33, 15, -17,
        7, -9, 3, -5
    };
    
    ALIGNED(32) int8_t dst[32] = {0};
    
    // 预期结果
    int8_t expected[32] = {
        0,    // 0 == 0 -> 不成立
        0xFF, // 127 > 126 -> 成立
        0,    // -128 < -127 -> 不成立
        0,    // -1 < 0 -> 不成立
        0,    // 10 == 10 -> 不成立
        0xFF, // -10 > -11 -> 成立
        0xFF, // 50 > 49 -> 成立
        0,    // -50 < -49 -> 不成立
        0,    // 127 == 127 -> 不成立
        0,    // -128 < 127 -> 不成立
        0,    // 0 < 1 -> 不成立
        0xFF, // -1 > -2 -> 成立
        0xFF, // 1 > 0 -> 成立
        0xFF, // 2 > 1 -> 成立
        0xFF, // 3 > 2 -> 成立
        0xFF, // 4 > 3 -> 成立
        0xFF, // 100 > 99 -> 成立
        0xFF, // -100 > -101 -> 成立
        0xFF, // 0 > -1 -> 成立
        0xFF, // -1 > -2 -> 成立
        0xFF, // 127 > 126 -> 成立
        0,    // -128 < -127 -> 不成立
        0xFF, // 64 > 63 -> 成立
        0xFF, // -64 > -65 -> 成立
        0xFF, // 32 > 31 -> 成立
        0xFF, // -32 > -33 -> 成立
        0xFF, // 16 > 15 -> 成立
        0xFF, // -16 > -17 -> 成立
        0xFF, // 8 > 7 -> 成立
        0xFF, // -8 > -9 -> 成立
        0xFF, // 4 > 3 -> 成立
        0xFF  // -4 > -5 -> 成立
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpgtb %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 32; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%02X, expected 0x%02X\n", i, (uint8_t)dst[i], (uint8_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtb (256-bit) test passed!\n");
    } else {
        printf("vpcmpgtb (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) int8_t dst128[16] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpgtb %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 16; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%02X, expected 0x%02X\n", i, (uint8_t)dst128[i], (uint8_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtb (128-bit) test passed!\n");
    } else {
        printf("vpcmpgtb (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpgtb();
    return 0;
}
