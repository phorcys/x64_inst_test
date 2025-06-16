#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vpcmpgtq 指令测试
static void test_vpcmpgtq() {
    printf("--- Testing vpcmpgtq ---\n");
    
    // 测试数据（有符号64位整数）
    // 使用表达式避免编译器警告
    ALIGNED(32) int64_t src1[4] = {
        0,
        (-9223372036854775807LL - 1), // 等同于 -9223372036854775808
        9223372036854775807LL,
        -1
    };
    
    ALIGNED(32) int64_t src2[4] = {
        0,
        -9223372036854775807LL,
        9223372036854775806LL,
        0
    };
    
    ALIGNED(32) int64_t dst[4] = {0};
    
    // 预期结果
    int64_t expected[4] = {
        0,           // 0 == 0 -> 不成立
        0,           // -9223372036854775808 < -9223372036854775807 -> 不成立
        -1,          // 9223372036854775807 > 9223372036854775806 -> 成立
        0            // -1 < 0 -> 不成立
    };
    
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm0\n\t"
        "vmovdqa %2, %%ymm1\n\t"
        "vpcmpgtq %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovdqa %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 4; i++) {
        if (dst[i] != expected[i]) {
            printf("Error at element %d: got 0x%016lX, expected 0x%016lX\n", i, (uint64_t)dst[i], (uint64_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtq (256-bit) test passed!\n");
    } else {
        printf("vpcmpgtq (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) int64_t dst128[2] = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm0\n\t"
        "vmovdqa %2, %%xmm1\n\t"
        "vpcmpgtq %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovdqa %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 2; i++) {
        if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got 0x%016lX, expected 0x%016lX\n", i, (uint64_t)dst128[i], (uint64_t)expected[i]);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("vpcmpgtq (128-bit) test passed!\n");
    } else {
        printf("vpcmpgtq (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    test_vpcmpgtq();
    return 0;
}
