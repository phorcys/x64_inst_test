#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpunpckhdq_128() {
    printf("=== Testing vpunpckhdq (128-bit) ===\n");
    
    uint32_t a[4] ALIGNED(16) = {0};
    uint32_t b[4] ALIGNED(16) = {0};
    uint32_t dst[4] ALIGNED(16) = {0};
    uint32_t expected[4] ALIGNED(16) = {0};
    
    #define TEST_UNPCK(a3, a2, b3, b2, e3, e2, e1, e0) \
    do { \
        a[0] = 0; a[1] = 0; a[2] = a2; a[3] = a3; \
        b[0] = 0; b[1] = 0; b[2] = b2; b[3] = b3; \
        __m128i v_a = _mm_load_si128((__m128i*)a); \
        __m128i v_b = _mm_load_si128((__m128i*)b); \
        __m128i v_dst; \
        asm volatile( \
            "vpunpckhdq %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "x"(v_b) \
            :  \
        ); \
        _mm_store_si128((__m128i*)dst, v_dst); \
        expected[0] = e0; expected[1] = e1; \
        expected[2] = e2; expected[3] = e3; \
        printf("A: [%08X, %08X]\n", a3, a2); \
        printf("B: [%08X, %08X]\n", b3, b2); \
        printf("Result:    [%08X, %08X, %08X, %08X]\n", dst[3], dst[2], dst[1], dst[0]); \
        printf("Expected:  [%08X, %08X, %08X, %08X]\n", expected[3], expected[2], expected[1], expected[0]); \
        printf("Test: %s\n\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 测试1: 基本解包
    TEST_UNPCK(0x11223344, 0x55667788, 
               0xAABBCCDD, 0xEEFF0011,
               0x11223344, 0xAABBCCDD, 
               0x55667788, 0xEEFF0011);
    
    // 测试2: 边界值
    TEST_UNPCK(0xFFFFFFFF, 0x00000000, 
               0x00000000, 0xFFFFFFFF,
               0xFFFFFFFF, 0x00000000, 
               0x00000000, 0xFFFFFFFF);
}

static void test_vpunpckhdq_256() {
    printf("\n=== Testing vpunpckhdq (256-bit) ===\n");
    
    uint32_t a[8] ALIGNED(32) = {0};
    uint32_t b[8] ALIGNED(32) = {0};
    uint32_t dst[8] ALIGNED(32) = {0};
    uint32_t expected[8] ALIGNED(32) = {0};
    
    #define TEST_UNPCK_256(a7, a6, a3, a2, b7, b6, b3, b2, e7, e6, e3, e2, e5, e4, e1, e0) \
    do { \
        a[0]=0; a[1]=0; a[2]=a2; a[3]=a3; a[4]=0; a[5]=0; a[6]=a6; a[7]=a7; \
        b[0]=0; b[1]=0; b[2]=b2; b[3]=b3; b[4]=0; b[5]=0; b[6]=b6; b[7]=b7; \
        __m256i v_a = _mm256_load_si256((__m256i*)a); \
        __m256i v_b = _mm256_load_si256((__m256i*)b); \
        __m256i v_dst; \
        asm volatile( \
            "vpunpckhdq %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "x"(v_b) \
            :  \
        ); \
        _mm256_store_si256((__m256i*)dst, v_dst); \
        expected[0]=e0; expected[1]=e1; expected[2]=e2; expected[3]=e3; \
        expected[4]=e4; expected[5]=e5; expected[6]=e6; expected[7]=e7; \
        printf("A: [%08X, %08X, %08X, %08X]\n", a7, a6, a3, a2); \
        printf("B: [%08X, %08X, %08X, %08X]\n", b7, b6, b3, b2); \
        printf("Result:    [%08X, %08X, %08X, %08X, %08X, %08X, %08X, %08X]\n", \
               dst[7], dst[6], dst[3], dst[2], dst[5], dst[4], dst[1], dst[0]); \
        printf("Expected:  [%08X, %08X, %08X, %08X, %08X, %08X, %08X, %08X]\n", \
               expected[7], expected[6], expected[3], expected[2], \
               expected[5], expected[4], expected[1], expected[0]); \
        printf("Test: %s\n\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 测试1: 基本解包
    TEST_UNPCK_256(0x11223344, 0x55667788, 
                   0x99AABBCC, 0xDDEEFF00,
                   0xAABBCCDD, 0xEEFF0011,
                   0x11223344, 0x55667788,
                   0x11223344, 0xAABBCCDD,
                   0x55667788, 0xEEFF0011,
                   0x99AABBCC, 0x11223344,
                   0xDDEEFF00, 0x55667788);
}

int main() {
    test_vpunpckhdq_128();
    test_vpunpckhdq_256();
    return 0;
}
