#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vmaxps 指令测试
static void test_vmaxps() {
    printf("--- Testing vmaxps ---\n");
    
    // 测试数据
    // 使用对齐内存
    ALIGNED(32) float src1[8] = {1.0f, -2.0f, 0.0f, -0.0f, -INFINITY, NAN, 3.5f, -3.5f};
    ALIGNED(32) float src2[8] = {-1.0f, 3.0f, -0.0f, 0.0f, NAN, INFINITY, 2.5f, -2.5f};
    ALIGNED(32) float dst[8] = {0};
    
    // 预期结果
    float expected[8] = {
        1.0f,   // max(1.0, -1.0)
        3.0f,   // max(-2.0, 3.0)
        -0.0f,  // max(0.0, -0.0) - 根据指令行为返回第二个操作数(-0.0)
        0.0f,   // max(-0.0, 0.0) - 根据指令行为返回第二个操作数(0.0)
        NAN,    // max(-INFINITY, NAN) - 应该返回NAN
        INFINITY, // max(NAN, INFINITY) - 应该返回INFINITY
        3.5f,   // max(3.5, 2.5)
        -2.5f   // max(-3.5, -2.5)
    };
    
    // 设置预期结果的位模式
    uint32_t expected_bits[8];
    for (int i = 0; i < 8; i++) {
        expected_bits[i] = *(uint32_t*)&expected[i];
    }
    
    // 使用非对齐加载/存储指令
    // 256位版本 (ymm)
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vmaxps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovups %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果
    int errors = 0;
    for (int i = 0; i < 8; i++) {
        if (isnan(expected[i])) {
            if (!isnan(dst[i])) {
                printf("Error at element %d: expected NaN, got %f (0x%x)\n", i, dst[i], *(uint32_t*)&dst[i]);
                errors++;
            } else {
                printf("Element %d OK: got NaN as expected\n", i);
            }
        } else if (expected[i] == 0.0f && dst[i] == 0.0f) {
            // 检查符号位
            uint32_t exp_bits = expected_bits[i];
            uint32_t dst_bits = *(uint32_t*)&dst[i];
            if ((exp_bits & 0x80000000) != (dst_bits & 0x80000000)) {
                printf("Error at element %d: sign mismatch. Expected %f (0x%x), got %f (0x%x)\n", 
                       i, expected[i], exp_bits, dst[i], dst_bits);
                errors++;
            } else {
                printf("Element %d OK: got %f (0x%x)\n", i, dst[i], dst_bits);
            }
        } else if (dst[i] != expected[i]) {
            printf("Error at element %d: got %f (0x%x), expected %f (0x%x)\n", 
                   i, dst[i], *(uint32_t*)&dst[i], expected[i], expected_bits[i]);
            errors++;
        } else {
            printf("Element %d OK: got %f (0x%x)\n", i, dst[i], *(uint32_t*)&dst[i]);
        }
    }
    
    if (errors == 0) {
        printf("vmaxps (256-bit) test passed!\n");
    } else {
        printf("vmaxps (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm)
    ALIGNED(16) float dst128[4] = {0};
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"
        "vmovups %2, %%xmm1\n\t"
        "vmaxps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovups %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    errors = 0;
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i])) {
            if (!isnan(dst128[i])) {
                printf("Error at element %d (128-bit): expected NaN, got %f (0x%x)\n", i, dst128[i], *(uint32_t*)&dst128[i]);
                errors++;
            } else {
                printf("Element %d (128-bit) OK: got NaN as expected\n", i);
            }
        } else if (expected[i] == 0.0f && dst128[i] == 0.0f) {
            // 检查符号位
            uint32_t exp_bits = expected_bits[i];
            uint32_t dst_bits = *(uint32_t*)&dst128[i];
            if ((exp_bits & 0x80000000) != (dst_bits & 0x80000000)) {
                printf("Error at element %d (128-bit): sign mismatch. Expected %f (0x%x), got %f (0x%x)\n", 
                       i, expected[i], exp_bits, dst128[i], dst_bits);
                errors++;
            } else {
                printf("Element %d (128-bit) OK: got %f (0x%x)\n", i, dst128[i], dst_bits);
            }
        } else if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got %f (0x%x), expected %f (0x%x)\n", 
                   i, dst128[i], *(uint32_t*)&dst128[i], expected[i], expected_bits[i]);
            errors++;
        } else {
            printf("Element %d (128-bit) OK: got %f (0x%x)\n", i, dst128[i], *(uint32_t*)&dst128[i]);
        }
    }
    
    if (errors == 0) {
        printf("vmaxps (128-bit) test passed!\n");
    } else {
        printf("vmaxps (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    // 设置MXCSR寄存器以控制浮点行为
    uint32_t mxcsr = get_mxcsr();
    mxcsr &= ~(0x6000); // 清除FTZ和DAZ标志
    set_mxcsr(mxcsr);
    
    print_mxcsr(get_mxcsr());
    test_vmaxps();
    
    return 0;
}
