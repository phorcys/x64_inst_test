#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRLDQ指令测试
void test_vpsrldq() {
    printf("=== Testing VPSRLDQ (Vector Packed Shift Right Logical Double Quadword) ===\n");
    
    // 测试数据（128位）
    uint8_t src_data[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, dst128;
    uint8_t expected128[16];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 测试1: 移位1字节
    asm volatile ("vpsrldq $1, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("Test 1: VPSRLDQ xmm, xmm, 1\n");
    print_xmm("Source", src128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 15; i++) {
        expected128[i] = src_data[i+1];
    }
    expected128[15] = 0;
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试2: 移位4字节
    asm volatile ("vpsrldq $4, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 2: VPSRLDQ xmm, xmm, 4\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 12; i++) {
        expected128[i] = src_data[i+4];
    }
    for (int i = 12; i < 16; i++) {
        expected128[i] = 0;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试3: 移位16字节（全部移出）
    asm volatile ("vpsrldq $16, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 3: VPSRLDQ xmm, xmm, 16\n");
    print_xmm("Result", dst128);
    
    // 计算预期值（全0）
    for (int i = 0; i < 16; i++) {
        expected128[i] = 0;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, dst256;
    uint8_t expected256[32];
    
    // 加载256位源数据（重复128位数据）
    uint8_t src256_data[32];
    for (int i = 0; i < 16; i++) {
        src256_data[i] = src_data[i];
        src256_data[i+16] = src_data[i];
    }
    src256 = _mm256_loadu_si256((__m256i*)src256_data);
    
    // 测试4: 移位1字节
    asm volatile ("vpsrldq $1, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 4: VPSRLDQ ymm, ymm, 1\n");
    print_ymm("Source", src256);
    print_ymm("Result", dst256);
    
    // 计算预期值（每个128位部分独立移位）
    for (int i = 0; i < 15; i++) {
        expected256[i] = src256_data[i+1];
        expected256[i+16] = src256_data[i+17];
    }
    expected256[15] = 0;
    expected256[31] = 0;
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试5: 移位8字节
    asm volatile ("vpsrldq $8, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 5: VPSRLDQ ymm, ymm, 8\n");
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 8; i++) {
        expected256[i] = src256_data[i+8];
        expected256[i+16] = src256_data[i+24];
    }
    for (int i = 8; i < 16; i++) {
        expected256[i] = 0;
        expected256[i+16] = 0;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试6: 移位32字节（全部移出）
    asm volatile ("vpsrldq $32, %1, %0" : "=x"(dst256) : "x"(src256));
    printf("\nTest 6: VPSRLDQ ymm, ymm, 32\n");
    print_ymm("Result", dst256);
    
    // 计算预期值（全0）
    for (int i = 0; i < 32; i++) {
        expected256[i] = 0;
    }
    exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试7: 移位量为0 (无变化)
    asm volatile ("vpsrldq $0, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 7: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
    
    // 测试8: 移位量大于16 (128位)
    asm volatile ("vpsrldq $17, %1, %0" : "=x"(dst128) : "x"(src128));
    printf("\nTest 8: Shift count 17 (128-bit, should be all zeros)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsrldq();
    return 0;
}
