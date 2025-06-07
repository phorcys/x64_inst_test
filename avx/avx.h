#ifndef AVX_TEST_H
#define AVX_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印256位向量
static inline void print_ymm(const char* name, __m256i ymm) {
    uint8_t buf[32];
    memcpy(buf, &ymm, 32);
    printf("%s: ", name);
    for(int i=0; i<32; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印128位向量
static inline void print_xmm(const char* name, __m128i xmm) {
    uint8_t buf[16];
    memcpy(buf, &xmm, 16);
    printf("%s: ", name);
    for(int i=0; i<16; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印浮点向量
static inline void print_float_vec(const char* name, float* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%f ", vec[i]);
    }
    printf("\n");
}

// 比较两个256位向量是否相等
static inline int cmp_ymm(__m256i a, __m256i b) {
    uint8_t buf1[32], buf2[32];
    memcpy(buf1, &a, 32);
    memcpy(buf2, &b, 32);
    return memcmp(buf1, buf2, 32) == 0;
}

// 比较两个128位向量是否相等
static inline int cmp_xmm(__m128i a, __m128i b) {
    uint8_t buf1[16], buf2[16];
    memcpy(buf1, &a, 16);
    memcpy(buf2, &b, 16);
    return memcmp(buf1, buf2, 16) == 0;
}

#endif // AVX_TEST_H
