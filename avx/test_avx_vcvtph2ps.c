#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 半精度浮点辅助函数
static inline uint16_t float_to_half(float f) {
    uint32_t x = *(uint32_t*)&f;
    uint16_t h = ((x>>16)&0x8000) | ((((x&0x7f800000)-0x38000000)>>13)&0x7c00) | ((x>>13)&0x03ff);
    return h;
}

// 测试vcvtph2ps指令
void test_vcvtph2ps() {
    printf("--- Testing vcvtph2ps ---\n");
    
    // 测试数据
    uint16_t src1[8] ALIGNED(16) = {
        float_to_half(1.5f), float_to_half(2.5f), 
        float_to_half(3.5f), float_to_half(4.5f),
        0, 0, 0, 0
    };
    uint16_t src2[8] ALIGNED(32) = {
        float_to_half(-1.25f), float_to_half(-2.25f),
        0x7E00,  // Half-precision NAN
        0x7C00,  // Half-precision +INF
        0xFC00,  // Half-precision -INF
        float_to_half(0.0f),
        0, 0
    };
    
    float dst1[4] ALIGNED(16) = {0};
    float dst2[8] ALIGNED(32) = {0};
    
    // 测试128位版本
    printf("\n[128-bit version]\n");
    __m128i xmm0 = _mm_load_si128((__m128i*)src1);
    __m128 xmm1;
    
    __asm__ __volatile__(
        "vcvtph2ps %1, %0"
        : "=x"(xmm1)
        : "x"(xmm0)
    );
    
    _mm_store_ps(dst1, xmm1);
    print_float_vec("Result", dst1, 4);
    
    // 测试256位版本
    printf("\n[256-bit version]\n");
    __m128i ymm0 = _mm_load_si128((__m128i*)src2);
    __m256 ymm1;
    
    __asm__ __volatile__(
        "vcvtph2ps %1, %0"
        : "=x"(ymm1)
        : "x"(ymm0)
    );
    
    _mm256_store_ps(dst2, ymm1);
    print_float_vec("Result", dst2, 8);
    print_hex_float_vec("Hex Result", dst2, 8);
    printf("Expected Hex: 0xBFA00000 0xC0100000 0x7FC00000 0x7F800000 0xFF800000 0x40000000 0x00000000 0x00000000\n");
    
    printf("\n--- vcvtph2ps test completed ---\n");
}

int main() {
    test_vcvtph2ps();
    return 0;
}
