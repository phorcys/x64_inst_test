#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// 打印128位向量的8位无符号整数元素
static inline void print_m128i_ubytes(__m128i value, const char* name) {
    printf("%s: ", name);
    uint8_t *v = (uint8_t*)&value;
    for(int i=0; i<16; i++) {
        printf("%u ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDUSB instruction\n");
    printf("==========================\n");
    
    // 测试1: 基本功能测试
    {
        __m128i a = _mm_setr_epi8(10, 20, 30, 40, 50, 60, 70, 80,
                                 90, 100, 110, 120, 130, 140, 150, 160);
        __m128i b = _mm_setr_epi8(5, 10, 15, 20, 25, 30, 35, 40,
                                 45, 50, 55, 60, 65, 70, 75, 80);
        
        __m128i result;
        asm volatile("vpaddusb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality\n");
        print_m128i_ubytes(a, "Operand A");
        print_m128i_ubytes(b, "Operand B");
        print_m128i_ubytes(result, "Result");
        
        __m128i expected = _mm_setr_epi8(15, 30, 45, 60, 75, 90, 105, 120,
                                        135, 150, 165, 180, 195, 210, 225, 240);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_ubytes(expected, "Expected");
        }
    }
    
    // 测试2: 饱和测试
    {
        __m128i a = _mm_setr_epi8(200, 210, 220, 230, 240, 250, 255, 255,
                                 100, 150, 200, 250, 0, 50, 100, 150);
        __m128i b = _mm_setr_epi8(100, 100, 100, 100, 100, 100, 1, 0,
                                 155, 105, 55, 5, 255, 205, 155, 105);
        
        __m128i result;
        asm volatile("vpaddusb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Saturation cases\n");
        print_m128i_ubytes(a, "Operand A");
        print_m128i_ubytes(b, "Operand B");
        print_m128i_ubytes(result, "Result");
        
        __m128i expected = _mm_setr_epi8(255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_ubytes(expected, "Expected");
        }
    }
    
    // 测试3: 256位AVX测试
    {
        __m256i a = _mm256_setr_epi8(
            10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160,
            170, 180, 190, 200, 210, 220, 230, 240, 250, 255, 255, 255, 255, 255, 255, 255);
        __m256i b = _mm256_setr_epi8(
            5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80,
            85, 90, 95, 100, 105, 110, 115, 120, 125, 1, 0, 0, 0, 0, 0, 0);
        
        __m256i result;
        asm volatile("vpaddusb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: 256-bit AVX\n");
        printf("Operand A: ");
        uint8_t *a_ptr = (uint8_t*)&a;
        for (int i = 0; i < 32; i++) printf("%u ", a_ptr[i]);
        printf("\nOperand B: ");
        uint8_t *b_ptr = (uint8_t*)&b;
        for (int i = 0; i < 32; i++) printf("%u ", b_ptr[i]);
        printf("\nResult: ");
        uint8_t *res_ptr = (uint8_t*)&result;
        for (int i = 0; i < 32; i++) printf("%u ", res_ptr[i]);
        printf("\n");
        
        __m256i expected = _mm256_setr_epi8(
            15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
        
        __m256i cmp = _mm256_cmpeq_epi8(result, expected);
        int mask = _mm256_movemask_epi8(cmp);
        if (mask == (int)0xFFFFFFFF) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            uint8_t *exp_ptr = (uint8_t*)&expected;
            for (int i = 0; i < 32; i++) printf("%u ", exp_ptr[i]);
            printf("\n");
        }
    }
    
    printf("\nVPADDUSB tests completed.\n");
    return 0;
}
