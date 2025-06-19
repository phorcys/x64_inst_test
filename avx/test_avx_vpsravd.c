#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRAVD指令测试
void test_vpsravd() {
    printf("=== Testing VPSRAVD (Variable Packed Shift Right Arithmetic Doubleword) ===\n");
    
    // 测试数据（包含正数和负数）
    int32_t src_data[8] = {
        0x01234567, 0x89ABCDEF, 
        0x7FFFFFFF, 0x80000000,
        0x00000001, 0xFFFFFFFF,
        0x33333333, 0xCCCCCCCC
    };
    
    // 移位量数据
    int32_t shift_counts[8] = {
        1, 2, 3, 4,
        31, 32, 0, 16
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, shift128, dst128;
    int32_t expected128[4];
    
    // 加载128位源数据和移位量
    src128 = _mm_loadu_si128((__m128i*)src_data);
    shift128 = _mm_loadu_si128((__m128i*)shift_counts);
    
    // 测试1: 可变移位
    asm volatile ("vpsravd %2, %1, %0" : "=x"(dst128) : "x"(src128), "x"(shift128));
    printf("Test 1: VPSRAVD xmm, xmm, xmm\n");
    print_xmm("Source", src128);
    print_xmm("Shift Counts", shift128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        // 移位量大于31时，结果应为符号位
        int32_t shift = shift_counts[i];
        if (shift > 31) shift = 31;
        expected128[i] = src_data[i] >> shift;
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试2: 移位量大于31的情况（使用不同的移位量）
    int32_t large_shifts[4] = {32, 33, 40, 63};
    shift128 = _mm_loadu_si128((__m128i*)large_shifts);
    asm volatile ("vpsravd %2, %1, %0" : "=x"(dst128) : "x"(src128), "x"(shift128));
    printf("\nTest 2: VPSRAVD xmm, xmm, xmm (large shifts)\n");
    print_xmm("Shift Counts", shift128);
    print_xmm("Result", dst128);
    
    // 计算预期值（所有元素都应变为符号位）
    for (int i = 0; i < 4; i++) {
        expected128[i] = src_data[i] >> 31;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, shift256, dst256;
    int32_t expected256[8];
    
    // 加载256位源数据和移位量
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    shift256 = _mm256_loadu_si256((__m256i*)shift_counts);
    
    // 测试3: 可变移位
    asm volatile ("vpsravd %2, %1, %0" : "=x"(dst256) : "x"(src256), "x"(shift256));
    printf("\nTest 3: VPSRAVD ymm, ymm, ymm\n");
    print_ymm("Source", src256);
    print_ymm("Shift Counts", shift256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        // 移位量大于31时，结果应为符号位
        int32_t shift = shift_counts[i];
        if (shift > 31) shift = 31;
        expected256[i] = src_data[i] >> shift;
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试4: 移位量大于31的情况（使用不同的移位量）
    int32_t large_shifts256[8] = {32, 33, 40, 63, 64, 100, 0, 16};
    shift256 = _mm256_loadu_si256((__m256i*)large_shifts256);
    asm volatile ("vpsravd %2, %1, %0" : "=x"(dst256) : "x"(src256), "x"(shift256));
    printf("\nTest 4: VPSRAVD ymm, ymm, ymm (large shifts)\n");
    print_ymm("Shift Counts", shift256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        // 移位量大于31时，结果应为符号位
        int32_t shift = large_shifts256[i];
        if (shift > 31) {
            expected256[i] = src_data[i] >> 31;
        } else {
            expected256[i] = src_data[i] >> shift;
        }
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试5: 移位量为0 (无变化)
    int32_t zero_shifts[4] = {0, 0, 0, 0};
    shift128 = _mm_loadu_si128((__m128i*)zero_shifts);
    asm volatile ("vpsravd %2, %1, %0" : "=x"(dst128) : "x"(src128), "x"(shift128));
    printf("\nTest 5: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsravd();
    return 0;
}
