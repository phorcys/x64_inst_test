#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印字向量
static void print_word_vec(const char *name, const uint16_t *vec, int len) {
    printf("%s: [", name);
    for (int i = 0; i < len; i++) {
        printf("%04x", vec[i]);
        if (i < len - 1) printf(" ");
    }
    printf("]\n");
}

// 测试单个比较操作
static int test_case(const char *name, const uint16_t *a, const uint16_t *b, int len) {
    ALIGNED(32) uint16_t result[16] = {0};
    int passed = 1;
    
    if (len == 8) {
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vb = _mm_loadu_si128((const __m128i*)b);
        __m128i vres;
        
        asm volatile("vpcmpeqw %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm_storeu_si128((__m128i*)result, vres);
    } else if (len == 16) {
        __m256i va = _mm256_loadu_si256((const __m256i*)a);
        __m256i vb = _mm256_loadu_si256((const __m256i*)b);
        __m256i vres;
        
        asm volatile("vpcmpeqw %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm256_storeu_si256((__m256i*)result, vres);
    }
    
    // 验证结果
    for (int i = 0; i < len; i++) {
        uint16_t expected = (a[i] == b[i]) ? 0xFFFF : 0x0000;
        if (result[i] != expected) {
            passed = 0;
            break;
        }
    }
    
    printf("\nTest: %s\n", name);
    print_word_vec("Operand A", a, len);
    print_word_vec("Operand B", b, len);
    print_word_vec("Result", result, len);
    printf("Result: %s\n", passed ? "PASS" : "FAIL");
    
    return passed;
}

int main() {
    int total = 0, passed = 0;
    
    // 测试128位版本
    {
        uint16_t a1[8] = {0x0000, 0x1111, 0x2222, 0xFFFF, 0xAAAA, 0x5555, 0x1234, 0x5678};
        uint16_t b1[8] = {0x0000, 0x1111, 0x2222, 0xFFFF, 0xAAAA, 0x5555, 0x1234, 0x5678};
        passed += test_case("VPCMPEQW 128-bit equal", a1, b1, 8);
        total++;
        
        uint16_t a2[8] = {0};
        uint16_t b2[8] = {0xFFFF};
        passed += test_case("VPCMPEQW 128-bit zero vs 0xFFFF", a2, b2, 8);
        total++;
        
        uint16_t a3[8] = {0x5555};
        uint16_t b3[8] = {0xAAAA};
        passed += test_case("VPCMPEQW 128-bit pattern", a3, b3, 8);
        total++;
    }
    
    // 测试256位版本
    {
        uint16_t a1[16];
        uint16_t b1[16];
        for (int i = 0; i < 16; i++) {
            a1[i] = i * 0x1111;
            b1[i] = i * 0x1111;
        }
        passed += test_case("VPCMPEQW 256-bit equal", a1, b1, 16);
        total++;
        
        uint16_t a2[16] = {0};
        uint16_t b2[16] = {0xFFFF};
        passed += test_case("VPCMPEQW 256-bit zero vs 0xFFFF", a2, b2, 16);
        total++;
        
        uint16_t a3[16];
        uint16_t b3[16];
        for (int i = 0; i < 16; i++) {
            a3[i] = i % 2 ? 0x5555 : 0xAAAA;
            b3[i] = i % 2 ? 0xAAAA : 0x5555;
        }
        passed += test_case("VPCMPEQW 256-bit pattern", a3, b3, 16);
        total++;
    }
    
    // 测试内存操作数
    {
        ALIGNED(32) uint16_t mem_ops[16] = {0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC, 0xBA98, 0x7654, 0x3210};
        uint16_t a[8] = {0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC, 0xBA98, 0x7654, 0x3210};
        
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vres;
        
        asm volatile("vpcmpeqw %[mem], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [mem] "m"(*mem_ops));
        
        uint16_t result[8];
        _mm_storeu_si128((__m128i*)result, vres);
        
        int mem_passed = 1;
        for (int i = 0; i < 8; i++) {
            if (result[i] != 0xFFFF) {
                mem_passed = 0;
                break;
            }
        }
        
        printf("\nTest: VPCMPEQW with memory operand\n");
        print_word_vec("Operand A", a, 8);
        print_word_vec("Operand B (mem)", mem_ops, 8);
        print_word_vec("Result", result, 8);
        printf("Result: %s\n", mem_passed ? "PASS" : "FAIL");
        passed += mem_passed;
        total++;
    }
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
