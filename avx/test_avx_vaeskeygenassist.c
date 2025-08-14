#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// VAESKEYGENASSIST指令测试函数
void test_vaeskeygenassist() {
    printf("----- VAESKEYGENASSIST Test -----\n");
    
    // 测试用例1：Intel官方测试向量
    {
        // 输入：0x000102030405060708090a0b0c0d0e0f (大端序)
        __m128i input = _mm_set_epi8(
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        );
        __m128i result;
        const int imm = 0x01;  // 轮常数
        
        // 执行VAESKEYGENASSIST指令
        asm volatile (
            "vaeskeygenassist %2, %1, %0"
            : "=x"(result)
            : "x"(input), "i"(imm)
        );
        
        // 打印输入和输出
        printf("Test Case 1: Official Vector (imm=0x%02X)\n", imm);
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例2：全零输入
    {
        __m128i input = _mm_setzero_si128();
        __m128i result;
        const int imm = 0x00;
        
        asm volatile (
            "vaeskeygenassist %2, %1, %0"
            : "=x"(result)
            : "x"(input), "i"(imm)
        );
        
        printf("Test Case 2: All Zeros (imm=0x%02X)\n", imm);
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例3：全一输入
    {
        __m128i input = _mm_set1_epi8(0xFF);
        __m128i result;
        const int imm = 0xFF;
        
        asm volatile (
            "vaeskeygenassist %2, %1, %0"
            : "=x"(result)
            : "x"(input), "i"(imm)
        );
        
        printf("Test Case 3: All Ones (imm=0x%02X)\n", imm);
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例4：随机测试向量1
    {
        __m128i input = _mm_set_epi32(0x12345678, 0x9ABCDEF0, 0x0F1E2D3C, 0x4B5A6978);
        __m128i result;
        const int imm = 0x0A;
        
        asm volatile (
            "vaeskeygenassist %2, %1, %0"
            : "=x"(result)
            : "x"(input), "i"(imm)
        );
        
        printf("Test Case 4: Random Vector 1 (imm=0x%02X)\n", imm);
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例5：随机测试向量2
    {
        __m128i input = _mm_set_epi32(0x55AA55AA, 0xAA55AA55, 0x5A5A5A5A, 0xA5A5A5A5);
        __m128i result;
        const int imm = 0x55;
        
        asm volatile (
            "vaeskeygenassist %2, %1, %0"
            : "=x"(result)
            : "x"(input), "i"(imm)
        );
        
        printf("Test Case 5: Random Vector 2 (imm=0x%02X)\n", imm);
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例6：边界值测试
    {
        __m128i input = _mm_set_epi32(0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA);
        __m128i result;
        const int imm = 0x80;
        
        asm volatile (
            "vaeskeygenassist %2, %1, %0"
            : "=x"(result)
            : "x"(input), "i"(imm)
        );
        
        printf("Test Case 6: Boundary Values (imm=0x%02X)\n", imm);
        print_m128i_hex(input, "Input");
        print_m128i_hex(result, "Result");
        printf("\n");
    }
    
    // 测试用例7：不同轮常数测试
    // 注意：由于内联汇编要求立即数为编译时常量，我们将手动展开循环
    {
        __m128i input = _mm_set_epi32(0x12345678, 0x9ABCDEF0, 0x0F1E2D3C, 0x4B5A6978);
        __m128i result;
        
        // 为每个立即数创建单独的测试
        for (int i = 0; i < 10; i++) {
            switch (i) {
                case 0:
                    asm volatile ("vaeskeygenassist $0, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 1:
                    asm volatile ("vaeskeygenassist $1, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 2:
                    asm volatile ("vaeskeygenassist $2, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 3:
                    asm volatile ("vaeskeygenassist $3, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 4:
                    asm volatile ("vaeskeygenassist $4, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 5:
                    asm volatile ("vaeskeygenassist $5, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 6:
                    asm volatile ("vaeskeygenassist $6, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 7:
                    asm volatile ("vaeskeygenassist $7, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 8:
                    asm volatile ("vaeskeygenassist $8, %1, %0" : "=x"(result) : "x"(input));
                    break;
                case 9:
                    asm volatile ("vaeskeygenassist $9, %1, %0" : "=x"(result) : "x"(input));
                    break;
            }
            
            printf("Test Case 7.%d: Round Constant Test (imm=0x%02X)\n", i, i);
            print_m128i_hex(input, "Input");
            print_m128i_hex(result, "Result");
            printf("\n");
        }
    }
}

int main() {
    test_vaeskeygenassist();
    return 0;
}
