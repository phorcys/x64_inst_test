#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// VAESENCLAST指令测试函数
void test_vaesenclast() {
    printf("----- VAESENCLAST Test -----\n");
    
    // 测试用例1：Intel官方测试向量
    {
        // 输入状态：0x7b5b54657374566563746f725d53475d (大端序)
        __m128i state = _mm_set_epi8(
            0x7b, 0x5b, 0x54, 0x65, 0x73, 0x74, 0x56, 0x65, 
            0x63, 0x74, 0x6f, 0x72, 0x5d, 0x53, 0x47, 0x5d
        );
        // 轮密钥：0x48692853686179295d477565726f6e5d (大端序)
        __m128i round_key = _mm_set_epi8(
            0x48, 0x69, 0x28, 0x53, 0x68, 0x61, 0x79, 0x29, 
            0x5d, 0x47, 0x75, 0x65, 0x72, 0x6f, 0x6e, 0x5d
        );
        __m128i result;
        
        // 执行VAESENCLAST指令
        asm volatile (
            "vaesenclast %2, %1, %0"
            : "=x"(result)
            : "x"(state), "x"(round_key)
        );
        
        // 打印输入和输出
        printf("Test Case 1: Official Vector\n");
        print_m128i_hex(state, "Input State");
        print_m128i_hex(round_key, "Round Key");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例2：全零输入
    {
        __m128i state = _mm_setzero_si128();
        __m128i round_key = _mm_setzero_si128();
        __m128i result;
        
        asm volatile (
            "vaesenclast %2, %1, %0"
            : "=x"(result)
            : "x"(state), "x"(round_key)
        );
        
        printf("Test Case 2: All Zeros\n");
        print_m128i_hex(state, "Input State");
        print_m128i_hex(round_key, "Round Key");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例3：全一输入
    {
        __m128i state = _mm_set1_epi8(0xFF);
        __m128i round_key = _mm_set1_epi8(0xFF);
        __m128i result;
        
        asm volatile (
            "vaesenclast %2, %1, %0"
            : "=x"(result)
            : "x"(state), "x"(round_key)
        );
        
        printf("Test Case 3: All Ones\n");
        print_m128i_hex(state, "Input State");
        print_m128i_hex(round_key, "Round Key");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例4：随机测试向量1
    {
        __m128i state = _mm_set_epi32(0x12345678, 0x9ABCDEF0, 0x0F1E2D3C, 0x4B5A6978);
        __m128i round_key = _mm_set_epi32(0x87654321, 0xFEDCBA09, 0x1A2B3C4D, 0x5E6F7A8B);
        __m128i result;
        
        asm volatile (
            "vaesenclast %2, %1, %0"
            : "=x"(result)
            : "x"(state), "x"(round_key)
        );
        
        printf("Test Case 4: Random Vector 1\n");
        print_m128i_hex(state, "Input State");
        print_m128i_hex(round_key, "Round Key");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例5：随机测试向量2
    {
        __m128i state = _mm_set_epi32(0x55AA55AA, 0xAA55AA55, 0x5A5A5A5A, 0xA5A5A5A5);
        __m128i round_key = _mm_set_epi32(0x33CC33CC, 0xCC33CC33, 0x3C3C3C3C, 0xC3C3C3C3);
        __m128i result;
        
        asm volatile (
            "vaesenclast %2, %1, %0"
            : "=x"(result)
            : "x"(state), "x"(round_key)
        );
        
        printf("Test Case 5: Random Vector 2\n");
        print_m128i_hex(state, "Input State");
        print_m128i_hex(round_key, "Round Key");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例6：边界值测试
    {
        __m128i state = _mm_set_epi32(0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA);
        __m128i round_key = _mm_set_epi32(0xAAAAAAAA, 0x55555555, 0x00000000, 0xFFFFFFFF);
        __m128i result;
        
        asm volatile (
            "vaesenclast %2, %1, %0"
            : "=x"(result)
            : "x"(state), "x"(round_key)
        );
        
        printf("Test Case 6: Boundary Values\n");
        print_m128i_hex(state, "Input State");
        print_m128i_hex(round_key, "Round Key");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
}

int main() {
    test_vaesenclast();
    return 0;
}
