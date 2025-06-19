#include <stdio.h>
#include <stdint.h>
#include "avx.h"

int main() {
    printf("Testing VPABSW instruction\n");
    printf("=========================\n");

    // 测试1: 128位寄存器操作数 - 混合正负数
    {
        __m128i input = _mm_setr_epi16(
            -32768, -16384, -1, 0,
            1, 16383, 32767, 0
        );
        
        __m128i result;
        asm volatile(
            "vpabsw %[input], %[result]"
            : [result] "=x" (result)
            : [input] "x" (input));
        
        printf("\nTest 1: 128-bit register operands (mixed positive/negative)\n");
        print_m128i_hex(input, "Input");
        
        // 打印结果
        uint16_t *res_words = (uint16_t*)&result;
        printf("Result: ");
        for(int i=0; i<8; i++) {
            printf("%04X ", res_words[i]);
        }
        printf("\n");
        
        // 预期结果
        uint16_t expected[8] = {
            32768, 16384, 1, 0,
            1, 16383, 32767, 0
        };
        
        int pass = 1;
        for(int i=0; i<8; i++) {
            if(res_words[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<8; i++) {
                printf("%04X ", expected[i]);
            }
            printf("\n");
        }
    }

    // 测试2: 128位内存操作数 - 边界值
    {
        __m128i input_mem = _mm_setr_epi16(
            32767, -32768, 0, -1,
            1, -16384, 16383, 0
        );
        
        __m128i result;
        asm volatile(
            "vpabsw %[input], %[result]"
            : [result] "=x" (result)
            : [input] "m" (input_mem));
        
        printf("\nTest 2: 128-bit memory operand (boundary values)\n");
        print_m128i_hex(input_mem, "Input");
        
        // 打印结果
        uint16_t *res_words = (uint16_t*)&result;
        printf("Result: ");
        for(int i=0; i<8; i++) {
            printf("%04X ", res_words[i]);
        }
        printf("\n");
        
        // 预期结果
        uint16_t expected[8] = {
            32767, 32768, 0, 1,
            1, 16384, 16383, 0
        };
        
        int pass = 1;
        for(int i=0; i<8; i++) {
            if(res_words[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<8; i++) {
                printf("%04X ", expected[i]);
            }
            printf("\n");
        }
    }

    // 测试3: 256位寄存器操作数
    {
        __m256i input = _mm256_setr_epi16(
            -32768, -16384, -1, 0,
            1, 16383, 32767, 0,
            32767, -32768, 0, -1,
            1, -16384, 16383, 0
        );
        
        __m256i result;
        asm volatile(
            "vpabsw %[input], %[result]"
            : [result] "=x" (result)
            : [input] "x" (input));
        
        printf("\nTest 3: 256-bit register operands\n");
        print_m256i_hex(input, "Input");
        
        // 打印结果
        uint16_t *res_words = (uint16_t*)&result;
        printf("Result (low): ");
        for(int i=0; i<8; i++) {
            printf("%04X ", res_words[i]);
        }
        printf("\nResult (high): ");
        for(int i=8; i<16; i++) {
            printf("%04X ", res_words[i]);
        }
        printf("\n");
        
        // 预期结果
        uint16_t expected[16] = {
            32768, 16384, 1, 0,
            1, 16383, 32767, 0,
            32767, 32768, 0, 1,
            1, 16384, 16383, 0
        };
        
        int pass = 1;
        for(int i=0; i<16; i++) {
            if(res_words[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected (low): ");
            for(int i=0; i<8; i++) {
                printf("%04X ", expected[i]);
            }
            printf("\nExpected (high): ");
            for(int i=8; i<16; i++) {
                printf("%04X ", expected[i]);
            }
            printf("\n");
        }
    }

    printf("\nVPABSW tests completed.\n");
    return 0;
}
