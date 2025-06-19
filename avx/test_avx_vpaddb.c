#include <stdio.h>
#include <stdint.h>
#include "avx.h"

int main() {
    printf("Testing VPADDB instruction\n");
    printf("=========================\n");

    // 测试1: 128位寄存器操作数 - 正常加法
    {
        __m128i a = _mm_setr_epi8(
            1, 2, 3, 4, 5, 6, 7, 8,
            0x7F, 0x80, 0xFF, 0x00, 0xFE, 0x01, 0x7E, 0x81
        );
        __m128i b = _mm_setr_epi8(
            9, 10, 11, 12, 13, 14, 15, 16,
            0x01, 0x7F, 0x01, 0xFF, 0x02, 0xFE, 0x01, 0x7F
        );
        
        __m128i result;
        asm volatile(
            "vpaddb %2, %1, %0"
            : "=x" (result)
            : "x" (a), "x" (b));
        
        printf("\nTest 1: 128-bit register operands (normal addition)\n");
        printf("Operand A: ");
        print_m128i_bytes(a);
        printf("Operand B: ");
        print_m128i_bytes(b);
        
        // 打印结果
        printf("Result: ");
        print_m128i_bytes(result);
        
        // 预期结果
        uint8_t expected[16] = {
            10, 12, 14, 16, 18, 20, 22, 24,
            0x80, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x7F, 0x00
        };
        
        int pass = 1;
        uint8_t *res_bytes = (uint8_t*)&result;
        for(int i=0; i<16; i++) {
            if(res_bytes[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<16; i++) {
                printf("%02X ", expected[i]);
            }
            printf("\n");
        }
    }

    // 测试2: 128位内存操作数 - 边界值
    {
        __m128i a = _mm_setr_epi8(
            0xFF, 0x00, 0x80, 0x7F,
            0x01, 0xFE, 0x7F, 0x80,
            0xFF, 0x00, 0x80, 0x7F,
            0x01, 0xFE, 0x7F, 0x80
        );
        __m128i b_mem = _mm_setr_epi8(
            0x01, 0xFF, 0x7F, 0x80,
            0xFF, 0x01, 0x80, 0x7F,
            0x00, 0xFF, 0x7F, 0x80,
            0xFF, 0x01, 0x80, 0x7F
        );
        
        __m128i result;
        asm volatile(
            "vpaddb %2, %1, %0"
            : "=x" (result)
            : "x" (a), "m" (b_mem));
        
        printf("\nTest 2: 128-bit memory operand (boundary values)\n");
        printf("Operand A: ");
        print_m128i_bytes(a);
        printf("Operand B: ");
        print_m128i_bytes(b_mem);
        
        // 打印结果
        printf("Result: ");
        print_m128i_bytes(result);
        
        // 预期结果
        uint8_t expected[16] = {
            0x00, 0xFF, 0xFF, 0xFF,
            0x00, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF,
            0x00, 0xFF, 0xFF, 0xFF
        };
        
        int pass = 1;
        uint8_t *res_bytes = (uint8_t*)&result;
        for(int i=0; i<16; i++) {
            if(res_bytes[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<16; i++) {
                printf("%02X ", expected[i]);
            }
            printf("\n");
        }
    }

    // 测试3: 256位寄存器操作数
    {
        __m256i a = _mm256_setr_epi8(
            1, 2, 3, 4, 5, 6, 7, 8,
            0x7F, 0x80, 0xFF, 0x00, 0xFE, 0x01, 0x7E, 0x81,
            0xFF, 0x00, 0x80, 0x7F, 0x01, 0xFE, 0x7F, 0x80,
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
        );
        __m256i b = _mm256_setr_epi8(
            9, 10, 11, 12, 13, 14, 15, 16,
            0x01, 0x7F, 0x01, 0xFF, 0x02, 0xFE, 0x01, 0x7F,
            0x01, 0xFF, 0x7F, 0x80, 0xFF, 0x01, 0x80, 0x7F,
            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
        );
        
        __m256i result;
        asm volatile(
            "vpaddb %2, %1, %0"
            : "=x" (result)
            : "x" (a), "x" (b));
        
        printf("\nTest 3: 256-bit register operands\n");
        printf("Operand A (low): ");
        print_m128i_bytes(_mm256_extractf128_si256(a, 0));
        printf("Operand A (high): ");
        print_m128i_bytes(_mm256_extractf128_si256(a, 1));
        printf("Operand B (low): ");
        print_m128i_bytes(_mm256_extractf128_si256(b, 0));
        printf("Operand B (high): ");
        print_m128i_bytes(_mm256_extractf128_si256(b, 1));
        
        // 打印结果
        printf("Result (low): ");
        print_m128i_bytes(_mm256_extractf128_si256(result, 0));
        printf("Result (high): ");
        print_m128i_bytes(_mm256_extractf128_si256(result, 1));
        
        // 预期结果
        uint8_t expected[32] = {
            10, 12, 14, 16, 18, 20, 22, 24,
            0x80, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x7F, 0x00,
            0x00, 0xFF, 0xFF, 0xFF,
            0x00, 0xFF, 0xFF, 0xFF,
            10, 12, 14, 16, 18, 20, 22, 24
        };
        
        int pass = 1;
        uint8_t *res_bytes = (uint8_t*)&result;
        for(int i=0; i<32; i++) {
            if(res_bytes[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected (low): ");
            for(int i=0; i<16; i++) {
                printf("%02X ", expected[i]);
            }
            printf("\nExpected (high): ");
            for(int i=16; i<32; i++) {
                printf("%02X ", expected[i]);
            }
            printf("\n");
        }
    }

    printf("\nVPADDB tests completed.\n");
    return 0;
}
