#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpshufd_128() {
    printf("=== Testing vpshufd (128-bit) ===\n");
    
    uint32_t a[4] ALIGNED(16) = {0xA0B1C2D3, 0xE4F50617, 0x28393A4B, 0x5C6D7E8F};
    uint32_t dst[4] ALIGNED(16) = {0};
    uint32_t expected[4] ALIGNED(16) = {0};
    
    // 测试不同的立即数排列
    #define TEST_SHUFFLE(imm, e0, e1, e2, e3) \
    do { \
        __m128i v_a = _mm_load_si128((__m128i*)a); \
        __m128i v_dst; \
        asm volatile( \
            "vpshufd %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "i"(imm) \
            :  \
        ); \
        _mm_store_si128((__m128i*)dst, v_dst); \
        expected[0] = a[e0]; expected[1] = a[e1]; \
        expected[2] = a[e2]; expected[3] = a[e3]; \
        printf("Immediate: 0x%02X\n", imm); \
        printf("Result:    "); \
        for (int i = 0; i < 4; i++) printf("%08X ", dst[i]); \
        printf("\nExpected:  "); \
        for (int i = 0; i < 4; i++) printf("%08X ", expected[i]); \
        printf("\nTest: %s\n\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 测试各种排列组合
    TEST_SHUFFLE(0b00000000, 0, 0, 0, 0);  // 全部复制第一个元素
    TEST_SHUFFLE(0b11111111, 3, 3, 3, 3);  // 全部复制最后一个元素
    TEST_SHUFFLE(0b01010101, 1, 1, 1, 1);  // 全部复制第二个元素
    TEST_SHUFFLE(0b11001100, 0, 3, 0, 3);  // 选择[0,3,0,3]
    TEST_SHUFFLE(0b00110011, 3, 0, 3, 0);  // 选择[3,0,3,0]
    TEST_SHUFFLE(0b10100101, 1, 1, 2, 2);  // 选择[1,1,2,2]
}

static void test_vpshufd_256() {
    printf("\n=== Testing vpshufd (256-bit) ===\n");
    
    uint32_t a[8] ALIGNED(32) = {
        0x10203040, 0x50607080, 0x90A0B0C0, 0xD0E0F000,
        0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00
    };
    uint32_t dst[8] ALIGNED(32) = {0};
    uint32_t expected[8] ALIGNED(32) = {0};
    
    // 测试256位版本
    #define TEST_SHUFFLE_256(imm, e0, e1, e2, e3, e4, e5, e6, e7) \
    do { \
        __m256i v_a = _mm256_load_si256((__m256i*)a); \
        __m256i v_dst; \
        asm volatile( \
            "vpshufd %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "i"(imm) \
            :  \
        ); \
        _mm256_store_si256((__m256i*)dst, v_dst); \
        expected[0] = a[e0]; expected[1] = a[e1]; \
        expected[2] = a[e2]; expected[3] = a[e3]; \
        expected[4] = a[e4]; expected[5] = a[e5]; \
        expected[6] = a[e6]; expected[7] = a[e7]; \
        printf("Immediate: 0x%02X\n", imm); \
        printf("Result:    "); \
        for (int i = 0; i < 8; i++) printf("%08X ", dst[i]); \
        printf("\nExpected:  "); \
        for (int i = 0; i < 8; i++) printf("%08X ", expected[i]); \
        printf("\nTest: %s\n\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 测试各种排列组合
    TEST_SHUFFLE_256(0b00000000, 0, 0, 0, 0, 4, 4, 4, 4);
    TEST_SHUFFLE_256(0b11111111, 3, 3, 3, 3, 7, 7, 7, 7);
    TEST_SHUFFLE_256(0b01010101, 1, 1, 1, 1, 5, 5, 5, 5);
    TEST_SHUFFLE_256(0b11001100, 0, 3, 0, 3, 4, 7, 4, 7); // 低通道[0,3,0,3], 高通道[4,7,4,7]
    TEST_SHUFFLE_256(0b10100101, 1, 1, 2, 2, 5, 5, 6, 6); // 低通道[1,1,2,2], 高通道[5,5,6,6]
}

int main() {
    test_vpshufd_128();
    test_vpshufd_256();
    return 0;
}
