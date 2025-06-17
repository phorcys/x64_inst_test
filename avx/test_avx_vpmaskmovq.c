#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMASKMOVQ 指令测试
void test_vpmaskmovq() {
    printf("=== Testing VPMASKMOVQ ===\n");
    
    // 测试1: 128位加载操作
    printf("\nTest 1: 128-bit masked load\n");
    long long src[2] = {10, 20};
    long long mask[2] = {0, -1}; // 掩码：0表示不加载，-1表示加载
    long long result[2] = {0}; // 初始化为0
    long long expected[2] = {0, 20}; // 预期结果
    
    __m128i mask_vec = _mm_loadu_si128((__m128i*)mask);
    
    __asm__ __volatile__ (
        "vpmaskmovq %1, %2, %%xmm1\n\t" // 执行VPMASKMOVQ加载
        "vmovdqu %%xmm1, %0"          // 存储结果
        : "=m" (*(long long (*)[2])result)
        : "m" (*(const long long (*)[2])src), "x" (mask_vec)
        : "xmm1", "memory"
    );
    
    printf("Source: [%lld, %lld]\n", src[0], src[1]);
    printf("Mask:   [%lld, %lld]\n", mask[0], mask[1]);
    printf("Result: [%lld, %lld]\n", result[0], result[1]);
    printf("Expected: [%lld, %lld]\n", expected[0], expected[1]);
    
    if(memcmp(result, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位加载操作
    printf("\nTest 2: 256-bit masked load\n");
    long long src256[4] = {100, 200, 300, 400};
    long long mask256[4] = {-1, 0, -1, 0}; // 掩码
    long long result256[4] = {0}; // 初始化为0
    long long expected256[4] = {100, 0, 300, 0}; // 预期结果
    
    __m256i mask_vec256 = _mm256_loadu_si256((__m256i*)mask256);
    
    __asm__ __volatile__ (
        "vpmaskmovq %1, %2, %%ymm1\n\t" // 执行VPMASKMOVQ加载
        "vmovdqu %%ymm1, %0"          // 存储结果
        : "=m" (*(long long (*)[4])result256)
        : "m" (*(const long long (*)[4])src256), "x" (mask_vec256)
        : "ymm1", "memory"
    );
    
    printf("Source: [%lld, %lld, %lld, %lld]\n", 
           src256[0], src256[1], src256[2], src256[3]);
    printf("Mask:   [%lld, %lld, %lld, %lld]\n", 
           mask256[0], mask256[1], mask256[2], mask256[3]);
    printf("Result: [%lld, %lld, %lld, %lld]\n", 
           result256[0], result256[1], result256[2], result256[3]);
    printf("Expected: [%lld, %lld, %lld, %lld]\n", 
           expected256[0], expected256[1], expected256[2], expected256[3]);
    
    if(memcmp(result256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vpmaskmovq();
    return 0;
}
