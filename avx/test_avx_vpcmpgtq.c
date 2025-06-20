#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>  // 添加PRId64宏定义
#include <string.h>
#include <immintrin.h>

// 打印64位整数数组
static void print_qwords(const char *name, const int64_t *qwords, int count) {
    printf("%s: [", name);
    for (int i = 0; i < count; i++) {
        printf("%" PRId64, qwords[i]);  // 使用PRId64宏确保跨平台兼容性
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
}

// 测试128位版本
static void test_vpcmpgtq_128(const char *name, const int64_t *a, const int64_t *b) {
    __m128i va = _mm_loadu_si128((const __m128i*)a);
    __m128i vb = _mm_loadu_si128((const __m128i*)b);
    __m128i res;
    
    asm volatile(
        "vpcmpgtq %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    int64_t result[2];
    _mm_storeu_si128((__m128i*)result, res);
    
    printf("\nTest 128: %s\n", name);
    print_qwords("Operand A", a, 2);
    print_qwords("Operand B", b, 2);
    print_qwords("Result", result, 2);
}

// 测试256位版本
static void test_vpcmpgtq_256(const char *name, const int64_t *a, const int64_t *b) {
    __m256i va = _mm256_loadu_si256((const __m256i*)a);
    __m256i vb = _mm256_loadu_si256((const __m256i*)b);
    __m256i res;
    
    asm volatile(
        "vpcmpgtq %[b], %[a], %[res]"
        : [res] "=x"(res)
        : [a] "x"(va), [b] "xm"(vb)
    );
    
    int64_t result[4];
    _mm256_storeu_si256((__m256i*)result, res);
    
    printf("\nTest 256: %s\n", name);
    print_qwords("Operand A", a, 4);
    print_qwords("Operand B", b, 4);
    print_qwords("Result", result, 4);
}

int main() {
    // 测试数据
    int64_t a1[4] = {0, 1, -1, INT64_MAX};
    int64_t b1[4] = {1, 0, -2, INT64_MIN};
    
    // 边界值测试
    int64_t a2[4] = {INT64_MAX, INT64_MIN, INT64_MAX, INT64_MIN};
    int64_t b2[4] = {INT64_MIN, INT64_MAX, INT64_MIN, INT64_MAX};
    
    // 128位测试
    test_vpcmpgtq_128("Normal case", a1, b1);
    test_vpcmpgtq_128("Boundary case", a2, b2);
    
    // 256位测试
    test_vpcmpgtq_256("Normal case", a1, b1);
    test_vpcmpgtq_256("Boundary case", a2, b2);
    
    return 0;
}
