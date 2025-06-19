#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// 打印128位向量的16位无符号整数元素
static inline void print_m128i_uwords(__m128i value, const char* name) {
    printf("%s: ", name);
    uint16_t *v = (uint16_t*)&value;
    for(int i=0; i<8; i++) {
        printf("%u ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDUSW instruction\n");
    printf("==========================\n");
    
    // 测试1: 基本功能测试
    {
        __m128i a = _mm_setr_epi16(1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000);
        __m128i b = _mm_setr_epi16(500, 1000, 1500, 2000, 2500, 3000, 3500, 4000);
        
        __m128i result;
        asm volatile("vpaddusw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality\n");
        print_m128i_uwords(a, "Operand A");
        print_m128i_uwords(b, "Operand B");
        print_m128i_uwords(result, "Result");
        
        __m128i expected = _mm_setr_epi16(1500, 3000, 4500, 6000, 7500, 9000, 10500, 12000);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_uwords(expected, "Expected");
        }
    }
    
    // 测试2: 饱和测试
    {
        __m128i a = _mm_setr_epi16(60000, 62000, 64000, 65000, 0, 10000, 20000, 30000);
        __m128i b = _mm_setr_epi16(6000, 4000, 2000, 1000, 65535, 55535, 45535, 35535);
        
        __m128i result;
        asm volatile("vpaddusw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Saturation cases\n");
        print_m128i_uwords(a, "Operand A");
        print_m128i_uwords(b, "Operand B");
        print_m128i_uwords(result, "Result");
        
        __m128i expected = _mm_setr_epi16(65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_m128i_uwords(expected, "Expected");
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
        asm volatile("vpaddusw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: 256-bit AVX\n");
        printf("Operand A: ");
        uint16_t *a_ptr = (uint16_t*)&a;
        for (int i = 0; i < 16; i++) printf("%u ", a_ptr[i]);
        printf("\nOperand B: ");
        uint16_t *b_ptr = (uint16_t*)&b;
        for (int i = 0; i < 16; i++) printf("%u ", b_ptr[i]);
        printf("\nResult: ");
        uint16_t *res_ptr = (uint16_t*)&result;
        for (int i = 0; i < 16; i++) printf("%u ", res_ptr[i]);
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
            uint16_t *exp_ptr = (uint16_t*)&expected;
            for (int i = 0; i < 16; i++) printf("%u ", exp_ptr[i]);
            printf("\n");
        }
    }
    
    printf("\nVPADDUSW tests completed.\n");
    return 0;
}
