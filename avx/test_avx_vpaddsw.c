#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// 打印128位向量的16位有符号整数元素
static inline void print_m128i_swords(__m128i value, const char* name) {
    printf("%s: ", name);
    int16_t *v = (int16_t*)&value;
    for(int i=0; i<8; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDSW instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试
    {
        __m128i a = _mm_setr_epi16(1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000);
        __m128i b = _mm_setr_epi16(500, 1000, 1500, 2000, 2500, 3000, 3500, 4000);
        
        __m128i result;
        asm volatile("vpaddsw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality\n");
        print_m128i_swords(a, "Operand A");
        print_m128i_swords(b, "Operand B");
        print_m128i_swords(result, "Result");
        
        __m128i expected = _mm_setr_epi16(1500, 3000, 4500, 6000, 7500, 9000, 10500, 12000);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_swords(expected, "Expected");
        }
    }
    
    // 测试2: 饱和测试
    {
        __m128i a = _mm_setr_epi16(30000, 32000, -30000, -32000, 0, 0, 10000, -10000);
        __m128i b = _mm_setr_epi16(3000, 1000, -3000, -1000, 32767, -32768, 25000, -25000);
        
        __m128i result;
        asm volatile("vpaddsw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Saturation cases\n");
        print_m128i_swords(a, "Operand A");
        print_m128i_swords(b, "Operand B");
        print_m128i_swords(result, "Result");
        
        __m128i expected = _mm_setr_epi16(32767, 32767, -32768, -32768, 32767, -32768, 32767, -32768);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_swords(expected, "Expected");
        }
    }
    
    // 测试3: 256位AVX测试
    {
        __m256i a = _mm256_setr_epi16(
            1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000,
            9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000);
        __m256i b = _mm256_setr_epi16(
            500, 1000, 1500, 2000, 2500, 3000, 3500, 4000,
            4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000);
        
        __m256i result;
        asm volatile("vpaddsw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: 256-bit AVX\n");
        printf("Operand A: ");
        int16_t *a_ptr = (int16_t*)&a;
        for (int i = 0; i < 16; i++) printf("%d ", a_ptr[i]);
        printf("\nOperand B: ");
        int16_t *b_ptr = (int16_t*)&b;
        for (int i = 0; i < 16; i++) printf("%d ", b_ptr[i]);
        printf("\nResult: ");
        int16_t *res_ptr = (int16_t*)&result;
        for (int i = 0; i < 16; i++) printf("%d ", res_ptr[i]);
        printf("\n");
        
        __m256i expected = _mm256_setr_epi16(
            1500, 3000, 4500, 6000, 7500, 9000, 10500, 12000,
            13500, 15000, 16500, 18000, 19500, 21000, 22500, 24000);
        
        __m256i cmp = _mm256_cmpeq_epi16(result, expected);
        int mask = _mm256_movemask_epi8(cmp);
        if (mask == (int)0xFFFFFFFF) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: ");
            int16_t *exp_ptr = (int16_t*)&expected;
            for (int i = 0; i < 16; i++) printf("%d ", exp_ptr[i]);
            printf("\n");
        }
    }
    
    printf("\nVPADDSW tests completed.\n");
    return 0;
}
