#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试vcvtpd2ps指令
void test_vcvtpd2ps() {
    printf("--- Testing vcvtpd2ps ---\n");
    
    // 测试数据
    double src1[4] ALIGNED(32) = {1.5, 2.5, 3.5, 4.5};
    double src2[2] ALIGNED(16) = {-1.25, -2.25};
    double nan_src[4] ALIGNED(32) = {NAN, INFINITY, -INFINITY, 0.0};
    
    float dst1[4] ALIGNED(16) = {0};
    float dst2[8] ALIGNED(32) = {0};
    float expected1[4] = {-1.25f, -2.25f, 0, 0};
    float expected2[8] = {1.5f, 2.5f, 3.5f, 4.5f, 0, 0, 0, 0};
    
    // 测试128位版本
    printf("\n[128-bit version]\n");
    __m128d xmm0 = _mm_load_pd(src2);
    __m128 xmm1;
    
    __asm__ __volatile__(
        "vcvtpd2ps %1, %0"
        : "=x"(xmm1)
        : "x"(xmm0)
    );
    
    _mm_store_ps(dst1, xmm1);
    print_float_vec("Result", dst1, 4);
    print_float_vec("Expected", expected1, 4);
    
    // 测试256位版本
    printf("\n[256-bit version]\n");
    __m256d ymm0 = _mm256_load_pd(src1);
    __m128 ymm1;
    
    __asm__ __volatile__(
        "vcvtpd2ps %1, %0"
        : "=x"(ymm1)
        : "x"(ymm0)
    );
    
    _mm_store_ps(dst2, ymm1);
    print_float_vec("Result", dst2, 4);
    print_float_vec("Expected", expected2, 4);
    
    // 测试NaN/INF情况
    printf("\n[Testing NaN/INF]\n");
    __m256d ymm2 = _mm256_load_pd(nan_src);
    __m128 ymm3;
    
    __asm__ __volatile__(
        "vcvtpd2ps %1, %0"
        : "=x"(ymm3)
        : "x"(ymm2)
    );
    
    _mm_store_ps(dst2, ymm3);
    print_hex_float_vec("NaN/INF Result", dst2, 4);
    
    printf("\n--- vcvtpd2ps test completed ---\n");
}

int main() {
    test_vcvtpd2ps();
    return 0;
}
