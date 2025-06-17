#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVZXDQ 指令测试
void test_vpmovzxdq() {
    printf("=== Testing VPMOVZXDQ ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    unsigned int src[2] = {0x80000000, 0x7FFFFFFF}; // 无符号双字
    unsigned long long expected[2] = {0x80000000, 0x7FFFFFFF}; // 预期结果
    unsigned long long result[2] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载2个双字到xmm0的低64位
        "vpmovzxdq %%xmm0, %%xmm1\n\t" // 执行VPMOVZXDQ：将无符号双字扩展为四字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(unsigned long long (*)[2])result)
        : "m" (*(const unsigned int (*)[2])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [0x%08X, 0x%08X]\n", src[0], src[1]);
    printf("Result: [0x%016llX, 0x%016llX]\n", result[0], result[1]);
    printf("Expected: [0x%016llX, 0x%016llX]\n", expected[0], expected[1]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    unsigned int src256[4] = {0x80000000, 0x7FFFFFFF, 0x00000000, 0xFFFFFFFF}; // 无符号双字
    unsigned long long expected256[4] = {0x80000000, 0x7FFFFFFF, 0x00000000, 0xFFFFFFFF}; // 预期结果
    unsigned long long result256[4] = {0};
    
    __asm__ __volatile__ (
        "vmovdqu %1, %%xmm0\n\t"     // 加载4个双字到xmm0
        "vpmovzxdq %%xmm0, %%ymm1\n\t" // 执行VPMOVZXDQ：将无符号双字扩展为四字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(unsigned long long (*)[4])result256)
        : "m" (*(const unsigned int (*)[4])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
           src256[0], src256[1], src256[2], src256[3]);
    printf("Result: [0x%016llX, 0x%016llX, 0x%016llX, 0x%016llX]\n", 
           result256[0], result256[1], result256[2], result256[3]);
    printf("Expected: [0x%016llX, 0x%016llX, 0x%016llX, 0x%016llX]\n", 
           expected256[0], expected256[1], expected256[2], expected256[3]);
    
    if(memcmp(result256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vpmovzxdq();
    return 0;
}
