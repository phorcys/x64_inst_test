#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMASKMOVD 指令测试
void test_vpmaskmovd() {
    printf("=== Testing VPMASKMOVD ===\n");
    
    // 测试1: 128位加载操作
    printf("\nTest 1: 128-bit masked load\n");
    int src[4] = {10, 20, 30, 40};
    int mask[4] = {0, -1, -1, 0}; // 掩码：0表示不加载，-1表示加载
    int result[4] = {0}; // 初始化为0
    int expected[4] = {0, 20, 30, 0}; // 预期结果
    
    __m128i mask_vec = _mm_loadu_si128((__m128i*)mask);
    
    __asm__ __volatile__ (
        "vpmaskmovd %1, %2, %%xmm1\n\t" // 执行VPMASKMOVD加载（根据掩码选择性地加载）
        "vmovdqu %%xmm1, %0"          // 存储结果
        : "=m" (*(int (*)[4])result)
        : "m" (*(const int (*)[4])src), "x" (mask_vec)
        : "xmm1", "memory"
    );
    
    printf("Source: [%d, %d, %d, %d]\n", src[0], src[1], src[2], src[3]);
    printf("Mask:   [%d, %d, %d, %d]\n", mask[0], mask[1], mask[2], mask[3]);
    printf("Result: [%d, %d, %d, %d]\n", result[0], result[1], result[2], result[3]);
    printf("Expected: [%d, %d, %d, %d]\n", expected[0], expected[1], expected[2], expected[3]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位加载操作
    printf("\nTest 2: 256-bit masked load\n");
    int src256[8] = {100, 200, 300, 400, 500, 600, 700, 800};
    int mask256[8] = {-1, 0, 0, -1, 0, -1, -1, 0}; // 掩码
    int result256[8] = {0}; // 初始化为0
    int expected256[8] = {100, 0, 0, 400, 0, 600, 700, 0}; // 预期结果
    
    __m256i mask_vec256 = _mm256_loadu_si256((__m256i*)mask256);
    
    __asm__ __volatile__ (
        "vpmaskmovd %1, %2, %%ymm1\n\t" // 执行VPMASKMOVD加载
        "vmovdqu %%ymm1, %0"          // 存储结果
        : "=m" (*(int (*)[8])result256)
        : "m" (*(const int (*)[8])src256), "x" (mask_vec256)
        : "ymm1", "memory"
    );
    
    printf("Source: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           src256[0], src256[1], src256[2], src256[3],
           src256[4], src256[5], src256[6], src256[7]);
    printf("Mask:   [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           mask256[0], mask256[1], mask256[2], mask256[3],
           mask256[4], mask256[5], mask256[6], mask256[7]);
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
    test_vpmaskmovd();
    return 0;
}
