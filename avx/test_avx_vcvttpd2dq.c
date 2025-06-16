#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>

// 内联汇编实现vcvttpd2dq指令 - 128位版本
static void test_vcvttpd2dq_128(double *src, int32_t *dst) {
    __asm__ __volatile__(
        "vcvttpd2dq %1, %0"
        : "=x" (*(__m128i*)dst)
        : "x" (*(__m128d*)src)
    );
}

// 内联汇编实现vcvttpd2dq指令 - 256位版本
static void test_vcvttpd2dq_256(double *src, int32_t *dst) {
    __asm__ __volatile__(
        "vcvttpd2dq %1, %0"
        : "=x" (*(__m128i*)dst)  // 256->128位结果
        : "x" (*(__m256d*)src)
    );
}

// 测试用例
static const double test_cases[16] = {
    0.0, -0.0,
    1.0, -1.0,
    123.456, -123.456,
    INFINITY, -INFINITY,
    NAN, -NAN,
    DBL_MAX, -DBL_MAX,
    DBL_MIN, -DBL_MIN,
    (double)INT_MAX, (double)INT_MIN
};

// 打印整数向量
static void print_int_vec(const char* name, int32_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing vcvttpd2dq instruction\n");
    printf("---------------------------\n");

    // 测试128位版本
    printf("\nTesting 128-bit version:\n");
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 2) {
        double src[2] ALIGNED(32) = {test_cases[i], test_cases[i+1]};
        int32_t dst[4] ALIGNED(32) = {0};
        
        test_vcvttpd2dq_128(src, dst);
        
        printf("Input:  ");
        print_double_vec("", src, 2);
        printf("Output: ");
        print_int_vec("", dst, 2);
        printf("\n");
    }

    // 测试256位版本
    printf("\nTesting 256-bit version:\n");
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 4) {
        double src[4] ALIGNED(32) = {test_cases[i], test_cases[i+1], 
                                   test_cases[i+2], test_cases[i+3]};
        int32_t dst[4] ALIGNED(32) = {0};
        
        test_vcvttpd2dq_256(src, dst);
        
        printf("Input:  ");
        print_double_vec("", src, 4);
        printf("Output: ");
        print_int_vec("", dst, 4);
        printf("\n");
    }

    printf("Tests completed\n");
    return 0;
}
