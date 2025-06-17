#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVZXBD 指令测试
void test_vpmovzxbd() {
    printf("=== Testing VPMOVZXBD ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    unsigned char src[4] = {0x80, 0x7F, 0x00, 0xFF}; // 无符号字节
    unsigned int expected[4] = {0x80, 0x7F, 0x00, 0xFF}; // 预期结果
    unsigned int result[4] = {0};
    
    __asm__ __volatile__ (
        "vmovd %1, %%xmm0\n\t"       // 加载4个字节到xmm0的低32位
        "vpmovzxbd %%xmm0, %%xmm1\n\t" // 执行VPMOVZXBD：将无符号字节扩展为双字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(unsigned int (*)[4])result)
        : "m" (*(const unsigned char (*)[4])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [0x%02X, 0x%02X, 0x%02X, 0x%02X]\n", 
           src[0], src[1], src[2], src[3]);
    printf("Result: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
           result[0], result[1], result[2], result[3]);
    printf("Expected: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
           expected[0], expected[1], expected[2], expected[3]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    unsigned char src256[8] = {0x80, 0x7F, 0x00, 0xFF, 0x55, 0xAA, 0x01, 0xFE}; // 无符号字节
    unsigned int expected256[8] = {0x80, 0x7F, 0x00, 0xFF, 0x55, 0xAA, 0x01, 0xFE}; // 预期结果
    unsigned int result256[8] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载8个字节到xmm0的低64位
        "vpmovzxbd %%xmm0, %%ymm1\n\t" // 执行VPMOVZXBD：将无符号字节扩展为双字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(unsigned int (*)[8])result256)
        : "m" (*(const unsigned char (*)[8])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%02X, ":"0x%02X", src256[i]);
    printf("]\n");
    printf("Result: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%08X, ":"0x%08X", result256[i]);
    printf("]\n");
    printf("Expected: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%08X, ":"0x%08X", expected256[i]);
    printf("]\n");
    
    if(memcmp(result256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vpmovzxbd();
    return 0;
}
