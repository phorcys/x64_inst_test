#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSLLW指令测试
void test_vpsllw() {
    printf("=== Testing VPSLLW (Vector Packed Shift Left Logical Word) ===\n");
    
    // 测试数据
    uint16_t src_data[16] = {
        0x0123, 0x4567, 0x89AB, 0xCDEF,
        0x5555, 0xAAAA, 0x7FFF, 0x8000,
        0x0001, 0xFFFF, 0x3333, 0xCCCC,
        0x0F0F, 0xF0F0, 0x00FF, 0xFF00
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, dst128;
    uint16_t expected128[8];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 测试1: 立即数移位 (移位量=1)
    asm volatile ("vpsllw $1, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("Test 1: VPSLLW xmm, xmm, 1\n");
    print_xmm("Source", src128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] << 1;
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试2: 标量移位 (使用立即数)
    asm volatile ("vpsllw $2, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 2: VPSLLW xmm, xmm, 2\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] << 2;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试3: 立即数移位 (移位量=15)
    asm volatile ("vpsllw $15, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 3: VPSLLW xmm, xmm, 15\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected128[i] = src_data[i] << 15;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, dst256;
    uint16_t expected256[16];
    
    // 加载256位源数据
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    
    // 测试4: 立即数移位 (移位量=1)
    asm volatile ("vpsllw $1, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 4: VPSLLW ymm, ymm, 1\n");
    print_ymm("Source", src256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 16; i++) {
        expected256[i] = src_data[i] << 1;
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试5: 标量移位 (使用立即数)
    asm volatile ("vpsllw $2, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 5: VPSLLW ymm, ymm, 2\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 16; i++) {
        expected256[i] = src_data[i] << 2;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试6: 立即数移位 (移位量=15)
    asm volatile ("vpsllw $15, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 6: VPSLLW ymm, ymm, 15\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 16; i++) {
        expected256[i] = src_data[i] << 15;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试7: 移位量为0 (无变化)
    asm volatile ("vpsllw $0, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 7: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
    
    // 测试8: 移位量为16 (应为0)
    asm volatile ("vpsllw $16, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 8: Shift count 16 (should be 0)\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected128[i] = 0;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试9: 移位量大于15 (应为0)
    __m128i shift_large = _mm_set1_epi16(17); // 移位量=17（16位值）
    asm volatile ("vpsllw %2, %1, %0" : "=x"(dst128) : "x"(src128), "x"(shift_large));
    printf("\nTest 9: Shift count 17 (should be 0)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsllw();
    return 0;
}
