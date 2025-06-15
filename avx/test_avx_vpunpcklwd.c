#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpunpcklwd_128() {
    printf("=== Testing vpunpcklwd (128-bit) ===\n");
    
    uint16_t a[8] ALIGNED(16) = {0};
    uint16_t b[8] ALIGNED(16) = {0};
    uint16_t dst[8] ALIGNED(16) = {0};
    uint16_t expected[8] ALIGNED(16) = {0};
    
    // 初始化测试数据
    for (int i = 0; i < 4; i++) {
        a[i] = i;         // 低4个元素
        a[i+4] = i + 0x10; // 高4个元素
        b[i] = i + 0x20;
        b[i+4] = i + 0x30;
    }
    
    __m128i v_a = _mm_load_si128((__m128i*)a);
    __m128i v_b = _mm_load_si128((__m128i*)b);
    __m128i v_dst;
    
    asm volatile(
        "vpunpcklwd %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm_store_si128((__m128i*)dst, v_dst);
    
    // 构建预期结果：交替取a和b的低4个元素
    for (int i = 0; i < 4; i++) {
        expected[2*i] = a[i];
        expected[2*i+1] = b[i];
    }
    
    printf("A: ");
    for (int i = 0; i < 8; i++) printf("%04X ", a[i]);
    printf("\nB: ");
    for (int i = 0; i < 8; i++) printf("%04X ", b[i]);
    printf("\nResult: ");
    for (int i = 0; i < 8; i++) printf("%04X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 8; i++) printf("%04X ", expected[i]);
    printf("\nTest: %s\n\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL");
}

static void test_vpunpcklwd_256() {
    printf("\n=== Testing vpunpcklwd (256-bit) ===\n");
    
    uint16_t a[16] ALIGNED(32) = {0};
    uint16_t b[16] ALIGNED(32) = {0};
    uint16_t dst[16] ALIGNED(32) = {0};
    uint16_t expected[16] ALIGNED(32) = {0};
    
    // 初始化测试数据
    for (int i = 0; i < 8; i++) {
        a[i] = i;
        b[i] = i + 0x20;
        a[i+8] = i + 0x10;
        b[i+8] = i + 0x30;
    }
    
    __m256i v_a = _mm256_load_si256((__m256i*)a);
    __m256i v_b = _mm256_load_si256((__m256i*)b);
    __m256i v_dst;
    
    asm volatile(
        "vpunpcklwd %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm256_store_si256((__m256i*)dst, v_dst);
    
    // 构建预期结果：对两个128位通道分别执行解包操作
    for (int chan = 0; chan < 2; chan++) {
        int offset = chan * 8;
        for (int i = 0; i < 4; i++) {
            expected[offset + 2*i] = a[offset + i];
            expected[offset + 2*i+1] = b[offset + i];
        }
    }
    
    printf("A: ");
    for (int i = 0; i < 16; i++) printf("%04X ", a[i]);
    printf("\nB: ");
    for (int i = 0; i < 16; i++) printf("%04X ", b[i]);
    printf("\nResult: ");
    for (int i = 0; i < 16; i++) printf("%04X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 16; i++) printf("%04X ", expected[i]);
    printf("\nTest: %s\n\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpunpcklwd_128();
    test_vpunpcklwd_256();
    return 0;
}
