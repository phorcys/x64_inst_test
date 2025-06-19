#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

void print_bytes_u8(__m128i value, const char* name) {
    uint8_t *v = (uint8_t*)&value;
    printf("%s: ", name);
    for (int i = 0; i < 16; i++) {
        printf("%u ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPAVGB instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试（四舍五入）
    {
        uint8_t a_arr[16] = {
            100, 150, 200, 50, 10, 20, 30, 40,
            250, 240, 230, 220, 210, 200, 190, 180
        };
        uint8_t b_arr[16] = {
            50, 50, 50, 50, 5, 5, 5, 5,
            5, 10, 15, 20, 25, 30, 35, 40
        };
        __m128i a = _mm_loadu_si128((__m128i const*)a_arr);
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile("vpavgb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("Test 1: Basic functionality (rounding)\n");
        print_bytes_u8(a, "Operand A");
        print_bytes_u8(b, "Operand B");
        print_bytes_u8(result, "Result");
        
        // 预期结果：平均值（四舍五入）
        uint8_t expected_arr[16] = {
            (100+50+1)/2, (150+50+1)/2, (200+50+1)/2, (50+50+1)/2, // 四舍五入
            (10+5+1)/2, (20+5+1)/2, (30+5+1)/2, (40+5+1)/2,
            (250+5+1)/2, (240+10+1)/2, (230+15+1)/2, (220+20+1)/2,
            (210+25+1)/2, (200+30+1)/2, (190+35+1)/2, (180+40+1)/2
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_bytes_u8(expected, "Expected");
        }
    }
    
    // 测试2: 边界值测试（四舍五入）
    {
        uint8_t a_arr[16] = {
            255, 255, 255, 255, 1, 1, 1, 1,
            128, 128, 128, 128, 127, 127, 127, 127
        };
        uint8_t b_arr[16] = {
            0, 1, 2, 255, 0, 1, 2, 3,
            127, 128, 129, 255, 128, 129, 130, 255
        };
        __m128i a = _mm_loadu_si128((__m128i const*)a_arr);
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile("vpavgb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 2: Boundary values (rounding)\n");
        print_bytes_u8(a, "Operand A");
        print_bytes_u8(b, "Operand B");
        print_bytes_u8(result, "Result");
        
        // 预期结果：平均值（四舍五入）
        uint8_t expected_arr[16] = {
            128, 128, 129, 255, // (255+0+1)/2=128, (255+1+1)/2=128, (255+2+1)/2=129, (255+255+1)/2=255
            1, 1, 2, 2,          // (1+0+1)/2=1, (1+1+1)/2=1, (1+2+1)/2=2, (1+3+1)/2=2
            128, 128, 129, 192,  // (128+127+1)/2=128, (128+128+1)/2=128, (128+129+1)/2=129, (128+255+1)/2=192
            128, 128, 129, 191   // (127+128+1)/2=128, (127+129+1)/2=128, (127+130+1)/2=129, (127+255+1)/2=191
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_bytes_u8(expected, "Expected");
        }
    }
    
    // 测试3: 全零测试
    {
        __m128i a = _mm_setzero_si128();
        __m128i b = _mm_setzero_si128();
        
        __m128i result;
        asm volatile("vpavgb %1, %2, %0" : "=x" (result) : "x" (a), "x" (b));
        
        printf("\nTest 3: All zeros\n");
        print_bytes_u8(a, "Operand A");
        print_bytes_u8(b, "Operand B");
        print_bytes_u8(result, "Result");
        
        __m128i expected = _mm_setzero_si128();
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_bytes_u8(expected, "Expected");
        }
    }
    
    // 测试4: 内存操作数
    {
        uint8_t a_mem[16] = {
            10, 20, 30, 40, 50, 60, 70, 80,
            90, 100, 110, 120, 130, 140, 150, 160
        };
        uint8_t b_arr[16] = {
            5, 10, 15, 20, 25, 30, 35, 40,
            45, 50, 55, 60, 65, 70, 75, 80
        };
        __m128i b = _mm_loadu_si128((__m128i const*)b_arr);
        
        __m128i result;
        asm volatile(
            "vpavgb %[a], %[b], %[res]"
            : [res] "=x" (result)
            : [a] "m" (*a_mem), [b] "x" (b));
        
        printf("\nTest 4: Memory operand\n");
        printf("Operand A (Memory): ");
        for (int i = 0; i < 16; i++) printf("%u ", a_mem[i]);
        printf("\n");
        print_bytes_u8(b, "Operand B");
        print_bytes_u8(result, "Result");
        
        // 预期结果：平均值（四舍五入）
        uint8_t expected_arr[16] = {
            (10+5+1)/2, (20+10+1)/2, (30+15+1)/2, (40+20+1)/2,
            (50+25+1)/2, (60+30+1)/2, (70+35+1)/2, (80+40+1)/2,
            (90+45+1)/2, (100+50+1)/2, (110+55+1)/2, (120+60+1)/2,
            (130+65+1)/2, (140+70+1)/2, (150+75+1)/2, (160+80+1)/2
        };
        __m128i expected = _mm_loadu_si128((__m128i const*)expected_arr);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_bytes_u8(expected, "Expected");
        }
    }
    
    printf("\nVPAVGB tests completed.\n");
    return 0;
}
