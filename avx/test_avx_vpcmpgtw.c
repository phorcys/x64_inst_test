#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印16位整数数组
static void print_words(const char *name, const int16_t *words, int count) {
    printf("%s: [", name);
    for (int i = 0; i < count; i++) {
        printf("%d", words[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
}

// 测试128位版本
static void test_vpcmpgtw_128(const char *name, const int16_t *a, const int16_t *b) {
    __m128i va = _mm_loadu_si128((const __m128i*)a);
    __m128i vb = _mm_loadu_si128((const __m128i*)b);
    __m128i res;
    
    asm volatile(
        "vpcmpgtw %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    int16_t result[8];
    _mm_storeu_si128((__m128i*)result, res);
    
    printf("\nTest 128: %s\n", name);
    print_words("Operand A", a, 8);
    print_words("Operand B", b, 8);
    print_words("Result", result, 8);
}

// 测试256位版本
static void test_vpcmpgtw_256(const char *name, const int16_t *a, const int16_t *b) {
    __m256i va = _mm256_loadu_si256((const __m256i*)a);
    __m256i vb = _mm256_loadu_si256((const __m256i*)b);
    __m256i res;
    
    asm volatile(
        "vpcmpgtw %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    int16_t result[16];
    _mm256_storeu_si256((__m256i*)result, res);
    
    printf("\nTest 256: %s\n", name);
    print_words("Operand A", a, 16);
    print_words("Operand B", b, 16);
    print_words("Result", result, 16);
}

int main() {
    // 测试数据
    int16_t a1[16] = {0, 1, -1, 32767, -32768, 100, -100, 0x7FFF,
                     0, 1, -1, 32767, -32768, 100, -100, 0x7FFF};
    int16_t b1[16] = {1, 0, -2, -32768, 32767, -100, 100, 0x8000,
                     1, 0, -2, -32768, 32767, -100, 100, 0x8000};
    
    // 边界值测试
    int16_t a2[16], b2[16];
    for (int i = 0; i < 16; i++) {
        a2[i] = (i % 2) ? 32767 : -32768;  // 最大/最小有符号16位整数
        b2[i] = (i % 2) ? -32768 : 32767;
    }
    
    // 128位测试
    test_vpcmpgtw_128("Normal case", a1, b1);
    test_vpcmpgtw_128("Boundary case", a2, b2);
    
    // 256位测试
    test_vpcmpgtw_256("Normal case", a1, b1);
    test_vpcmpgtw_256("Boundary case", a2, b2);
    
    return 0;
}
