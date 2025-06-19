#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

void print_words_u16(__m128i value, const char* name) {
    uint16_t *v = (uint16_t*)&value;
    printf("%s: ", name);
    for (int i = 0; i < 8; i++) {
        printf("%u ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPAVGW instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试（四舍五入）
    {
        uint16_t a_arr[8] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
        uint16_t b_arr[8] = {500, 1000, 1500, 2000, 2500, 3000, 3500, 4000};
        __m128i a = _mm_loadu_si128((__m128i const*)a_arr);
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile("vpavgw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality (rounding)\n");
        print_words_u16(a, "Operand A");
        print_words_u16(b, "Operand B");
        print_words_u16(result, "Result");
        
        // 预期结果：平均值（四舍五入）
        uint16_t expected_arr[8] = {
            (1000+500+1)/2, (2000+1000+1)/2, (3000+1500+1)/2, (4000+2000+1)/2,
            (5000+2500+1)/2, (6000+3000+1)/2, (7000+3500+1)/2, (8000+4000+1)/2
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_u16(expected, "Expected");
        }
    }
    
    // 测试2: 边界值测试（四舍五入）
    {
        uint16_t a_arr[8] = {65535, 65535, 65535, 65535, 1, 1, 1, 1};
        uint16_t b_arr[8] = {0, 1, 2, 65535, 0, 1, 2, 3};
        __m128i a = _mm_loadu_si128((__m128i const*)a_arr);
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile("vpavgw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Boundary values (rounding)\n");
        print_words_u16(a, "Operand A");
        print_words_u16(b, "Operand B");
        print_words_u16(result, "Result");
        
        // 预期结果：平均值（四舍五入）
        uint16_t expected_arr[8] = {
            (65535+0+1)/2, (65535+1+1)/2, (65535+2+1)/2, (65535+65535+1)/2,
            (1+0+1)/2, (1+1+1)/2, (1+2+1)/2, (1+3+1)/2
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_u16(expected, "Expected");
        }
    }
    
    // 测试3: 全零测试
    {
        __m128i a = _mm_setzero_si128();
        __m128i b = _mm_setzero_si128();
        
        __m128i result;
        asm volatile("vpavgw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: All zeros\n");
        print_words_u16(a, "Operand A");
        print_words_u16(b, "Operand B");
        print_words_u16(result, "Result");
        
        __m128i expected = _mm_setzero_si128();
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_u16(expected, "Expected");
        }
    }
    
    // 测试4: 内存操作数
    {
        uint16_t a_mem[8] = {100, 200, 300, 400, 500, 600, 700, 800};
        uint16_t b_arr[8] = {50, 100, 150, 200, 250, 300, 350, 400};
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile(
            "vpavgw %[a], %[b], %[res]"
            : [res] "=x" (result)
            : [a] "m" (*a_mem), [b] "x" (b));
        
        printf("\nTest 4: Memory operand\n");
        printf("Operand A (Memory): ");
        for (int i = 0; i < 8; i++) printf("%u ", a_mem[i]);
        printf("\n");
        print_words_u16(b, "Operand B");
        print_words_u16(result, "Result");
        
        // 预期结果：平均值（四舍五入）
        uint16_t expected_arr[8] = {
            (100+50+1)/2, (200+100+1)/2, (300+150+1)/2, (400+200+1)/2,
            (500+250+1)/2, (600+300+1)/2, (700+350+1)/2, (800+400+1)/2
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_u16(expected, "Expected");
        }
    }
    
    printf("\nVPAVGW tests completed.\n");
    return 0;
}
