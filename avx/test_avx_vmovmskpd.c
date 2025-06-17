#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVMSKPD 指令测试
void test_vmovmskpd() {
    printf("=== Testing VMOVMSKPD ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    double src128[2] = {-1.0, 2.0}; // 负数符号位为1，正数为0
    unsigned int expected128 = 0b01; // 预期掩码值
    
    __m128d vec128 = _mm_loadu_pd(src128);
    unsigned int result128;
    
    __asm__ __volatile__ (
        "vmovmskpd %1, %0" // 执行VMOVMSKPD
        : "=r" (result128)
        : "x" (vec128)
    );
    
    printf("Source: [%.1f, %.1f]\n", src128[0], src128[1]);
    printf("Result: 0b%02b (decimal: %u)\n", result128, result128);
    printf("Expected: 0b%02b (decimal: %u)\n", expected128, expected128);
    
    if(result128 == expected128) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    double src256[4] = {-1.0, 2.0, -3.0, 4.0}; // 负数符号位为1，正数为0
    unsigned int expected256 = 0b0101; // 预期掩码值
    
    __m256d vec256 = _mm256_loadu_pd(src256);
    unsigned int result256;
    
    __asm__ __volatile__ (
        "vmovmskpd %1, %0" // 执行VMOVMSKPD
        : "=r" (result256)
        : "x" (vec256)
    );
    
    printf("Source: [%.1f, %.1f, %.1f, %.1f]\n", 
           src256[0], src256[1], src256[2], src256[3]);
    printf("Result: 0b%04b (decimal: %u)\n", result256, result256);
    printf("Expected: 0b%04b (decimal: %u)\n", expected256, expected256);
    
    if(result256 == expected256) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovmskpd();
    return 0;
}
