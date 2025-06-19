#include <stdio.h>
#include <stdint.h>
#include "avx.h"

int main() {
    printf("Testing VPABSD instruction\n");
    printf("=========================\n");

    // 测试1: 128位寄存器操作数 - 混合正负数
    {
        __m128i input = _mm_setr_epi32(
            -2147483648, -1073741824, -1, 0
        );
        
        __m128i result;
        asm volatile(
            "vpabsd %[input], %[result]"
            : [result] "=x" (result)
            : [input] "x" (input));
        
        printf("\nTest 1: 128-bit register operands (mixed positive/negative)\n");
        print_m128i_hex(input, "Input");
        
        // 打印结果
        uint32_t *res_dwords = (uint32_t*)&result;
        printf("Result: ");
        for(int i=0; i<4; i++) {
            printf("%08X ", res_dwords[i]);
        }
        printf("\n");
        
        // 预期结果
        uint32_t expected[4] = {
            2147483648, 1073741824, 1, 0
        };
        
        int pass = 1;
        for(int i=0; i<4; i++) {
            if(res_dwords[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<4; i++) {
                printf("%08X ", expected[i]);
            }
            printf("\n");
        }
    }

    // 测试2: 128位内存操作数 - 边界值
    {
        __m128i input_mem = _mm_setr_epi32(
            2147483647, -2147483648, 0, -1
        );
        
        __m128i result;
        asm volatile(
            "vpabsd %[input], %[result]"
            : [result] "=x" (result)
            : [input] "m" (input_mem));
        
        printf("\nTest 2: 128-bit memory operand (boundary values)\n");
        print_m128i_hex(input_mem, "Input");
        
        // 打印结果
        uint32_t *res_dwords = (uint32_t*)&result;
        printf("Result: ");
        for(int i=0; i<4; i++) {
            printf("%08X ", res_dwords[i]);
        }
        printf("\n");
        
        // 预期结果
        uint32_t expected[4] = {
            2147483647, 2147483648, 0, 1
        };
        
        int pass = 1;
        for(int i=0; i<4; i++) {
            if(res_dwords[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            for(int i=0; i<4; i++) {
                printf("%08X ", expected[i]);
            }
            printf("\n");
        }
    }

    // 测试3: 256位寄存器操作数
    {
        __m256i input = _mm256_setr_epi32(
            -2147483648, -1073741824, -1, 0,
            1, 1073741824, 2147483647, 0
        );
        
        __m256i result;
        asm volatile(
            "vpabsd %[input], %[result]"
            : [result] "=x" (result)
            : [input] "x" (input));
        
        printf("\nTest 3: 256-bit register operands\n");
        print_m256i_hex(input, "Input");
        
        // 打印结果
        uint32_t *res_dwords = (uint32_t*)&result;
        printf("Result (low): ");
        for(int i=0; i<4; i++) {
            printf("%08X ", res_dwords[i]);
        }
        printf("\nResult (high): ");
        for(int i=4; i<8; i++) {
            printf("%08X ", res_dwords[i]);
        }
        printf("\n");
        
        // 预期结果
        uint32_t expected[8] = {
            2147483648, 1073741824, 1, 0,
            1, 1073741824, 2147483647, 0
        };
        
        int pass = 1;
        for(int i=0; i<8; i++) {
            if(res_dwords[i] != expected[i]) pass = 0;
        }
        
        if(pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected (low): ");
            for(int i=0; i<4; i++) {
                printf("%08X ", expected[i]);
            }
            printf("\nExpected (high): ");
            for(int i=4; i<8; i++) {
                printf("%08X ", expected[i]);
            }
            printf("\n");
        }
    }

    printf("\nVPABSD tests completed.\n");
    return 0;
}
