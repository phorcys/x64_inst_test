#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpunpckhbw_128() {
    printf("=== Testing vpunpckhbw (128-bit) ===\n");
    
    uint8_t a[16] ALIGNED(16) = {0};
    uint8_t b[16] ALIGNED(16) = {0};
    uint8_t dst[16] ALIGNED(16) = {0};
    uint8_t expected[16] ALIGNED(16) = {0};
    
    // 初始化测试数据：a的低8字节为0x00-0x07，高8字节为0x10-0x17
    //              b的低8字节为0x20-0x27，高8字节为0x30-0x37
    for (int i = 0; i < 8; i++) {
        a[i] = i;
        b[i] = i + 0x20;
        a[i+8] = i + 0x10;
        b[i+8] = i + 0x30;
    }
    
    __m128i v_a = _mm_load_si128((__m128i*)a);
    __m128i v_b = _mm_load_si128((__m128i*)b);
    __m128i v_dst;
    
    asm volatile(
        "vpunpckhbw %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm_store_si128((__m128i*)dst, v_dst);
    
    // 构建预期结果：交替取a和b的高8字节
    // 预期结果: [a[8], b[8], a[9], b[9], ..., a[15], b[15]]
    for (int i = 0; i < 8; i++) {
        expected[2*i] = a[8+i];
        expected[2*i+1] = b[8+i];
    }
    
    printf("A: ");
    for (int i = 0; i < 16; i++) printf("%02X ", a[i]);
    printf("\nB: ");
    for (int i = 0; i < 16; i++) printf("%02X ", b[i]);
    printf("\nResult: ");
    for (int i = 0; i < 16; i++) printf("%02X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 16; i++) printf("%02X ", expected[i]);
    printf("\nTest: %s\n\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL");
}

static void test_vpunpckhbw_256() {
    printf("\n=== Testing vpunpckhbw (256-bit) ===\n");
    
    uint8_t a[32] ALIGNED(32) = {0};
    uint8_t b[32] ALIGNED(32) = {0};
    uint8_t dst[32] ALIGNED(32) = {0};
    uint8_t expected[32] ALIGNED(32) = {0};
    
    // 初始化测试数据
    for (int i = 0; i < 16; i++) {
        a[i] = i;
        b[i] = i + 0x20;
        a[i+16] = i + 0x10;
        b[i+16] = i + 0x30;
    }
    
    __m256i v_a = _mm256_load_si256((__m256i*)a);
    __m256i v_b = _mm256_load_si256((__m256i*)b);
    __m256i v_dst;
    
    asm volatile(
        "vpunpckhbw %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm256_store_si256((__m256i*)dst, v_dst);
    
    // 构建预期结果：对两个128位通道分别执行解包操作
    for (int chan = 0; chan < 2; chan++) {
        int offset = chan * 16;
        for (int i = 0; i < 8; i++) {
            expected[offset + 2*i] = a[offset + 8 + i];
            expected[offset + 2*i+1] = b[offset + 8 + i];
        }
    }
    
    printf("A: ");
    for (int i = 0; i < 32; i++) printf("%02X ", a[i]);
    printf("\nB: ");
    for (int i = 0; i < 32; i++) printf("%02X ", b[i]);
    printf("\nResult: ");
    for (int i = 0; i < 32; i++) printf("%02X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 32; i++) printf("%02X ", expected[i]);
    printf("\nTest: %s\n\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpunpckhbw_128();
    test_vpunpckhbw_256();
    return 0;
}
