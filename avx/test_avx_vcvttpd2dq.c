#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>

// 内联汇编实现vcvttpd2dq指令 - 128位版本
static uint32_t test_vcvttpd2dq_128(double *src, int32_t *dst) {
    uint32_t mxcsr_after;
    __asm__ __volatile__(
        "vcvttpd2dq %[src], %[dst]\n\t"
        "stmxcsr %[mxcsr]"
        : [dst] "=x" (*(__m128i*)dst), [mxcsr] "=m" (mxcsr_after)
        : [src] "x" (*(__m128d*)src)
        : 
    );
    return mxcsr_after;
}

// 内联汇编实现vcvttpd2dq指令 - 256位版本
static uint32_t test_vcvttpd2dq_256(double *src, int32_t *dst) {
    uint32_t mxcsr_after;
    __asm__ __volatile__(
        "vcvttpd2dq %[src], %[dst]\n\t"
        "stmxcsr %[mxcsr]"
        : [dst] "=x" (*(__m128i*)dst), [mxcsr] "=m" (mxcsr_after)
        : [src] "x" (*(__m256d*)src)
        : 
    );
    return mxcsr_after;
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
        
        uint32_t mxcsr = test_vcvttpd2dq_128(src, dst);
        
        printf("Input:  ");
        print_double_vec("", src, 2);
        printf("Output: ");
        print_int_vec("", dst, 2);
        printf("--- MXCSR State After Operation ---\n");
        print_mxcsr(mxcsr);
        printf("\n");
    }

    // 测试256位版本
    printf("\nTesting 256-bit version:\n");
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 4) {
        double src[4] ALIGNED(32) = {test_cases[i], test_cases[i+1], 
                                   test_cases[i+2], test_cases[i+3]};
        int32_t dst[4] ALIGNED(32) = {0};
        
        uint32_t mxcsr = test_vcvttpd2dq_256(src, dst);
        
        printf("Input:  ");
        print_double_vec("", src, 4);
        printf("Output: ");
        print_int_vec("", dst, 4);
        printf("--- MXCSR State After Operation ---\n");
        print_mxcsr(mxcsr);
        printf("\n");
    }

    printf("Tests completed\n");
    return 0;
}
