#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印字节数组
static void print_bytes(const char *name, const uint8_t *bytes, int count) {
    printf("%s: [", name);
    for (int i = 0; i < count; i++) {
        printf("%d", bytes[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
}

// 测试128位版本
static void test_vpcmpgtb_128(const char *name, const uint8_t *a, const uint8_t *b) {
    __m128i va = _mm_loadu_si128((const __m128i*)a);
    __m128i vb = _mm_loadu_si128((const __m128i*)b);
    __m128i res;
    
    asm volatile(
        "vpcmpgtb %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    uint8_t result[16];
    _mm_storeu_si128((__m128i*)result, res);
    
    printf("\nTest 128: %s\n", name);
    print_bytes("Operand A", a, 16);
    print_bytes("Operand B", b, 16);
    print_bytes("Result", result, 16);
}

// 测试256位版本
static void test_vpcmpgtb_256(const char *name, const uint8_t *a, const uint8_t *b) {
    __m256i va = _mm256_loadu_si256((const __m256i*)a);
    __m256i vb = _mm256_loadu_si256((const __m256i*)b);
    __m256i res;
    
    asm volatile(
        "vpcmpgtb %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    uint8_t result[32];
    _mm256_storeu_si256((__m256i*)result, res);
    
    printf("\nTest 256: %s\n", name);
    print_bytes("Operand A", a, 32);
    print_bytes("Operand B", b, 32);
    print_bytes("Result", result, 32);
}

int main() {
    // 测试数据
    uint8_t a1[32] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    uint8_t b1[32] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                     31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16};
    
    // 边界值测试
    uint8_t a2[32], b2[32];
    for (int i = 0; i < 32; i++) {
        a2[i] = (i < 16) ? 127 : -128;  // 最大/最小有符号字节
        b2[i] = (i < 16) ? -128 : 127;
    }
    
    // 128位测试
    test_vpcmpgtb_128("Normal case", a1, b1);
    test_vpcmpgtb_128("Boundary case", a2, b2);
    
    // 256位测试
    test_vpcmpgtb_256("Normal case", a1, b1);
    test_vpcmpgtb_256("Boundary case", a2, b2);
    
    return 0;
}
