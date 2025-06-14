#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmovddup指令
void test_vmovddup() {
    printf("=== Testing vmovddup ===\n");
    
    // 测试128位版本
    {
        double src[2] = {1.0, 2.0};
        double dst[2] = {0.0};
        __m128d reg = _mm_loadu_pd(src);
        
        printf("Testing vmovddup (128-bit):\n");
        printf("Before: src=[%.2f, %.2f]\n", src[0], src[1]);
        
        // 使用内联汇编实现vmovddup
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm_storeu_pd(dst, reg);
        printf("After:  dst=[%.2f, %.2f]\n", dst[0], dst[1]);
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0]) {
            printf("  PASS: 128-bit vmovddup\n");
        } else {
            printf("  FAIL: 128-bit vmovddup\n");
        }
    }
    
    // 测试256位版本
    {
        double src[4] = {1.0, 2.0, 3.0, 4.0};
        double dst[4] = {0.0};
        __m256d reg = _mm256_loadu_pd(src);
        
        printf("\nTesting vmovddup (256-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovddup
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm256_storeu_pd(dst, reg);
        printf("After:  dst=[%.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0] &&
           dst[2] == src[2] && dst[3] == src[2]) {
            printf("  PASS: 256-bit vmovddup\n");
        } else {
            printf("  FAIL: 256-bit vmovddup\n");
        }
    }
    
    // 测试边界条件：NaN和无穷大
    {
        double src[2] = {NAN, INFINITY};
        double dst[2] = {0.0};
        __m128d reg = _mm_loadu_pd(src);
        
        printf("\nTesting vmovddup with special values:\n");
        printf("Before: src=[%f, %f]\n", src[0], src[1]);
        
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm_storeu_pd(dst, reg);
        printf("After:  dst=[%f, %f]\n", dst[0], dst[1]);
        
        if(isnan(dst[0]) && isnan(dst[1])) {
            printf("  PASS: Special values handled correctly\n");
        } else {
            printf("  FAIL: Special values not handled correctly\n");
        }
    }
}

int main() {
    test_vmovddup();
    return 0;
}
