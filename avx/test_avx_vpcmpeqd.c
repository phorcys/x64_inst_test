#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpeqd 指令测试
static void test_vpcmpeqd() {
    printf("--- Testing vpcmpeqd ---\n");
    
    // 测试数据
    ALIGNED(32) uint32_t src1[8] = {
        0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA,
        0x00000001, 0x80000000, 0x7FFFFFFF, 0xFFFFFFFE
    };
    
    ALIGNED(32) uint32_t src2[8] = {
        0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAB,
        0x00000001, 0x80000000, 0x7FFFFFFE, 0xFFFFFFFE
    };
    
    ALIGNED(32) uint32_t dst[8] = {0};
    
    // 预期结果
    uint32_t expected[8] = {
        0xFFFFFFFF,  // 0x00000000 == 0x00000000
        0xFFFFFFFF,  // 0xFFFFFFFF == 0xFFFFFFFF
        0xFFFFFFFF,  // 0x55555555 == 0x55555555
        0x00000000,  // 0xAAAAAAAA != 0xAAAAAAAB
        0xFFFFFFFF,  // 0x00000001 == 0x00000001
        0xFFFFFFFF,  // 0x80000000 == 0x80000000
        0x00000000,  // 0x7FFFFFFF != 0x7FFFFFFE
        0xFFFFFFFF   // 0xFFFFFFFE == 0xFFFFFFFE
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpeqd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 8; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%08X, expected 0x%08X\n", i, dst[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqd (256-bit) test passed!\n");
    } else {
        printf("vpcmpeqd (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) uint32_t dst128[4] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpeqd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 4; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%08X, expected 0x%08X\n", i, dst128[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqd (128-bit) test passed!\n");
    } else {
        printf("vpcmpeqd (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpeqd();
    return 0;
}
