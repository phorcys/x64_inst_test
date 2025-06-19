#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// VPSLLVD指令测试
void test_vpsllvd() {
    printf("=== Testing VPSLLVD (Vector Packed Shift Left Logical Variable Doubleword) ===\n");
    
    // 测试数据
    uint32_t src_data[8] = {
        0x12345678, 0x7FFFFFFF, 0x80000000, 0xFEDCBA98,
        0x00000001, 0x55555555, 0xAAAAAAAA, 0xFFFFFFFF
    };
    
    // 移位量数据（每个双字独立）
    uint32_t shift_counts[8] = {
        1, 4, 8, 16,
        31, 32, 33, 0
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, shift128, dst128;
    uint32_t expected128[4];
    
    // 加载128位源数据和移位量
    src128 = _mm_loadu_si128((__m128i*)src_data);
    shift128 = _mm_loadu_si128((__m128i*)shift_counts);
    
    // 执行VPSLLVD指令
    asm volatile ("vpsllvd %1, %2, %0" : "=x"(dst128) : "x"(shift128), "x"(src128));
    print_xmm("Source", src128);
    print_xmm("Shift Counts", shift128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        if (shift_counts[i] >= 32) {
            expected128[i] = 0;
        } else {
            expected128[i] = src_data[i] << shift_counts[i];
        }
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, shift256, dst256;
    uint32_t expected256[8];
    
    // 加载256位源数据和移位量
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    shift256 = _mm256_loadu_si256((__m256i*)shift_counts);
    
    // 执行VPSLLVD指令
    asm volatile ("vpsllvd %1, %2, %0" : "=x"(dst256) : "x"(shift256), "x"(src256));
    print_ymm("Source", src256);
    print_ymm("Shift Counts", shift256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        if (shift_counts[i] >= 32) {
            expected256[i] = 0;
        } else {
            expected256[i] = src_data[i] << shift_counts[i];
        }
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
}

int main() {
    test_vpsllvd();
    return 0;
}
