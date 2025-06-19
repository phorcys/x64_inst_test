#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRLQ指令测试
void test_vpsrlq() {
    printf("=== Testing VPSRLQ (Vector Packed Shift Right Logical Quadword) ===\n");
    
    // 测试数据（64位值）
    uint64_t src_data[4] = {
        0x123456789ABCDEF0,
        0x7FFFFFFFFFFFFFFF,
        0x8000000000000000,
        0xFEDCBA9876543210
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, dst128;
    uint64_t expected128[2];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 测试1: 立即数移位 (移位量=1)
    asm volatile ("vpsrlq $1, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("Test 1: VPSRLQ xmm, xmm, 1\n");
    print_xmm("Source", src128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    expected128[0] = src_data[0] >> 1;
    expected128[1] = src_data[1] >> 1;
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试2: 立即数移位 (移位量=4)
    asm volatile ("vpsrlq $4, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 2: VPSRLQ xmm, xmm, 4\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    expected128[0] = src_data[0] >> 4;
    expected128[1] = src_data[1] >> 4;
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试3: 立即数移位 (移位量=63)
    asm volatile ("vpsrlq $63, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 3: VPSRLQ xmm, xmm, 63\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    expected128[0] = src_data[0] >> 63;
    expected128[1] = src_data[1] >> 63;
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, dst256;
    uint64_t expected256[4];
    
    // 加载256位源数据
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    
    // 测试4: 立即数移位 (移位量=1)
    asm volatile ("vpsrlq $1, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 4: VPSRLQ ymm, ymm, 1\n");
    print_ymm("Source", src256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        expected256[i] = src_data[i] >> 1;
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试5: 立即数移位 (移位量=4)
    asm volatile ("vpsrlq $4, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 5: VPSRLQ ymm, ymm, 4\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        expected256[i] = src_data[i] >> 4;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试6: 立即数移位 (移位量=63)
    asm volatile ("vpsrlq $63, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 6: VPSRLQ ymm, ymm, 63\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        expected256[i] = src_data[i] >> 63;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试7: 移位量为0 (无变化)
    asm volatile ("vpsrlq $0, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 7: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
    
    // 测试8: 移位量为64 (应为全0)
    asm volatile ("vpsrlq $64, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 8: Shift count 64 (should be all zeros)\n");
    print_xmm("Result", dst128);
    
    // 计算预期值（所有元素变为0）
    expected128[0] = 0;
    expected128[1] = 0;
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsrlq();
    return 0;
}
