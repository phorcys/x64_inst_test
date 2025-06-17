#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVZXWD 指令测试
void test_vpmovzxwd() {
    printf("=== Testing VPMOVZXWD ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    unsigned short src[4] = {0x8000, 0x7FFF, 0x0000, 0xFFFF}; // 无符号字
    unsigned int expected[4] = {0x8000, 0x7FFF, 0x0000, 0xFFFF}; // 预期结果
    unsigned int result[4] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载4个字到xmm0的低64位
        "vpmovzxwd %%xmm0, %%xmm1\n\t" // 执行VPMOVZXWD：将无符号字扩展为双字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(unsigned int (*)[4])result)
        : "m" (*(const unsigned short (*)[4])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [0x%04X, 0x%04X, 0x%04X, 0x%04X]\n", 
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
    unsigned short src256[8] = {
        0x8000, 0x7FFF, 0x0000, 0xFFFF, 
        0x5555, 0xAAAA, 0x0001, 0xFFFE
    }; // 无符号字
    unsigned int expected256[8] = {
        0x8000, 0x7FFF, 0x0000, 0xFFFF,
        0x5555, 0xAAAA, 0x0001, 0xFFFE
    }; // 预期结果
    unsigned int result256[8] = {0};
    
    __asm__ __volatile__ (
        "vmovdqu %1, %%xmm0\n\t"     // 加载8个字到xmm0
        "vpmovzxwd %%xmm0, %%ymm1\n\t" // 执行VPMOVZXWD：将无符号字扩展为双字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(unsigned int (*)[8])result256)
        : "m" (*(const unsigned short (*)[8])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [");
    for(int i=0; i<8; i++) printf(i<7?"0x%04X, ":"0x%04X", src256[i]);
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
    test_vpmovzxwd();
    return 0;
}
