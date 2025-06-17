#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VPMOVMSKB 指令测试
void test_vpmovmskb() {
    printf("=== Testing VPMOVMSKB ===\n");
    
    // 测试1: 128位操作数
    printf("\nTest 1: 128-bit operand\n");
    unsigned char src[16] = {
        0x80, 0x00, 0x80, 0x00,  // 最高位设置：1,0,1,0
        0x80, 0x00, 0x80, 0x00, 
        0x80, 0x00, 0x80, 0x00, 
        0x80, 0x00, 0x80, 0x00
    };
    unsigned int result;
    unsigned int expected = 0x5555; // 0101010101010101
    
    __m128i vec = _mm_loadu_si128((__m128i*)src);
    
    __asm__ __volatile__ (
        "vpmovmskb %1, %0" // 执行VPMOVMSKB
        : "=r" (result)
        : "x" (vec)
    );
    
    printf("Source: [");
    for(int i=0; i<16; i++) printf(i<15?"0x%02X, ":"0x%02X", src[i]);
    printf("]\n");
    printf("Result: 0x%04X\n", result);
    printf("Expected: 0x%04X\n", expected);
    
    if(result == expected) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位操作数
    printf("\nTest 2: 256-bit operand\n");
    unsigned char src256[32] = {
        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00
    };
    unsigned int result256;
    unsigned int expected256 = 0x55555555; // 01010101010101010101010101010101
    
    __m256i vec256 = _mm256_loadu_si256((__m256i*)src256);
    
    __asm__ __volatile__ (
        "vpmovmskb %1, %0" // 执行VPMOVMSKB
        : "=r" (result256)
        : "x" (vec256)
    );
    
    printf("Source: [");
    for(int i=0; i<32; i++) printf(i<31?"0x%02X, ":"0x%02X", src256[i]);
    printf("]\n");
    printf("Result: 0x%08X\n", result256);
    printf("Expected: 0x%08X\n", expected256);
    
    if(result256 == expected256) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
    
    // 测试3: 混合模式
    printf("\nTest 3: Mixed pattern\n");
    unsigned char src_mixed[16] = {
        0xFF, 0x00, 0xFF, 0x00, 
        0xFF, 0x00, 0xFF, 0x00, 
        0xFF, 0x00, 0xFF, 0x00, 
        0xFF, 0x00, 0xFF, 0x00
    };
    unsigned int result_mixed;
    unsigned int expected_mixed = 0x5555; // 0101010101010101
    
    __m128i vec_mixed = _mm_loadu_si128((__m128i*)src_mixed);
    
    __asm__ __volatile__ (
        "vpmovmskb %1, %0" // 执行VPMOVMSKB
        : "=r" (result_mixed)
        : "x" (vec_mixed)
    );
    
    printf("Source: [");
    for(int i=0; i<16; i++) printf(i<15?"0x%02X, ":"0x%02X", src_mixed[i]);
    printf("]\n");
    printf("Result: 0x%04X\n", result_mixed);
    printf("Expected: 0x%04X\n", expected_mixed);
    
    if(result_mixed == expected_mixed) {
        printf("Test 3 PASSED\n");
    } else {
        printf("Test 3 FAILED\n");
    }
}

int main() {
    test_vpmovmskb();
    return 0;
}
