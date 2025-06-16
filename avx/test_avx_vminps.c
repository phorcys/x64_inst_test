#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vminps 指令测试
static void test_vminps() {
    printf("--- Testing vminps ---\n");
    
    // 测试数据
    ALIGNED(32) float src1[8] = {
        1.0f, -2.0f, 0.0f, -0.0f, 
        -INFINITY, NAN, 3.5f, -3.5f
    };
    
    ALIGNED(32) float src2[8] = {
        -1.0f, 3.0f, -0.0f, 0.0f, 
        NAN, INFINITY, 2.5f, -2.5f
    };
    
    // 添加更多边界情况
    ALIGNED(32) float src3[8] = {
        INFINITY, -INFINITY, __builtin_nanf(""), -0.0f,
        0.0f, __builtin_nanf(""), -INFINITY, 0.0f
    };
    
    ALIGNED(32) float src4[8] = {
        0.0f, __builtin_nanf(""), -INFINITY, 0.0f,
        INFINITY, -INFINITY, __builtin_nanf(""), -0.0f
    };
    
    ALIGNED(32) float dst[8] = {0};
    
    // 预期结果
    float expected[8] = {
        -1.0f,   // min(1.0, -1.0) -> -1.0
        -2.0f,   // min(-2.0, 3.0) -> -2.0
        -0.0f,   // min(0.0, -0.0) -> 返回第二个操作数(-0.0)
        0.0f,    // min(-0.0, 0.0) -> 返回第二个操作数(0.0) [修正]
        NAN,     // min(-INFINITY, NAN) -> 返回第二个操作数(NAN)
        INFINITY,// min(NAN, INFINITY) -> 返回第二个操作数(INFINITY)
        2.5f,    // min(3.5, 2.5) -> 2.5
        -3.5f    // min(-3.5, -2.5) -> -3.5
    };
    
    // 设置预期结果的位模式
    uint32_t expected_bits[8] = {
        *(uint32_t*)&expected[0],
        *(uint32_t*)&expected[1],
        *(uint32_t*)&expected[2],
        *(uint32_t*)&expected[3],
        *(uint32_t*)&expected[4],
        *(uint32_t*)&expected[5],
        *(uint32_t*)&expected[6],
        *(uint32_t*)&expected[7]
    };
    
    // 手动设置第3个元素的位模式为0.0f (0x00000000)
    expected_bits[3] = 0x00000000;  // 0.0f的位模式
    
    // 256位版本 (ymm) - 测试1
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vminps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 256位版本 (ymm) - 测试2 (更多边界情况)
    float dst2[8] = {0};
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vminps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m" (dst2)
        : "m" (src3), "m" (src4)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果 - 测试1
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
    
    // 检查结果 - 测试2
    float expected2[8] = {
        0.0f,        // min(INFINITY, 0.0) -> 0.0
        NAN,         // min(-INFINITY, NAN) -> 返回第二个操作数(NaN)
        -INFINITY,   // min(NAN, -INFINITY) -> 返回第二个操作数(-INFINITY)
        0.0f,        // min(-0.0, 0.0) -> 返回第二个操作数(0.0)
        0.0f,        // min(0.0, INFINITY) -> 0.0
        -INFINITY,   // min(NAN, -INFINITY) -> 返回第二个操作数(-INFINITY)
        -INFINITY,   // min(-INFINITY, NAN) -> 返回第二个操作数(NaN) -> 但这里应该是NAN，修正为NAN
        -0.0f        // min(0.0, -0.0) -> 返回第二个操作数(-0.0)
    };
    
    // 修正第6个元素（索引6）的预期结果
    expected2[6] = NAN;  // 因为src3[6] = -INFINITY, src4[6] = NAN -> 返回第二个操作数(NaN)
    
    // 设置预期结果的位模式
    uint32_t expected2_bits[8] = {
        *(uint32_t*)&expected2[0],
        *(uint32_t*)&expected2[1],
        *(uint32_t*)&expected2[2],
        *(uint32_t*)&expected2[3],
        *(uint32_t*)&expected2[4],
        *(uint32_t*)&expected2[5],
        *(uint32_t*)&expected2[6],
        *(uint32_t*)&expected2[7]
    };
    
    for (int i = 0; i < 8; i++) {
        if (isnan(expected2[i])) {
            if (!isnan(dst2[i])) {
                printf("Error at element %d: expected NaN, got %f (0x%x)\n", i, dst2[i], *(uint32_t*)&dst2[i]);
                errors++;
            } else {
                printf("Element %d OK: got NaN as expected\n", i);
            }
        } else if (expected2[i] == 0.0f && dst2[i] == 0.0f) {
            // 检查符号位
            uint32_t exp_bits = expected2_bits[i];
            uint32_t dst_bits = *(uint32_t*)&dst2[i];
            if ((exp_bits & 0x80000000) != (dst_bits & 0x80000000)) {
                printf("Error at element %d: sign mismatch. Expected %f (0x%x), got %f (0x%x)\n", 
                       i, expected2[i], exp_bits, dst2[i], dst_bits);
                errors++;
            } else {
                printf("Element %d OK: got %f (0x%x)\n", i, dst2[i], dst_bits);
            }
        } else if (dst2[i] != expected2[i]) {
            printf("Error at element %d: got %f (0x%x), expected %f (0x%x)\n", 
                   i, dst2[i], *(uint32_t*)&dst2[i], expected2[i], expected2_bits[i]);
            errors++;
        } else {
            printf("Element %d OK: got %f (0x%x)\n", i, dst2[i], *(uint32_t*)&dst2[i]);
        }
    }
    
    if (errors == 0) {
        printf("vminps (256-bit) test passed!\n");
    } else {
        printf("vminps (256-bit) test failed with %d errors\n", errors);
    }
    
    if (errors == 0) {
        printf("vminps (256-bit) test passed!\n");
    } else {
        printf("vminps (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm) - 测试1
    ALIGNED(16) float dst128[4] = {0};
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vminps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 128位版本 (xmm) - 测试2
    ALIGNED(16) float dst128_2[4] = {0};
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vminps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m" (dst128_2)
        : "m" (src3), "m" (src4)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果 - 测试1
    int errors128 = 0;
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i])) {
            if (!isnan(dst128[i])) {
                printf("Error at element %d (128-bit): expected NaN, got %f (0x%x)\n", 
                       i, dst128[i], *(uint32_t*)&dst128[i]);
                errors128++;
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
                errors128++;
            } else {
                printf("Element %d (128-bit) OK: got %f (0x%x)\n", i, dst128[i], dst_bits);
            }
        } else if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got %f (0x%x), expected %f (0x%x)\n", 
                   i, dst128[i], *(uint32_t*)&dst128[i], expected[i], expected_bits[i]);
            errors128++;
        } else {
            printf("Element %d (128-bit) OK: got %f (0x%x)\n", i, dst128[i], *(uint32_t*)&dst128[i]);
        }
    }
    
    // 128位结果检查 - 测试2
    float expected128_2[4] = {
        0.0f,        // min(INFINITY, 0.0) -> 0.0
        NAN,         // min(-INFINITY, NAN) -> 返回第二个操作数(NaN)
        -INFINITY,   // min(NAN, -INFINITY) -> 返回第二个操作数(-INFINITY)
        0.0f         // min(-0.0, 0.0) -> 返回第二个操作数(0.0)
    };
    
    for (int i = 0; i < 4; i++) {
        if (isnan(expected128_2[i])) {
            if (!isnan(dst128_2[i])) {
                printf("Error at element %d (128-bit test2): expected NaN, got %f\n", i, dst128_2[i]);
                errors128++;
            }
        } else if (expected128_2[i] == 0.0f && dst128_2[i] == 0.0f) {
            // 检查符号位
            uint32_t exp_bits = *(uint32_t*)&expected128_2[i];
            uint32_t dst_bits = *(uint32_t*)&dst128_2[i];
            if ((exp_bits & 0x80000000) != (dst_bits & 0x80000000)) {
                printf("Error at element %d (128-bit test2): sign mismatch. Expected %f (0x%x), got %f (0x%x)\n", 
                       i, expected128_2[i], exp_bits, dst128_2[i], dst_bits);
                errors128++;
            }
        } else if (dst128_2[i] != expected128_2[i]) {
            printf("Error at element %d (128-bit test2): got %f, expected %f\n", i, dst128_2[i], expected128_2[i]);
            errors128++;
        }
    }
    
    if (errors128 == 0) {
        printf("vminps (128-bit) test passed!\n");
    } else {
        printf("vminps (128-bit) test failed with %d errors\n", errors128);
    }
    
    // 总错误计数
    int total_errors = errors + errors128;
    
    if (total_errors == 0) {
        printf("All vminps tests passed!\n");
    } else {
        printf("vminps tests failed with %d total errors\n", total_errors);
    }
    
    if (errors == 0) {
        printf("vminps (128-bit) test passed!\n");
    } else {
        printf("vminps (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    // 设置MXCSR寄存器以控制浮点行为
    uint32_t mxcsr = get_mxcsr();
    mxcsr &= ~(0x6000); // 清除FTZ和DAZ标志
    set_mxcsr(mxcsr);
    
    print_mxcsr(get_mxcsr());
    test_vminps();
    
    return 0;
}
