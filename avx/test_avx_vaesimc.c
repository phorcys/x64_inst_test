#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// VAESIMC指令测试函数
void test_vaesimc() {
    printf("----- VAESIMC Test -----\n");
    
    // 测试用例1：Intel官方测试向量
    {
        __m128i input = _mm_set_epi32(0x7b5b5465, 0x73745665, 0x63746f72, 0x5d53475d);
        __m128i result;
        
        // 执行VAESIMC指令
        asm volatile (
            "vaesimc %1, %0"
            : "=x"(result)
            : "x"(input)
        );
        
        // 打印输入和输出
        printf("Test Case 1: Official Vector\n");
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例2：全零输入
    {
        __m128i input = _mm_setzero_si128();
        __m128i result;
        
        asm volatile (
            "vaesimc %1, %0"
            : "=x"(result)
            : "x"(input)
        );
        
        printf("Test Case 2: All Zeros\n");
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例3：全一输入
    {
        __m128i input = _mm_set1_epi8(0xFF);
        __m128i result;
        
        asm volatile (
            "vaesimc %1, %0"
            : "=x"(result)
            : "x"(input)
        );
        
        printf("Test Case 3: All Ones\n");
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例4：随机测试向量1
    {
        __m128i input = _mm_set_epi32(0x12345678, 0x9ABCDEF0, 0x0F1E2D3C, 0x4B5A6978);
        __m128i result;
        
        asm volatile (
            "vaesimc %1, %0"
            : "=x"(result)
            : "x"(input)
        );
        
        printf("Test Case 4: Random Vector 1\n");
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例5：随机测试向量2
    {
        __m128i input = _mm_set_epi32(0x55AA55AA, 0xAA55AA55, 0x5A5A5A5A, 0xA5A5A5A5);
        __m128i result;
        
        asm volatile (
            "vaesimc %1, %0"
            : "=x"(result)
            : "x"(input)
        );
        
        printf("Test Case 5: Random Vector 2\n");
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例6：边界值测试
    {
        __m128i input = _mm_set_epi32(0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA);
        __m128i result;
        
        asm volatile (
            "vaesimc %1, %0"
            : "=x"(result)
            : "x"(input)
        );
        
        printf("Test Case 6: Boundary Values\n");
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
}

int main() {
    test_vaesimc();
    return 0;
}
