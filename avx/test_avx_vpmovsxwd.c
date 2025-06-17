#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVSXWD 指令测试
void test_vpmovsxwd() {
    printf("=== Testing VPMOVSXWD ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    short src[4] = {-32768, -1, 0, 32767}; // 有符号字
    int expected[4] = {-32768, -1, 0, 32767}; // 预期结果
    int result[4] = {0};
    
    __asm__ __volatile__ (
        "vmovq %1, %%xmm0\n\t"       // 加载4个字到xmm0
        "vpmovsxwd %%xmm0, %%xmm1\n\t" // 执行VPMOVSXWD：将有符号字扩展为双字
        "vmovdqu %%xmm1, %0"         // 存储结果
        : "=m" (*(int (*)[4])result)
        : "m" (*(const short (*)[4])src)
        : "xmm0", "xmm1", "memory"
    );
    
    printf("Source: [%d, %d, %d, %d]\n", src[0], src[1], src[2], src[3]);
    printf("Result: [%d, %d, %d, %d]\n", result[0], result[1], result[2], result[3]);
    printf("Expected: [%d, %d, %d, %d]\n", expected[0], expected[1], expected[2], expected[3]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    short src256[8] = {-32768, -16384, -1, 0, 1, 16383, 32767, 32766}; // 有符号字
    int expected256[8] = {-32768, -16384, -1, 0, 1, 16383, 32767, 32766}; // 预期结果
    int result256[8] = {0};
    
    __asm__ __volatile__ (
        "vmovdqu %1, %%xmm0\n\t"     // 加载8个字到xmm0
        "vpmovsxwd %%xmm0, %%ymm1\n\t" // 执行VPMOVSXWD：将有符号字扩展为双字
        "vmovdqu %%ymm1, %0"         // 存储结果
        : "=m" (*(int (*)[8])result256)
        : "m" (*(const short (*)[8])src256)
        : "xmm0", "ymm1", "memory"
    );
    
    printf("Source: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           src256[0], src256[1], src256[2], src256[3],
           src256[4], src256[5], src256[6], src256[7]);
    printf("Result: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           result256[0], result256[1], result256[2], result256[3],
           result256[4], result256[5], result256[6], result256[7]);
    printf("Expected: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           expected256[0], expected256[1], expected256[2], expected256[3],
           expected256[4], expected256[5], expected256[6], expected256[7]);
    
    if(memcmp(result256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vpmovsxwd();
    return 0;
}
