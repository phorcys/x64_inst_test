#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVZXWQ 指令测试
void test_vpmovzxwq() {
    printf("=== Testing VPMOVZXWQ ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    unsigned short src[2] = {0x8000, 0x7FFF}; // 无符号字
    unsigned long long expected[2] = {0x8000, 0x7FFF}; // 预期结果
    unsigned long long result[2] = {0};
    
    __asm__ __volatile__ (
        "vmovd %1, %%xmm0\n\t"       // 加载2个字到xmm0的低32位
        "vpmovzxwq %%xmm0, %%xmm1\n\t" // 执行VPMOVZXWQ：将无符号字扩展为四字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(unsigned long long (*)[2])result)
        : "m" (*(const unsigned short (*)[2])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [0x%04X, 0x%04X]\n", src[0], src[1]);
    printf("Result: [0x%016llX, 0x%016llX]\n", result[0], result[1]);
    printf("Expected: [0x%016llX, 0x%016llX]\n", expected[0], expected[1]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    unsigned short src256[4] = {0x8000, 0x7FFF, 0x0000, 0xFFFF}; // 无符号字
    unsigned long long expected256[4] = {0x8000, 0x7FFF, 0x0000, 0xFFFF}; // 预期结果
    unsigned long long result256[4] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载4个字到xmm0的低64位
        "vpmovzxwq %%xmm0, %%ymm1\n\t" // 执行VPMOVZXWQ：将无符号字扩展为四字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(unsigned long long (*)[4])result256)
        : "m" (*(const unsigned short (*)[4])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [0x%04X, 0x%04X, 0x%04X, 0x%04X]\n", 
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
    test_vpmovzxwq();
    return 0;
}
