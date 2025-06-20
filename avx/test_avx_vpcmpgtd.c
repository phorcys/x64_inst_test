#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印32位整数数组
static void print_dwords(const char *name, const int32_t *dwords, int count) {
    printf("%s: [", name);
    for (int i = 0; i < count; i++) {
        printf("%d", dwords[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
}

// 测试128位版本
static void test_vpcmpgtd_128(const char *name, const int32_t *a, const int32_t *b) {
    __m128i va = _mm_loadu_si128((const __m128i*)a);
    __m128i vb = _mm_loadu_si128((const __m128i*)b);
    __m128i res;
    
    asm volatile(
        "vpcmpgtd %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    int32_t result[4];
    _mm_storeu_si128((__m128i*)result, res);
    
    printf("\nTest 128: %s\n", name);
    print_dwords("Operand A", a, 4);
    print_dwords("Operand B", b, 4);
    print_dwords("Result", result, 4);
}

// 测试256位版本
static void test_vpcmpgtd_256(const char *name, const int32_t *a, const int32_t *b) {
    __m256i va = _mm256_loadu_si256((const __m256i*)a);
    __m256i vb = _mm256_loadu_si256((const __m256i*)b);
    __m256i res;
    
    asm volatile(
        "vpcmpgtd %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    int32_t result[8];
    _mm256_storeu_si256((__m256i*)result, res);
    
    printf("\nTest 256: %s\n", name);
    print_dwords("Operand A", a, 8);
    print_dwords("Operand B", b, 8);
    print_dwords("Result", result, 8);
}

int main() {
    // 测试数据
    int32_t a1[8] = {0, 1, -1, 2147483647, -2147483648, 100, -100, 0x7FFFFFFF};
    int32_t b1[8] = {1, 0, -2, -2147483648, 2147483647, -100, 100, 0x80000000};
    
    // 边界值测试
    int32_t a2[8], b2[8];
    for (int i = 0; i < 8; i++) {
        a2[i] = (i % 2) ? 2147483647 : -2147483648;  // 最大/最小有符号32位整数
        b2[i] = (i % 2) ? -2147483648 : 2147483647;
    }
    
    // 128位测试
    test_vpcmpgtd_128("Normal case", a1, b1);
    test_vpcmpgtd_128("Boundary case", a2, b2);
    
    // 256位测试
    test_vpcmpgtd_256("Normal case", a1, b1);
    test_vpcmpgtd_256("Boundary case", a2, b2);
    
    return 0;
}
