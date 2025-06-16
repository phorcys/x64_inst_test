#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// vminpd 指令测试
static void test_vminpd() {
    printf("--- Testing vminpd ---\n");
    
    // 测试数据
    ALIGNED(32) double src1[4] = {
        1.0,
        -2.0,
        0.0,
        -INFINITY
    };
    
    ALIGNED(32) double src2[4] = {
        -1.0,
        3.0,
        -0.0,
        NAN
    };
    
    // 添加更多边界情况
    ALIGNED(32) double src3[4] = {
        INFINITY,
        -INFINITY,
        __builtin_nan(""),
        -0.0
    };
    
    ALIGNED(32) double src4[4] = {
        0.0,
        __builtin_nan(""),
        -INFINITY,
        0.0
    };
    ALIGNED(32) double dst[4] = {0};
    
    // 预期结果
    double expected[4] = {
        -1.0,   // min(1.0, -1.0)
        -2.0,   // min(-2.0, 3.0)
        -0.0,   // min(0.0, -0.0) - 应该返回第二个操作数(-0.0)
        NAN     // min(-INFINITY, NAN) - 应该返回NAN
    };
    
    // 设置预期结果的位模式
    uint64_t expected_bits[4] = {
        *(uint64_t*)&expected[0],
        *(uint64_t*)&expected[1],
        *(uint64_t*)&expected[2],
        *(uint64_t*)&expected[3]
    };
    
    // 256位版本 (ymm) - 测试1
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vminpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m" (dst)
        : "m" (src1), "m" (src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 256位版本 (ymm) - 测试2 (更多边界情况)
    double dst2[4] = {0};
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vminpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m" (dst2)
        : "m" (src3), "m" (src4)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 检查结果 - 测试1
    int errors = 0;
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i])) {
            if (!isnan(dst[i])) {
                printf("Error at element %d: expected NaN, got %f (0x%lx)\n", i, dst[i], *(uint64_t*)&dst[i]);
                errors++;
            } else {
                printf("Element %d OK: got NaN as expected\n", i);
            }
        } else if (expected[i] == 0.0 && dst[i] == 0.0) {
            // 检查符号位
            uint64_t exp_bits = expected_bits[i];
            uint64_t dst_bits = *(uint64_t*)&dst[i];
            if ((exp_bits & 0x8000000000000000) != (dst_bits & 0x8000000000000000)) {
                printf("Error at element %d: sign mismatch. Expected %f (0x%lx), got %f (0x%lx)\n", 
                       i, expected[i], exp_bits, dst[i], dst_bits);
                errors++;
            } else {
                printf("Element %d OK: got %f (0x%lx)\n", i, dst[i], dst_bits);
            }
        } else if (dst[i] != expected[i]) {
            printf("Error at element %d: got %f (0x%lx), expected %f (0x%lx)\n", 
                   i, dst[i], *(uint64_t*)&dst[i], expected[i], expected_bits[i]);
            errors++;
        } else {
            printf("Element %d OK: got %f (0x%lx)\n", i, dst[i], *(uint64_t*)&dst[i]);
        }
    }
    
    // 检查结果 - 测试2
    double expected2[4] = {
        0.0,        // min(INFINITY, 0.0) -> 0.0
        NAN,        // min(-INFINITY, NAN) -> 返回第二个操作数(NaN)
        -INFINITY,  // min(NAN, -INFINITY) -> 返回第二个操作数(-INFINITY)
        0.0         // min(-0.0, 0.0) -> 返回第二个操作数(0.0)
    };
    
    // 设置预期结果的位模式
    uint64_t expected2_bits[4] = {
        *(uint64_t*)&expected2[0],
        *(uint64_t*)&expected2[1],
        *(uint64_t*)&expected2[2],
        *(uint64_t*)&expected2[3]
    };
    
    for (int i = 0; i < 4; i++) {
        if (isnan(expected2[i])) {
            if (!isnan(dst2[i])) {
                printf("Error at element %d: expected NaN, got %f (0x%lx)\n", i, dst2[i], *(uint64_t*)&dst2[i]);
                errors++;
            } else {
                printf("Element %d OK: got NaN as expected\n", i);
            }
        } else if (expected2[i] == 0.0 && dst2[i] == 0.0) {
            // 检查符号位
            uint64_t exp_bits = expected2_bits[i];
            uint64_t dst_bits = *(uint64_t*)&dst2[i];
            if ((exp_bits & 0x8000000000000000) != (dst_bits & 0x8000000000000000)) {
                printf("Error at element %d: sign mismatch. Expected %f (0x%lx), got %f (0x%lx)\n", 
                       i, expected2[i], exp_bits, dst2[i], dst_bits);
                errors++;
            } else {
                printf("Element %d OK: got %f (0x%lx)\n", i, dst2[i], dst_bits);
            }
        } else if (dst2[i] != expected2[i]) {
            printf("Error at element %d: got %f (0x%lx), expected %f (0x%lx)\n", 
                   i, dst2[i], *(uint64_t*)&dst2[i], expected2[i], expected2_bits[i]);
            errors++;
        } else {
            printf("Element %d OK: got %f (0x%lx)\n", i, dst2[i], *(uint64_t*)&dst2[i]);
        }
    }
    
    if (errors == 0) {
        printf("vminpd (256-bit) test passed!\n");
    } else {
        printf("vminpd (256-bit) test failed with %d errors\n", errors);
    }
    
    // 128位版本 (xmm) - 测试1
    ALIGNED(16) double dst128[2] = {0};
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vminpd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %0\n\t"
        : "=m" (dst128)
        : "m" (src1), "m" (src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 128位版本 (xmm) - 测试2
    ALIGNED(16) double dst128_2[2] = {0};
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vminpd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %0\n\t"
        : "=m" (dst128_2)
        : "m" (src3), "m" (src4)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 检查128位结果
    // 128位结果检查 - 测试1
    int errors128 = 0;
    for (int i = 0; i < 2; i++) {
        if (isnan(expected[i])) {
            if (!isnan(dst128[i])) {
                printf("Error at element %d (128-bit): expected NaN, got %f (0x%lx)\n", 
                       i, dst128[i], *(uint64_t*)&dst128[i]);
                errors128++;
            }
        } else if (expected[i] == 0.0 && dst128[i] == 0.0) {
            // 检查符号位
            uint64_t exp_bits = expected_bits[i];
            uint64_t dst_bits = *(uint64_t*)&dst128[i];
            if ((exp_bits & 0x8000000000000000) != (dst_bits & 0x8000000000000000)) {
                printf("Error at element %d (128-bit): sign mismatch. Expected %f (0x%lx), got %f (0x%lx)\n", 
                       i, expected[i], exp_bits, dst128[i], dst_bits);
                errors128++;
            } else {
                printf("Element %d (128-bit) OK: got %f (0x%lx)\n", i, dst128[i], dst_bits);
            }
        } else if (dst128[i] != expected[i]) {
            printf("Error at element %d (128-bit): got %f (0x%lx), expected %f (0x%lx)\n", 
                   i, dst128[i], *(uint64_t*)&dst128[i], expected[i], expected_bits[i]);
            errors128++;
        } else {
            printf("Element %d (128-bit) OK: got %f (0x%lx)\n", i, dst128[i], *(uint64_t*)&dst128[i]);
        }
    }
    
    // 128位结果检查 - 测试2
    double expected128_2[2] = {
        0.0,        // min(INFINITY, 0.0) -> 0.0
        NAN         // min(-INFINITY, NAN) -> 返回第二个操作数(NaN)
    };
    
    for (int i = 0; i < 2; i++) {
        if (isnan(expected128_2[i])) {
            if (!isnan(dst128_2[i])) {
                printf("Error at element %d (128-bit test2): expected NaN, got %f\n", i, dst128_2[i]);
                errors128++;
            }
        } else if (expected128_2[i] == 0.0 && dst128_2[i] == 0.0) {
            // 检查符号位
            uint64_t exp_bits = *(uint64_t*)&expected128_2[i];
            uint64_t dst_bits = *(uint64_t*)&dst128_2[i];
            if ((exp_bits & 0x8000000000000000) != (dst_bits & 0x8000000000000000)) {
                printf("Error at element %d (128-bit test2): sign mismatch. Expected %f (0x%lx), got %f (0x%lx)\n", 
                       i, expected128_2[i], exp_bits, dst128_2[i], dst_bits);
                errors128++;
            }
        } else if (dst128_2[i] != expected128_2[i]) {
            printf("Error at element %d (128-bit test2): got %f, expected %f\n", i, dst128_2[i], expected128_2[i]);
            errors128++;
        }
    }
    
    if (errors128 == 0) {
        printf("vminpd (128-bit) test passed!\n");
    } else {
        printf("vminpd (128-bit) test failed with %d errors\n", errors128);
    }
    
    // 总错误计数
    int total_errors = errors + errors128;
    
    if (total_errors == 0) {
        printf("All vminpd tests passed!\n");
    } else {
        printf("vminpd tests failed with %d total errors\n", total_errors);
    }
    
    if (errors == 0) {
        printf("vminpd (128-bit) test passed!\n");
    } else {
        printf("vminpd (128-bit) test failed with %d errors\n", errors);
    }
    
    if (errors == 0) {
        printf("vminpd (128-bit) test passed!\n");
    } else {
        printf("vminpd (128-bit) test failed with %d errors\n", errors);
    }
}

int main() {
    // 设置MXCSR寄存器以控制浮点行为
    uint32_t mxcsr = get_mxcsr();
    mxcsr &= ~(0x6000); // 清除FTZ和DAZ标志
    set_mxcsr(mxcsr);
    
    print_mxcsr(get_mxcsr());
    test_vminpd();
    
    return 0;
}
