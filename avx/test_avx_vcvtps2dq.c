#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

void test_vcvtps2dq() {
    printf("--- Testing vcvtps2dq ---\n");
    
    // 测试数据
    float src1[4] ALIGNED(16) = {
        1.5f, -2.5f, 3.75f, -4.25f
    };
    
    float src2[8] ALIGNED(32) = {
        2147483520.0f,  // 接近INT_MAX
        -2147483520.0f, // 接近INT_MIN
        INFINITY,
        -INFINITY,
        NAN,
        1234.5678f,
        -9876.5432f,
        0.0f
    };
    
    int32_t dst1[4] ALIGNED(16) = {0};
    int32_t dst2[8] ALIGNED(32) = {0};
    
    // 测试128位版本
    printf("\n[128-bit version]\n");
    __m128 xmm0 = _mm_load_ps(src1);
    __m128i xmm1;
    
    __asm__ __volatile__(
        "vcvtps2dq %1, %0"
        : "=x"(xmm1)
        : "x"(xmm0)
    );
    
    _mm_store_si128((__m128i*)dst1, xmm1);
    print_int32_vec("Result", dst1, 4);
    printf("Expected: 1 -2 3 -4\n");
    
    // 测试256位版本
    printf("\n[256-bit version]\n");
    __m256 ymm0 = _mm256_load_ps(src2);
    __m256i ymm1;
    
    __asm__ __volatile__(
        "vcvtps2dq %1, %0"
        : "=x"(ymm1)
        : "x"(ymm0)
    );
    
    _mm256_store_si256((__m256i*)dst2, ymm1);
    print_int32_vec("Result", dst2, 8);
    printf("Expected: 2147483520 -2147483520 0x80000000 0x80000000 0x80000000 1234 -9876 0\n");
    
    printf("\n--- vcvtps2dq test completed ---\n");
}

int main() {
    test_vcvtps2dq();
    return 0;
}
