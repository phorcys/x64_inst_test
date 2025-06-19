#include <stdio.h>
#include <stdint.h>
#include "avx.h"

int main() {
    printf("Testing VPABSB instruction\n");
    printf("=========================\n");

    // 测试1: 128位寄存器操作数 - 混合正负数
    {
        __m128i input = _mm_setr_epi8(
            -128, -64, -32, -16, -8, -4, -2, -1,
            0, 1, 2, 4, 8, 16, 32, 64
        );
        
        __m128i result;
        asm volatile(
            "vpabsb %[input], %[result]"
            : [result] "=x" (result)
            : [input] "x" (input));
        
        printf("\nTest 1: 128-bit register operands (mixed positive/negative)\n");
        print_m128i_hex(input, "Input");
        
        // 打印结果
        int8_t *res_bytes = (int8_t*)&result;
        printf("Result: ");
        for(int i=0; i<16; i++) {
            printf("%02X ", (uint8_t)res_bytes[i]);
        }
        printf("\n");
        
        // 预期结果
        int8_t expected[16] = {
            128, 64, 32, 16, 8, 4, 2, 1,
            0, 1, 2, 4, 8, 16, 32, 64
        };
        
        int pass = 1;
        for(int i=0; i<16; i++) {
            if(res_bytes[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<16; i++) {
                printf("%02X ", (uint8_t)expected[i]);
            }
            printf("\n");
        }
    }

    // 测试2: 128位内存操作数 - 边界值
    {
        __m128i input_mem = _mm_setr_epi8(
            127, -127, 0, -1, 1, -2, 2, -128,
            64, -64, 32, -32, 16, -16, 8, -8
        );
        
        __m128i result;
        asm volatile(
            "vpabsb %[input], %[result]"
            : [result] "=x" (result)
            : [input] "m" (input_mem));
        
        printf("\nTest 2: 128-bit memory operand (boundary values)\n");
        print_m128i_hex(input_mem, "Input");
        
        // 打印结果
        int8_t *res_bytes = (int8_t*)&result;
        printf("Result: ");
        for(int i=0; i<16; i++) {
            printf("%02X ", (uint8_t)res_bytes[i]);
        }
        printf("\n");
        
        // 预期结果
        int8_t expected[16] = {
            127, 127, 0, 1, 1, 2, 2, 128,
            64, 64, 32, 32, 16, 16, 8, 8
        };
        
        int pass = 1;
        for(int i=0; i<16; i++) {
            if(res_bytes[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<16; i++) {
                printf("%02X ", (uint8_t)expected[i]);
            }
            printf("\n");
        }
    }

    // 测试3: 256位寄存器操作数
    {
        __m256i input = _mm256_setr_epi8(
            -128, -64, -32, -16, -8, -4, -2, -1,
            0, 1, 2, 4, 8, 16, 32, 64,
            127, -127, 0, -1, 1, -2, 2, -128,
            64, -64, 32, -32, 16, -16, 8, -8
        );
        
        __m256i result;
        asm volatile(
            "vpabsb %[input], %[result]"
            : [result] "=x" (result)
            : [input] "x" (input));
        
        printf("\nTest 3: 256-bit register operands\n");
        print_m256i_hex(input, "Input");
        
        // 打印结果
        int8_t *res_bytes = (int8_t*)&result;
        printf("Result (low): ");
        for(int i=0; i<16; i++) {
            printf("%02X ", (uint8_t)res_bytes[i]);
        }
        printf("\nResult (high): ");
        for(int i=16; i<32; i++) {
            printf("%02X ", (uint8_t)res_bytes[i]);
        }
        printf("\n");
        
        // 预期结果
        int8_t expected[32] = {
            128, 64, 32, 16, 8, 4, 2, 1,
            0, 1, 2, 4, 8, 16, 32, 64,
            127, 127, 0, 1, 1, 2, 2, 128,
            64, 64, 32, 32, 16, 16, 8, 8
        };
        
        int pass = 1;
        for(int i=0; i<32; i++) {
            if(res_bytes[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected (low): ");
            for(int i=0; i<16; i++) {
                printf("%02X ", (uint8_t)expected[i]);
            }
            printf("\nExpected (high): ");
            for(int i=16; i<32; i++) {
                printf("%02X ", (uint8_t)expected[i]);
            }
            printf("\n");
        }
    }

    printf("\nVPABSB tests completed.\n");
    return 0;
}
