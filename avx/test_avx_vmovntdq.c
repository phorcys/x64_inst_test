#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVNTDQ 指令测试
void test_vmovntdq() {
    printf("=== Testing VMOVNTDQ ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    int src128[4] = {1, 2, 3, 4};
    int dest128[4] __attribute__ ((aligned (16))) = {0}; // 16字节对齐
    int expected128[4] = {1, 2, 3, 4};
    
    __m128i vec128 = _mm_loadu_si128((__m128i*)src128);
    
    // 使用非临时存储指令
    _mm_stream_si128((__m128i*)dest128, vec128);
    
    printf("Source: [%d, %d, %d, %d]\n", src128[0], src128[1], src128[2], src128[3]);
    printf("Result: [%d, %d, %d, %d]\n", dest128[0], dest128[1], dest128[2], dest128[3]);
    printf("Expected: [%d, %d, %d, %d]\n", expected128[0], expected128[1], expected128[2], expected128[3]);
    
    if(memcmp(dest128, expected128, sizeof(expected128)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    int src256[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    int dest256[8] __attribute__ ((aligned (32))) = {0}; // 32字节对齐
    int expected256[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    
    __m256i vec256 = _mm256_loadu_si256((__m256i*)src256);
    
    // 使用非临时存储指令
    _mm256_stream_si256((__m256i*)dest256, vec256);
    
    printf("Source: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           src256[0], src256[1], src256[2], src256[3],
           src256[4], src256[5], src256[6], src256[7]);
    printf("Result: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           dest256[0], dest256[1], dest256[2], dest256[3],
           dest256[4], dest256[5], dest256[6], dest256[7]);
    printf("Expected: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           expected256[0], expected256[1], expected256[2], expected256[3],
           expected256[4], expected256[5], expected256[6], expected256[7]);
    
    if(memcmp(dest256, expected256, sizeof(expected256)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovntdq();
    return 0;
}
