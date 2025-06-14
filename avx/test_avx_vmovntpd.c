#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 测试vmovntpd指令
void test_vmovntpd() {
    printf("=== Testing vmovntpd ===\n");
    
    // 测试128位版本
    {
        double src[2] = {1.23456789, -2.34567890};
        double* dst = (double*)_mm_malloc(2*sizeof(double), 16);
        if(!dst) {
            printf("Memory allocation failed\n");
            return;
        }
        memset(dst, 0, 2*sizeof(double));
        __m128d reg = _mm_loadu_pd(src);
        
        printf("Testing vmovntpd (128-bit):\n");
        printf("Before: src=[%.8f, %.8f]\n", src[0], src[1]);
        
        // 使用内联汇编实现vmovntpd
        __asm__ __volatile__(
            "vmovntpd %1, (%0)"
            : 
            : "r"(dst), "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[%.8f, %.8f]\n", dst[0], dst[1]);
        
        // 验证结果
        if(memcmp(src, dst, 2*sizeof(double)) == 0) {
            printf("  PASS: 128-bit vmovntpd\n");
        } else {
            printf("  FAIL: 128-bit vmovntpd\n");
        }
        _mm_free(dst);
    }
    
    // 测试256位版本
    {
        double src[4] = {1.1, -2.2, 3.3, -4.4};
        double* dst = (double*)_mm_malloc(4*sizeof(double), 32);
        if(!dst) {
            printf("Memory allocation failed\n");
            return;
        }
        memset(dst, 0, 4*sizeof(double));
        __m256d reg = _mm256_loadu_pd(src);
        
        printf("\nTesting vmovntpd (256-bit):\n");
        printf("Before: src=[%.8f, %.8f, %.8f, %.8f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovntpd
        __asm__ __volatile__(
            "vmovntpd %1, (%0)"
            : 
            : "r"(dst), "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[%.8f, %.8f, %.8f, %.8f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(memcmp(src, dst, 4*sizeof(double)) == 0) {
            printf("  PASS: 256-bit vmovntpd\n");
        } else {
            printf("  FAIL: 256-bit vmovntpd\n");
        }
        _mm_free(dst);
    }
    
    // 测试特殊浮点值
    {
        double src[2] = {NAN, -INFINITY};
        double* dst = (double*)_mm_malloc(2*sizeof(double), 16);
        if(!dst) {
            printf("Memory allocation failed\n");
            return;
        }
        memset(dst, 0, 2*sizeof(double));
        __m128d reg = _mm_loadu_pd(src);
        
        printf("\nTesting vmovntpd with special values:\n");
        printf("Before: src=[%f, %f]\n", src[0], src[1]);
        
        __asm__ __volatile__(
            "vmovntpd %1, (%0)"
            : 
            : "r"(dst), "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[%f, %f]\n", dst[0], dst[1]);
        
        // 验证NaN和INF是否正确存储
        if(isnan(dst[0]) && isinf(dst[1]) && signbit(dst[1])) {
            printf("  PASS: Special values handled correctly\n");
        } else {
            printf("  FAIL: Special values not handled correctly\n");
        }
        _mm_free(dst);
    }
}

int main() {
    test_vmovntpd();
    return 0;
}
