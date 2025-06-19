#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// 使用avx.h中的打印函数
static inline void print_m128i_sbytes(__m128i value, const char* name) {
    printf("%s: ", name);
    int8_t *v = (int8_t*)&value;
    for(int i=0; i<16; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDSB instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试
    {
        __m128i a = _mm_setr_epi8(10, 20, 30, 40, 50, 60, 70, 80,
                                 90, 100, 110, 120, -10, -20, -30, -40);
        __m128i b = _mm_setr_epi8(5, 10, 15, 20, 25, 30, 35, 40,
                                 45, 50, 55, 60, -5, -10, -15, -20);
        
        __m128i result;
        asm volatile("vpaddsb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality\n");
        print_m128i_sbytes(a, "Operand A");
        print_m128i_sbytes(b, "Operand B");
        print_m128i_sbytes(result, "Result");
        
        __m128i expected = _mm_setr_epi8(15, 30, 45, 60, 75, 90, 105, 120,
                                       127, 127, 127, 127, -15, -30, -45, -60);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_sbytes(expected, "Expected");
        }
    }
    
    // 测试2: 饱和测试
    {
        __m128i a = _mm_setr_epi8(100, 120, 127, -100, -120, -128, 0, 0,
                                 50, 70, 90, 110, -50, -70, -90, -110);
        __m128i b = _mm_setr_epi8(30, 10, 1, -30, -10, -1, 127, -128,
                                 80, 60, 40, 20, -80, -60, -40, -20);
        
        __m128i result;
        asm volatile("vpaddsb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Saturation cases\n");
        print_m128i_sbytes(a, "Operand A");
        print_m128i_sbytes(b, "Operand B");
        print_m128i_sbytes(result, "Result");
        
        __m128i expected = _mm_setr_epi8(127, 127, 127, -128, -128, -128, 127, -128,
                                       127, 127, 127, 127, -128, -128, -128, -128);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_sbytes(expected, "Expected");
        }
    }
    
    // 测试3: 256位AVX测试
    {
        __m256i a = _mm256_setr_epi8(
            100, 120, 127, -100, -120, -128, 0, 0,
            50, 70, 90, 110, -50, -70, -90, -110,
            10, 20, 30, 40, 50, 60, 70, 80,
            90, 100, 110, 120, -10, -20, -30, -40);
        __m256i b = _mm256_setr_epi8(
            30, 10, 1, -30, -10, -1, 127, -128,
            80, 60, 40, 20, -80, -60, -40, -20,
            5, 10, 15, 20, 25, 30, 35, 40,
            45, 50, 55, 60, -5, -10, -15, -20);
        
        __m256i result;
        asm volatile("vpaddsb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: 256-bit AVX\n");
        printf("Operand A: ");
        int8_t *a_ptr = (int8_t*)&a;
        for (int i = 0; i < 32; i++) printf("%d ", a_ptr[i]);
        printf("\nOperand B: ");
        int8_t *b_ptr = (int8_t*)&b;
        for (int i = 0; i < 32; i++) printf("%d ", b_ptr[i]);
        printf("\nResult: ");
        int8_t *res_ptr = (int8_t*)&result;
        for (int i = 0; i < 32; i++) printf("%d ", res_ptr[i]);
        printf("\n");
        
        __m256i expected = _mm256_setr_epi8(
            127, 127, 127, -128, -128, -128, 127, -128,
            127, 127, 127, 127, -128, -128, -128, -128,
            15, 30, 45, 60, 75, 90, 105, 120,
            127, 127, 127, 127, -15, -30, -45, -60);
        
        __m256i cmp = _mm256_cmpeq_epi8(result, expected);
        int mask = _mm256_movemask_epi8(cmp);
        if (mask == (int)0xFFFFFFFF) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            int8_t *exp_ptr = (int8_t*)&expected;
            for (int i = 0; i < 32; i++) printf("%d ", exp_ptr[i]);
            printf("\n");
        }
    }
    
    printf("\nVPADDSB tests completed.\n");
    return 0;
}
