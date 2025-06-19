#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRAW指令测试
void test_vpsraw() {
    printf("=== Testing VPSRAW (Vector Packed Shift Right Arithmetic Word) ===\n");
    
    // 测试数据（包含正数和负数）
    int16_t src_data[16] = {
        0x1234, 0x5678, 0x7FFF, 0x8000,
        0x0001, 0xFFFF, 0x3333, 0xCCCC,
        0x00FF, 0xFF00, 0x55AA, 0xAA55,
        0x1000, 0x2000, 0x4000, 0x8000
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, dst128;
    int16_t expected128[8];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 测试1: 立即数移位 (移位量=1)
    asm volatile ("vpsraw $1, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("Test 1: VPSRAW xmm, xmm, 1\n");
    print_xmm("Source", src128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] >> 1;
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试2: 立即数移位 (移位量=4)
    asm volatile ("vpsraw $4, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 2: VPSRAW xmm, xmm, 4\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] >> 4;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试3: 立即数移位 (移位量=15)
    asm volatile ("vpsraw $15, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 3: VPSRAW xmm, xmm, 15\n");
    print_xmm("Result", dst128);
    
    // 计算预期值（所有元素变为符号位）
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] >> 15;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, dst256;
    int16_t expected256[16];
    
    // 加载256位源数据（重复128位数据）
    int16_t src256_data[16];
    for (int i = 0; i < 16; i++) {
        src256_data[i] = src_data[i % 8];
    }
    src256 = _mm256_loadu_si256((__m256i*)src256_data);
    
    // 测试4: 立即数移位 (移位量=1)
    asm volatile ("vpsraw $1, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 4: VPSRAW ymm, ymm, 1\n");
    print_ymm("Source", src256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 16; i++) {
        expected256[i] = src256_data[i] >> 1;
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试5: 立即数移位 (移位量=4)
    asm volatile ("vpsraw $4, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 5: VPSRAW ymm, ymm, 4\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 16; i++) {
        expected256[i] = src256_data[i] >> 4;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试6: 立即数移位 (移位量=15)
    asm volatile ("vpsraw $15, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 6: VPSRAW ymm, ymm, 15\n");
    print_ymm("Result", dst256);
    
    // 计算预期值（所有元素变为符号位）
    for (int i = 0; i < 16; i++) {
        expected256[i] = src256_data[i] >> 15;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试7: 移位量为0 (无变化)
    asm volatile ("vpsraw $0, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 7: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
    
    // 测试8: 移位量为16 (应为全符号位)
    asm volatile ("vpsraw $16, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 8: Shift count 16 (should be all sign bits)\n");
    print_xmm("Result", dst128);
    
    // 计算预期值（所有元素变为符号位）
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] >> 15;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsraw();
    return 0;
}
