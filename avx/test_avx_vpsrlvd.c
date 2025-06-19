#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRLVD指令测试
void test_vpsrlvd() {
    printf("=== Testing VPSRLVD (Variable Shift Right Logical Doublewords) ===\n");
    
    // 测试数据（32位值）
    uint32_t src_data[8] = {
        0x12345678,
        0x7FFFFFFF,
        0x80000000,
        0xFEDCBA98,
        0x87654321,
        0x55555555,
        0xAAAAAAAA,
        0xDEADBEEF
    };
    
    // 移位计数数据（每个元素指定对应元素的移位量）
    uint32_t shift_counts[8] = {
        1, 4, 8, 16,
        31, 0, 32, 5
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, shift128, dst128;
    uint32_t expected128[4];
    
    // 加载128位源数据和移位计数
    src128 = _mm_loadu_si128((__m128i*)src_data);
    shift128 = _mm_loadu_si128((__m128i*)shift_counts);
    
    // 执行VPSRLVD指令
    asm volatile ("vpsrlvd %1, %2, %0" : "=x"(dst128) : "x"(shift128), "x"(src128));
    printf("Test: VPSRLVD xmm, xmm, xmm\n");
    print_xmm("Source", src128);
    print_xmm("Shift Counts", shift128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        // 移位量大于31时结果为0
        if (shift_counts[i] > 31) {
            expected128[i] = 0;
        } else {
            expected128[i] = src_data[i] >> shift_counts[i];
        }
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, shift256, dst256;
    uint32_t expected256[8];
    
    // 加载256位源数据和移位计数
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    shift256 = _mm256_loadu_si256((__m256i*)shift_counts);
    
    // 执行VPSRLVD指令
    asm volatile ("vpsrlvd %1, %2, %0" : "=x"(dst256) : "x"(shift256), "x"(src256));
    printf("Test: VPSRLVD ymm, ymm, ymm\n");
    print_ymm("Source", src256);
    print_ymm("Shift Counts", shift256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        // 移位量大于31时结果为0
        if (shift_counts[i] > 31) {
            expected256[i] = 0;
        } else {
            expected256[i] = src_data[i] >> shift_counts[i];
        }
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
}

int main() {
    test_vpsrlvd();
    return 0;
}
