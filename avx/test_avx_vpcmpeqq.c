#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpeqq 指令测试
static void test_vpcmpeqq() {
    printf("--- Testing vpcmpeqq ---\n");
    
    // 测试数据
    ALIGNED(32) uint64_t src1[4] = {
        0x0000000000000000, 
        0xFFFFFFFFFFFFFFFF, 
        0x5555555555555555, 
        0xAAAAAAAAAAAAAAAA
    };
    
    ALIGNED(32) uint64_t src2[4] = {
        0x0000000000000000, 
        0xFFFFFFFFFFFFFFFF, 
        0x5555555555555555, 
        0xAAAAAAAAAAAAAAAB
    };
    
    ALIGNED(32) uint64_t dst[4] = {0};
    
    // 预期结果
    uint64_t expected[4] = {
        0xFFFFFFFFFFFFFFFF,  // 相等
        0xFFFFFFFFFFFFFFFF,  // 相等
        0xFFFFFFFFFFFFFFFF,  // 相等
        0x0000000000000000   // 不相等
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpeqq %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 4; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%016lX, expected 0x%016lX\n", i, dst[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqq (256-bit) test passed!\n");
    } else {
        printf("vpcmpeqq (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) uint64_t dst128[2] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpeqq %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 2; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%016lX, expected 0x%016lX\n", i, dst128[i], expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpeqq (128-bit) test passed!\n");
    } else {
        printf("vpcmpeqq (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpeqq();
    return 0;
}
