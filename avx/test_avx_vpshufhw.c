#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpshufhw_128() {
    printf("=== Testing vpshufhw (128-bit) ===\n");
    
    uint64_t a[2] ALIGNED(16) = {0x0001000200030004, 0x0005000600070008};
    uint64_t dst[2] ALIGNED(16) = {0};
    uint64_t expected[2] ALIGNED(16) = {0};
    
    // 测试不同的立即数排列
    #define TEST_SHUFFLE(imm, e0, e1, e2, e3) \
    do { \
        __m128i v_a = _mm_load_si128((__m128i*)a); \
        __m128i v_dst; \
        asm volatile( \
            "vpshufhw %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "i"(imm) \
            :  \
        ); \
        _mm_store_si128((__m128i*)dst, v_dst); \
        expected[0] = a[0]; /* 低64位保持不变 */ \
        expected[1] = ((uint64_t)e0 << 48) | ((uint64_t)e1 << 32) | \
                      ((uint64_t)e2 << 16) | e3; \
        printf("Immediate: 0x%02X\n", imm); \
        printf("Input:     %016lX %016lX\n", a[0], a[1]); \
        printf("Result:    %016lX %016lX\n", dst[0], dst[1]); \
        printf("Expected:  %016lX %016lX\n", expected[0], expected[1]); \
        printf("Test: %s\n\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 修正输入值
    a[0] = 0x1111111111111111;
    a[1] = 0x1111222233334444; // 高64位: word0=0x4444, word1=0x3333, word2=0x2222, word3=0x1111
    
    // 修正期望值
    // 立即数0x00: 全部复制第一个元素 (word0=0x4444)
    TEST_SHUFFLE(0x00, 0x4444, 0x4444, 0x4444, 0x4444);
    
    // 立即数0x55: 全部复制第二个元素 (word1=0x3333)
    TEST_SHUFFLE(0x55, 0x3333, 0x3333, 0x3333, 0x3333);
    
    // 立即数0xAA: 全部复制第三个元素 (word2=0x2222)
    TEST_SHUFFLE(0xAA, 0x2222, 0x2222, 0x2222, 0x2222);
    
    // 立即数0xFF: 全部复制第四个元素 (word3=0x1111)
    TEST_SHUFFLE(0xFF, 0x1111, 0x1111, 0x1111, 0x1111);
    
    // 立即数0x1B: 实际指令行为产生逆序
    TEST_SHUFFLE(0x1B, 0x4444, 0x3333, 0x2222, 0x1111);
}

static void test_vpshufhw_256() {
    printf("\n=== Testing vpshufhw (256-bit) ===\n");
    
    uint64_t a[4] ALIGNED(32) = {
        0x0001000200030004, 0x0005000600070008,
        0x0009000A000B000C, 0x000D000E000F0010
    };
    uint64_t dst[4] ALIGNED(32) = {0};
    uint64_t expected[4] ALIGNED(32) = {0};
    
    // 测试256位版本
    #define TEST_SHUFFLE_256(imm, e0, e1, e2, e3, e4, e5, e6, e7) \
    do { \
        __m256i v_a = _mm256_load_si256((__m256i*)a); \
        __m256i v_dst; \
        asm volatile( \
            "vpshufhw %2, %1, %0" \
            : "=x"(v_dst) \
            : "x"(v_a), "i"(imm) \
            :  \
        ); \
        _mm256_store_si256((__m256i*)dst, v_dst); \
        expected[0] = a[0]; /* 低64位保持不变 */ \
        expected[1] = ((uint64_t)e0 << 48) | ((uint64_t)e1 << 32) | \
                      ((uint64_t)e2 << 16) | e3; \
        expected[2] = a[2]; /* 低64位保持不变 */ \
        expected[3] = ((uint64_t)e4 << 48) | ((uint64_t)e5 << 32) | \
                      ((uint64_t)e6 << 16) | e7; \
        printf("Immediate: 0x%02X\n", imm); \
        printf("Input:     %016lX %016lX %016lX %016lX\n", a[0], a[1], a[2], a[3]); \
        printf("Result:    %016lX %016lX %016lX %016lX\n", dst[0], dst[1], dst[2], dst[3]); \
        printf("Expected:  %016lX %016lX %016lX %016lX\n", expected[0], expected[1], expected[2], expected[3]); \
        printf("Test: %s\n\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL"); \
    } while(0)
    
    // 修正输入值
    a[0] = 0x1111111111111111;
    a[1] = 0x1111222233334444; // 低通道高64位: word0=0x4444, word1=0x3333, word2=0x2222, word3=0x1111
    a[2] = 0x5555666677778888;
    a[3] = 0x5555666677778888; // 高通道高64位: word0=0x8888, word1=0x7777, word2=0x6666, word3=0x5555
    
    // 测试1: 低通道复制第一个元素，高通道复制第三个元素
    TEST_SHUFFLE_256(0x00, 0x4444, 0x4444, 0x4444, 0x4444,
                     0x8888, 0x8888, 0x8888, 0x8888);
    
    // 立即数0x1B: 实际指令行为产生逆序
    TEST_SHUFFLE_256(0x1B, 0x4444, 0x3333, 0x2222, 0x1111,
                     0x8888, 0x7777, 0x6666, 0x5555);
}

int main() {
    test_vpshufhw_128();
    test_vpshufhw_256();
    return 0;
}
