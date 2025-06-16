#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpeqb 指令测试
static void test_vpcmpeqb() {
    printf("--- Testing vpcmpeqb ---\n");
    
    // 测试数据
    ALIGNED(32) uint8_t src1[32] = {
        0x00, 0xFF, 0x55, 0xAA, 0x01, 0x80, 0x7F, 0xFE,
        0x00, 0x00, 0xFF, 0xFF, 0x55, 0xAA, 0xAA, 0x55,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
    };
    
    ALIGNED(32) uint8_t src2[32] = {
        0x00, 0xFF, 0x55, 0xAB, 0x01, 0x80, 0x7E, 0xFE,
        0x00, 0x01, 0xFF, 0xFE, 0x55, 0xAA, 0x55, 0xAA,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01
    };
    
    ALIGNED(32) uint8_t dst[32] = {0};
    
    // 预期结果
    uint8_t expected[32] = {
        0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpeqb %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 32; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at byte %d: got 0x%02X, expected 0x%02X\n", i, dst[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqb (256-bit) test passed!\n");
    } else {
        printf("vpcmpeqb (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) uint8_t dst128[16] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpeqb %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 16; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at byte %d (128-bit): got 0x%02X, expected 0x%02X\n", i, dst128[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqb (128-bit) test passed!\n");
    } else {
        printf("vpcmpeqb (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpeqb();
    return 0;
}
