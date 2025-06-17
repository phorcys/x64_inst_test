#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVMSKPS 指令测试
void test_vmovmskps() {
    printf("=== Testing VMOVMSKPS ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    float src128[4] = {-1.0f, 2.0f, -3.0f, 4.0f}; // 负数符号位为1，正数为0
    unsigned int expected128 = 0b0101; // 预期掩码值
    
    __m128 vec128 = _mm_loadu_ps(src128);
    unsigned int result128;
    
    __asm__ __volatile__ (
        "vmovmskps %1, %0" // 执行VMOVMSKPS
        : "=r" (result128)
        : "x" (vec128)
    );
    
    printf("Source: [%.1f, %.1f, %.1f, %.1f]\n", 
           src128[0], src128[1], src128[2], src128[3]);
    printf("Result: 0b%04b (decimal: %u)\n", result128, result128);
    printf("Expected: 0b%04b (decimal: %u)\n", expected128, expected128);
    
    if(result128 == expected128) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    float src256[8] = {-1.0f, 2.0f, -3.0f, 4.0f, -5.0f, 6.0f, -7.0f, 8.0f}; // 负数符号位为1，正数为0
    unsigned int expected256 = 0b01010101; // 修正后的预期掩码值（85）
    
    __m256 vec256 = _mm256_loadu_ps(src256);
    unsigned int result256;
    
    __asm__ __volatile__ (
        "vmovmskps %1, %0" // 执行VMOVMSKPS
        : "=r" (result256)
        : "x" (vec256)
    );
    
    printf("Source: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", 
           src256[0], src256[1], src256[2], src256[3], 
           src256[4], src256[5], src256[6], src256[7]);
    printf("Result: 0b%08b (decimal: %u)\n", result256, result256);
    printf("Expected: 0b%08b (decimal: %u)\n", expected256, expected256);
    
    if(result256 == expected256) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovmskps();
    return 0;
}
