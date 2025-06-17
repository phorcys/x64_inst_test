#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVZXBW 指令测试
void test_vpmovzxbw() {
    printf("=== Testing VPMOVZXBW ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    unsigned char src[8] = {0x80, 0x7F, 0x00, 0xFF, 0x55, 0xAA, 0x01, 0xFE}; // 无符号字节
    unsigned short expected[8] = {0x80, 0x7F, 0x00, 0xFF, 0x55, 0xAA, 0x01, 0xFE}; // 预期结果
    unsigned short result[8] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载8个字节到xmm0的低64位
        "vpmovzxbw %%xmm0, %%xmm1\n\t" // 执行VPMOVZXBW：将无符号字节扩展为字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(unsigned short (*)[8])result)
        : "m" (*(const unsigned char (*)[8])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%02X, ":"0x%02X", src[i]);
    printf("]\n");
    printf("Result: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%04X, ":"0x%04X", result[i]);
    printf("]\n");
    printf("Expected: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%04X, ":"0x%04X", expected[i]);
    printf("]\n");
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    unsigned char src256[16] = {
        0x80, 0x7F, 0x00, 0xFF, 0x55, 0xAA, 0x01, 0xFE,
        0x00, 0xFF, 0x80, 0x7F, 0xAA, 0x55, 0xFE, 0x01
    }; // 无符号字节
    unsigned short expected256[16] = {
        0x80, 0x7F, 0x00, 0xFF, 0x55, 0xAA, 0x01, 0xFE,
        0x00, 0xFF, 0x80, 0x7F, 0xAA, 0x55, 0xFE, 0x01
    }; // 预期结果
    unsigned short result256[16] = {0};
    
    __asm__ __volatile__ (
        "vmovdqu %1, %%xmm0\n\t"     // 加载16个字节到xmm0
        "vpmovzxbw %%xmm0, %%ymm1\n\t" // 执行VPMOVZXBW：将无符号字节扩展为字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(unsigned short (*)[16])result256)
        : "m" (*(const unsigned char (*)[16])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [");
    for(int i=0; i<16; i++) printf(i<15?"0x%02X, ":"0x%02X", src256[i]);
    printf("]\n");
    printf("Result: [");
    for(int i=0; i<16; i++) printf(i<15?"0x%04X, ":"0x%04X", result256[i]);
    printf("]\n");
    printf("Expected: [");
    for(int i=0; i<16; i++) printf(i<15?"0x%04X, ":"0x%04X", expected256[i]);
    printf("]\n");
    
    if(memcmp(result256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vpmovzxbw();
    return 0;
}
