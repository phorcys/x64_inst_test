#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpeqw 指令测试
static void test_vpcmpeqw() {
    printf("--- Testing vpcmpeqw ---\n");
    
    // 测试数据
    ALIGNED(32) uint16_t src1[16] = {
        0x0000, 0xFFFF, 0x5555, 0xAAAA,
        0x0001, 0x8000, 0x7FFF, 0xFFFE,
        0x1234, 0x5678, 0x9ABC, 0xDEF0,
        0x1111, 0x2222, 0x3333, 0x4444
    };
    
    ALIGNED(32) uint16_t src2[16] = {
        0x0000, 0xFFFF, 0x5555, 0xAAAB,
        0x0001, 0x8000, 0x7FFE, 0xFFFE,
        0x1234, 0x5679, 0x9ABC, 0xDEF1,
        0x1111, 0x2223, 0x3333, 0x4445
    };
    
    ALIGNED(32) uint16_t dst[16] = {0};
    
    // 预期结果
    uint16_t expected[16] = {
        0xFFFF,  // 相等
        0xFFFF,  // 相等
        0xFFFF,  // 相等
        0x0000,  // 不相等
        0xFFFF,  // 相等
        0xFFFF,  // 相等
        0x0000,  // 不相等
        0xFFFF,  // 相等
        0xFFFF,  // 相等
        0x0000,  // 不相等
        0xFFFF,  // 相等
        0x0000,  // 不相等
        0xFFFF,  // 相等
        0x0000,  // 不相等
        0xFFFF,  // 相等
        0x0000   // 不相等
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpeqw %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 16; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%04X, expected 0x%04X\n", i, dst[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqw (256-bit) test passed!\n");
    } else {
        printf("vpcmpeqw (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) uint16_t dst128[8] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpeqw %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 8; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%04X, expected 0x%04X\n", i, dst128[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqw (128-bit) test passed!\n");
    } else {
        printf("vpcmpeqw (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpeqw();
    return 0;
}
