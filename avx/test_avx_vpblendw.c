#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_vpblendw() {
    printf("=== Testing VPBLENDW ===\n");
    
    // 测试数据
    ALIGNED(16) uint16_t src1[8] = {0x1111, 0x2222, 0x3333, 0x4444, 
                                   0x5555, 0x6666, 0x7777, 0x8888};
    ALIGNED(16) uint16_t src2[8] = {0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD,
                                   0xEEEE, 0xFFFF, 0x0000, 0x1111};
    ALIGNED(16) uint16_t dst[8];
    
    // 128位测试
    __m128i xmm1 = _mm_load_si128((__m128i*)src1);
    __m128i xmm2 = _mm_load_si128((__m128i*)src2);
    
    // 测试1: 交替选择(立即数0xAA)
    __m128i res;
    asm volatile (
        "vpblendw $0xAA, %1, %2, %0"
        : "=x"(res)
        : "x"(xmm2), "x"(xmm1)
    );
    _mm_store_si128((__m128i*)dst, res);
    uint16_t expected1[8] = {0x1111, 0xBBBB, 0x3333, 0xDDDD,
                           0x5555, 0xFFFF, 0x7777, 0x1111};
    printf("Test1 - 128bit alternating blend:\n");
    print_xmm("Expected", _mm_load_si128((__m128i*)expected1));
    print_xmm("Result  ", res);
    printf("Match: %s\n\n", cmp_xmm(res, _mm_load_si128((__m128i*)expected1)) ? "YES" : "NO");

    // 256位测试
    ALIGNED(32) uint16_t src1_256[16] = {0x1111, 0x2222, 0x3333, 0x4444, 
                                       0x5555, 0x6666, 0x7777, 0x8888,
                                       0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
                                       0xDDDD, 0xEEEE, 0xFFFF, 0x0000};
    ALIGNED(32) uint16_t src2_256[16] = {0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD,
                                       0xEEEE, 0xFFFF, 0x0000, 0x1111,
                                       0x2222, 0x3333, 0x4444, 0x5555,
                                       0x6666, 0x7777, 0x8888, 0x9999};
    ALIGNED(32) uint16_t dst_256[16];
    
    __m256i ymm1 = _mm256_load_si256((__m256i*)src1_256);
    __m256i ymm2 = _mm256_load_si256((__m256i*)src2_256);
    
    // 测试2: 256位混合(立即数0xF0)
    __m256i res256;
    asm volatile (
        "vpblendw $0xF0, %1, %2, %0"
        : "=x"(res256)
        : "x"(ymm2), "x"(ymm1)
    );
    _mm256_store_si256((__m256i*)dst_256, res256);
    uint16_t expected2[16] = {0x1111, 0x2222, 0x3333, 0x4444,
                            0xEEEE, 0xFFFF, 0x0000, 0x1111,
                            0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
                            0x6666, 0x7777, 0x8888, 0x9999};
    printf("Test2 - 256bit blend:\n");
    print_ymm("Expected", _mm256_load_si256((__m256i*)expected2));
    print_ymm("Result  ", res256);
    printf("Match: %s\n\n", cmp_ymm(res256, _mm256_load_si256((__m256i*)expected2)) ? "YES" : "NO");
}

int main() {
    test_vpblendw();
    return 0;
}
