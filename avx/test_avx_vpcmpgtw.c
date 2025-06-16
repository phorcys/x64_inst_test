#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpgtw 指令测试
static void test_vpcmpgtw() {
    printf("--- Testing vpcmpgtw ---\n");
    
    // 测试数据（有符号16位整数）
    ALIGNED(32) int16_t src1[16] = {
        0,
        32767,
        -32768,
        -1,
        100,
        -100,
        0x7FFF,
        0x8000,
        50,
        -50,
        0,
        -1,
        200,
        -200,
        0x4000,
        -0x4000
    };
    
    ALIGNED(32) int16_t src2[16] = {
        0,
        32766,
        -32767,
        0,
        99,
        -101,
        0x7FFE,
        0x7FFF,
        49,
        -49,
        -1,
        -2,
        199,
        -201,
        0x3FFF,
        -0x4001
    };
    
    ALIGNED(32) int16_t dst[16] = {0};
    
    // 预期结果
    int16_t expected[16] = {
        0,       // 0 == 0 -> 不成立
        0xFFFF,  // 32767 > 32766 -> 成立
        0,       // -32768 < -32767 -> 不成立
        0,       // -1 < 0 -> 不成立
        0xFFFF,  // 100 > 99 -> 成立
        0xFFFF,  // -100 > -101 -> 成立
        0xFFFF,  // 0x7FFF > 0x7FFE -> 成立
        0,       // 0x8000 < 0x7FFF -> 不成立
        0xFFFF,  // 50 > 49 -> 成立
        0,       // -50 < -49 -> 不成立
        0xFFFF,  // 0 > -1 -> 成立
        0xFFFF,  // -1 > -2 -> 成立
        0xFFFF,  // 200 > 199 -> 成立
        0xFFFF,  // -200 > -201 -> 成立
        0xFFFF,  // 0x4000 > 0x3FFF -> 成立
        0xFFFF   // -0x4000 > -0x4001 -> 成立
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpgtw %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 16; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%04X, expected 0x%04X\n", i, (uint16_t)dst[i], (uint16_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtw (256-bit) test passed!\n");
    } else {
        printf("vpcmpgtw (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) int16_t dst128[8] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpgtw %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 8; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%04X, expected 0x%04X\n", i, (uint16_t)dst128[i], (uint16_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtw (128-bit) test passed!\n");
    } else {
        printf("vpcmpgtw (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpgtw();
    return 0;
}
