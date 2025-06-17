#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVSXBW 指令测试
void test_vpmovsxbw() {
    printf("=== Testing VPMOVSXBW ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    char src[8] = {-128, -64, 0, 64, 127, -1, -32, 32}; // 有符号字节
    short expected[8] = {-128, -64, 0, 64, 127, -1, -32, 32}; // 预期结果
    short result[8] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载8个字节到xmm0的低64位
        "vpmovsxbw %%xmm0, %%xmm1\n\t" // 执行VPMOVSXBW：将有符号字节扩展为字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(short (*)[8])result)
        : "m" (*(const char (*)[8])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7]);
    printf("Result: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           result[0], result[1], result[2], result[3], 
           result[4], result[5], result[6], result[7]);
    printf("Expected: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           expected[0], expected[1], expected[2], expected[3], 
           expected[4], expected[5], expected[6], expected[7]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    char src256[16] = {
        -128, -64, 0, 64, 127, -1, -32, 32,
        -100, 100, -50, 50, -25, 25, -10, 10
    }; // 有符号字节
    short expected256[16] = {
        -128, -64, 0, 64, 127, -1, -32, 32,
        -100, 100, -50, 50, -25, 25, -10, 10
    }; // 预期结果
    short result256[16] = {0};
    
    __asm__ __volatile__ (
        "vmovdqu %1, %%xmm0\n\t"     // 加载16个字节到xmm0
        "vpmovsxbw %%xmm0, %%ymm1\n\t" // 执行VPMOVSXBW：将有符号字节扩展为字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(short (*)[16])result256)
        : "m" (*(const char (*)[16])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [");
    for(int i=0; i<16; i++) printf(i<15?"%d, ":"%d", src256[i]);
    printf("]\n");
    printf("Result: [");
    for(int i=0; i<16; i++) printf(i<15?"%d, ":"%d", result256[i]);
    printf("]\n");
    printf("Expected: [");
    for(int i=0; i<16; i++) printf(i<15?"%d, ":"%d", expected256[i]);
    printf("]\n");
    
    if(memcmp(result256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vpmovsxbw();
    return 0;
}
