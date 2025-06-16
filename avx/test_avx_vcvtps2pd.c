#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// 内联汇编实现vcvtps2pd指令
static void test_vcvtps2pd_128(float *src, double *dst) {
    __asm__ __volatile__(
        "vcvtps2pd %1, %0"
        : "=x" (*(__m128d*)dst)
        : "m" (*src)
    );
}

static void test_vcvtps2pd_256(float *src, double *dst) {
    __asm__ __volatile__(
        "vcvtps2pd %1, %0"
        : "=v" (*(__m256d*)dst)
        : "m" (*src)
    );
}

// 测试用例
static const float test_cases[] = {
    0.0f, -0.0f,
    1.0f, -1.0f,
    123.456f, -123.456f,
    INFINITY, -INFINITY,
    NAN, -NAN,
    1.0e-38f, -1.0e-38f,  // 接近下界的值
    3.4028235e38f, -3.4028235e38f  // 接近上界的值
};

int main() {
    printf("Testing vcvtps2pd instruction\n");
    printf("---------------------------\n");

    // 测试128位版本
    printf("\nTesting 128-bit version:\n");
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 2) {
        float src[2] = {test_cases[i], test_cases[i+1]};
        double dst[2] = {0};
        
        test_vcvtps2pd_128(src, dst);
        
        printf("Input:  ");
        print_float_vec("", src, 2);
        printf("Output: ");
        print_double_vec("", dst, 2);
        printf("Hex:    ");
        print_double_vec_hex("", dst, 2);
        printf("\n");
    }

    // 测试256位版本
    printf("\nTesting 256-bit version:\n");
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 4) {
        float src[4] ALIGNED(32) = {test_cases[i], test_cases[i+1], test_cases[i+2], test_cases[i+3]};
        double dst[4] ALIGNED(32) = {0};
        
        test_vcvtps2pd_256(src, dst);
        
        printf("Input:  ");
        print_float_vec("", src, 4);
        printf("Output: ");
        print_double_vec("", dst, 4);
        printf("Hex:    ");
        print_double_vec_hex("", dst, 4);
        printf("\n");
    }

    printf("Tests completed\n");
    return 0;
}
