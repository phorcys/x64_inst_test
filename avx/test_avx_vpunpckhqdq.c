#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpunpckhqdq_128() {
    printf("=== Testing vpunpckhqdq (128-bit) ===\n");
    
    uint64_t a[2] ALIGNED(16) = {0};
    uint64_t b[2] ALIGNED(16) = {0};
    uint64_t dst[2] ALIGNED(16) = {0};
    uint64_t expected[2] ALIGNED(16) = {0};
    
    #define TEST_UNPCK(a1, b1, e1, e0) \
    do { \
        a[0] = 0; a[1] = a1; \
        b[0] = 0; b[1] = b1; \
        __m128i v_a = _mm_load_si128((__m128i*)a); \
        __m128i v_b = _mm_load_si128((__m128i*)b); \
        __m128i v_dst; \
        asm volatile( \
            "vpunpckhqdq %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "x"(v_b) \
            :  \
        ); \
        _mm_store_si128((__m128i*)dst, v_dst); \
        expected[0] = e0; expected[1] = e1; \
        printf("A: [%016lX]\n", a1); \
        printf("B: [%016lX]\n", b1); \
        printf("Result:    [%016lX, %016lX]\n", dst[1], dst[0]); \
        printf("Expected:  [%016lX, %016lX]\n", expected[1], expected[0]); \
        printf("Test: %s\n\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 测试1: 基本解包
    TEST_UNPCK(0x1122334455667788, 
               0xAABBCCDDEEFF0011,
               0xAABBCCDDEEFF0011,  // 第二个源的高64位
               0x1122334455667788); // 第一个源的高64位
    
    // 测试2: 边界值
    TEST_UNPCK(0xFFFFFFFFFFFFFFFF, 
               0x0000000000000000,
               0x0000000000000000,  // 第二个源的高64位
               0xFFFFFFFFFFFFFFFF); // 第一个源的高64位
}

static void test_vpunpckhqdq_256() {
    printf("\n=== Testing vpunpckhqdq (256-bit) ===\n");
    
    uint64_t a[4] ALIGNED(32) = {0};
    uint64_t b[4] ALIGNED(32) = {0};
    uint64_t dst[4] ALIGNED(32) = {0};
    uint64_t expected[4] ALIGNED(32) = {0};
    
    #define TEST_UNPCK_256(a3, a1, b3, b1, e3, e2, e1, e0) \
    do { \
        a[0]=0; a[1]=a1; a[2]=0; a[3]=a3; \
        b[0]=0; b[1]=b1; b[2]=0; b[3]=b3; \
        __m256i v_a = _mm256_load_si256((__m256i*)a); \
        __m256i v_b = _mm256_load_si256((__m256i*)b); \
        __m256i v_dst; \
        asm volatile( \
            "vpunpckhqdq %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "x"(v_b) \
            :  \
        ); \
        _mm256_store_si256((__m256i*)dst, v_dst); \
        expected[0]=e0; expected[1]=e1; expected[2]=e2; expected[3]=e3; \
        printf("A: [%016lX, %016lX]\n", a3, a1); \
        printf("B: [%016lX, %016lX]\n", b3, b1); \
        printf("Result:    [%016lX, %016lX, %016lX, %016lX]\n", \
               dst[3], dst[2], dst[1], dst[0]); \
        printf("Expected:  [%016lX, %016lX, %016lX, %016lX]\n", \
               expected[3], expected[2], expected[1], expected[0]); \
        printf("Test: %s\n\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 测试1: 基本解包
    TEST_UNPCK_256(0x1122334455667788, 
                   0x99AABBCCDDEEFF00,
                   0xAABBCCDDEEFF0011,
                   0x1122334455667788,
                   0xAABBCCDDEEFF0011, // 通道1第二个源的高64位
                   0x1122334455667788, // 通道1第一个源的高64位
                   0x1122334455667788, // 通道0第二个源的高64位
                   0x99AABBCCDDEEFF00); // 通道0第一个源的高64位
}

int main() {
    test_vpunpckhqdq_128();
    test_vpunpckhqdq_256();
    return 0;
}
