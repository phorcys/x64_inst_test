#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

void print_words_i16(__m128i value, const char* name) {
    int16_t *v = (int16_t*)&value;
    printf("%s: ", name);
    for (int i = 0; i < 8; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDW instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试
    {
        int16_t a_arr[8] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
        int16_t b_arr[8] = {500, 1000, 1500, 2000, 2500, 3000, 3500, 4000};
        __m128i a = _mm_loadu_si128((__m128i const*)a_arr);
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile("vpaddw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality\n");
        print_words_i16(a, "Operand A");
        print_words_i16(b, "Operand B");
        print_words_i16(result, "Result");
        
        // 预期结果：正常加法（有符号字）
        int16_t expected_arr[8] = {
            1500, 3000, 4500, 6000, // 1000+500=1500, 2000+1000=3000, 3000+1500=4500, 4000+2000=6000
            7500, 9000, 10500, 12000  // 5000+2500=7500, 6000+3000=9000, 7000+3500=10500, 8000+4000=12000
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_i16(expected, "Expected");
        }
    }
    
    // 测试2: 边界值测试（溢出回绕）
    {
        int16_t a_arr[8] = {
            32767, 32767, 32767, -32768,
            -32768, -32768, -32768, 0
        };
        int16_t b_arr[8] = {
            1, 0, -1, -1,
            1, 0, -1, -32768
        };
        __m128i a = _mm_loadu_si128((__m128i const*)a_arr);
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile("vpaddw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Boundary values (overflow wrap-around)\n");
        print_words_i16(a, "Operand A");
        print_words_i16(b, "Operand B");
        print_words_i16(result, "Result");
        
        // 预期结果：回绕加法
        int16_t expected_arr[8] = {
            -32768, 32767, 32766, 32767, // 32767+1=-32768, 32767+0=32767, 32767-1=32766, -32768-1=32767
            -32767, -32768, 32767, -32768  // -32768+1=-32767, -32768+0=-32768, -32768-1=32767, 0-32768=-32768
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_i16(expected, "Expected");
        }
    }
    
    // 测试3: 全零测试
    {
        __m128i a = _mm_setzero_si128();
        __m128i b = _mm_setzero_si128();
        
        __m128i result;
        asm volatile("vpaddw %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: All zeros\n");
        print_words_i16(a, "Operand A");
        print_words_i16(b, "Operand B");
        print_words_i16(result, "Result");
        
        __m128i expected = _mm_setzero_si128();
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_i16(expected, "Expected");
        }
    }
    
    // 测试4: 内存操作数
    {
        int16_t a_mem[8] = {100, 200, 300, 400, 500, 600, 700, 800};
        int16_t b_arr[8] = {50, 100, 150, 200, 250, 300, 350, 400};
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile(
            "vpaddw %[a], %[b], %[res]"
            : [res] "=x" (result)
            : [a] "m" (*a_mem), [b] "x" (b));
        
        printf("\nTest 4: Memory operand\n");
        printf("Operand A (Memory): ");
        for (int i = 0; i < 8; i++) printf("%d ", a_mem[i]);
        printf("\n");
        print_words_i16(b, "Operand B");
        print_words_i16(result, "Result");
        
        // 预期结果：正常加法
        int16_t expected_arr[8] = {
            150, 300, 450, 600, // 100+50=150, 200+100=300, 300+150=450, 400+200=600
            750, 900, 1050, 1200  // 500+250=750, 600+300=900, 700+350=1050, 800+400=1200
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words_i16(expected, "Expected");
        }
    }
    
    printf("\nVPADDW tests completed.\n");
    return 0;
}
