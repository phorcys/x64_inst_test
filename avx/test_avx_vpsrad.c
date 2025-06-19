#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRAD指令测试
void test_vpsrad() {
    printf("=== Testing VPSRAD (Vector Packed Shift Right Arithmetic Doubleword) ===\n");
    
    // 测试数据（包含正数和负数）
    int32_t src_data[8] = {
        0x01234567, 0x89ABCDEF, 
        0x7FFFFFFF, 0x80000000,
        0x00000001, 0xFFFFFFFF,
        0x33333333, 0xCCCCCCCC
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, dst128;
    int32_t expected128[4];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 测试1: 立即数移位 (移位量=1)
    asm volatile ("vpsrad $1, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("Test 1: VPSRAD xmm, xmm, 1\n");
    print_xmm("Source", src128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        expected128[i] = src_data[i] >> 1;
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试2: 立即数移位 (移位量=2)
    asm volatile ("vpsrad $2, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 2: VPSRAD xmm, xmm, 2\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        expected128[i] = src_data[i] >> 2;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试3: 立即数移位 (移位量=31)
    asm volatile ("vpsrad $31, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 3: VPSRAD xmm, xmm, 31\n");
    print_xmm("Result", dst128);
    
    // 计算预期值（所有元素变为符号位）
    for (int i = 0; i < 4; i++) {
        expected128[i] = src_data[i] >> 31;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, dst256;
    int32_t expected256[8];
    
    // 加载256位源数据（重复128位数据）
    int32_t src256_data[8];
    for (int i = 0; i < 8; i++) {
        src256_data[i] = src_data[i % 4];
    }
    src256 = _mm256_loadu_si256((__m256i*)src256_data);
    
    // 测试4: 立即数移位 (移位量=1)
    asm volatile ("vpsrad $1, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 4: VPSRAD ymm, ymm, 1\n");
    print_ymm("Source", src256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected256[i] = src256_data[i] >> 1;
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试5: 立即数移位 (移位量=2)
    asm volatile ("vpsrad $2, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 5: VPSRAD ymm, ymm, 2\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected256[i] = src256_data[i] >> 2;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试6: 立即数移位 (移位量=31)
    asm volatile ("vpsrad $31, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 6: VPSRAD ymm, ymm, 31\n");
    print_ymm("Result", dst256);
    
    // 计算预期值（所有元素变为符号位）
    for (int i = 0; i < 8; i++) {
        expected256[i] = src256_data[i] >> 31;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试7: 移位量为0 (无变化)
    asm volatile ("vpsrad $0, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 7: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
    
    // 测试8: 移位量为32 (应为全符号位)
    asm volatile ("vpsrad $32, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 8: Shift count 32 (should be all sign bits)\n");
    print_xmm("Result", dst128);
    
    // 计算预期值（所有元素变为符号位）
    for (int i = 0; i < 4; i++) {
        expected128[i] = src_data[i] >> 31;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsrad();
    return 0;
}
