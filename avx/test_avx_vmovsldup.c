#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVSLDUP 指令测试
void test_vmovsldup() {
    printf("=== Testing VMOVSLDUP ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    float src128[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float expected128[4] = {1.0f, 1.0f, 3.0f, 3.0f};
    float result128[4] = {0};
    
    __asm__ __volatile__ (
        "vmovups %1, %%xmm0\n\t"      // 加载向量到xmm0
        "vmovsldup %%xmm0, %%xmm1\n\t" // 执行VMOVSLDUP
        "vmovups %%xmm1, %0"          // 存储结果
        : "=m" (*(float (*)[4])result128)
        : "m" (*(const float (*)[4])src128)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [%.1f, %.1f, %.1f, %.1f]\n", src128[0], src128[1], src128[2], src128[3]);
    printf("Result: [%.1f, %.1f, %.1f, %.1f]\n", result128[0], result128[1], result128[2], result128[3]);
    printf("Expected: [%.1f, %.1f, %.1f, %.1f]\n", expected128[0], expected128[1], expected128[2], expected128[3]);
    
    if(memcmp(result128, expected128, sizeof(result128)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    float src256[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float expected256[8] = {1.0f, 1.0f, 3.0f, 3.0f, 5.0f, 5.0f, 7.0f, 7.0f};
    float result256[8] = {0};
    
    __asm__ __volatile__ (
        "vmovups %1, %%ymm0\n\t"      // 加载向量到ymm0
        "vmovsldup %%ymm0, %%ymm1\n\t" // 执行VMOVSLDUP
        "vmovups %%ymm1, %0"          // 存储结果
        : "=m" (*(float (*)[8])result256)
        : "m" (*(const float (*)[8])src256)
        : "ymm0", "ymm1", "memory"
    );
    
    printf("Source: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", 
           src256[0], src256[1], src256[2], src256[3],
           src256[4], src256[5], src256[6], src256[7]);
    printf("Result: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", 
           result256[0], result256[1], result256[2], result256[3],
           result256[4], result256[5], result256[6], result256[7]);
    printf("Expected: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", 
           expected256[0], expected256[1], expected256[2], expected256[3],
           expected256[4], expected256[5], expected256[6], expected256[7]);
    
    if(memcmp(result256, expected256, sizeof(result256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovsldup();
    return 0;
}
