#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmovshdup指令
void test_vmovshdup() {
    printf("=== Testing vmovshdup ===\n");
    
    // 测试128位版本
    {
        float src[4] = {1.0f, 2.0f, 3.0f, 4.0f};
        float dst[4] = {0.0f};
        __m128 reg = _mm_loadu_ps(src);
        
        printf("Testing vmovshdup (128-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovshdup
        __asm__ __volatile__(
            "vmovshdup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm_storeu_ps(dst, reg);
        printf("After:  dst=[%.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(dst[0] == src[1] && dst[1] == src[1] &&
           dst[2] == src[3] && dst[3] == src[3]) {
            printf("  PASS: 128-bit vmovshdup\n");
        } else {
            printf("  FAIL: 128-bit vmovshdup\n");
        }
    }
    
    // 测试256位版本
    {
        float src[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        float dst[8] = {0.0f};
        __m256 reg = _mm256_loadu_ps(src);
        
        printf("\nTesting vmovshdup (256-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3],
               src[4], src[5], src[6], src[7]);
        
        // 使用内联汇编实现vmovshdup
        __asm__ __volatile__(
            "vmovshdup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm256_storeu_ps(dst, reg);
        printf("After:  dst=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3],
               dst[4], dst[5], dst[6], dst[7]);
        
        // 验证结果
        if(dst[0] == src[1] && dst[1] == src[1] &&
           dst[2] == src[3] && dst[3] == src[3] &&
           dst[4] == src[5] && dst[5] == src[5] &&
           dst[6] == src[7] && dst[7] == src[7]) {
            printf("  PASS: 256-bit vmovshdup\n");
        } else {
            printf("  FAIL: 256-bit vmovshdup\n");
        }
    }
    
    // 测试边界条件：NaN和无穷大
    {
        float src[4] = {NAN, INFINITY, -INFINITY, 0.0f};
        float dst[4] = {0.0f};
        __m128 reg = _mm_loadu_ps(src);
        
        printf("\nTesting vmovshdup with special values:\n");
        printf("Before: src=[%f, %f, %f, %f]\n", src[0], src[1], src[2], src[3]);
        
        __asm__ __volatile__(
            "vmovshdup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm_storeu_ps(dst, reg);
        printf("After:  dst=[%f, %f, %f, %f]\n", dst[0], dst[1], dst[2], dst[3]);
        
        if(isinf(dst[0]) && isinf(dst[1]) && dst[0] > 0 &&
           dst[2] == 0.0f && dst[3] == 0.0f) {
            printf("  PASS: Special values handled correctly\n");
        } else {
            printf("  FAIL: Special values not handled correctly\n");
        }
    }
}

int main() {
    test_vmovshdup();
    return 0;
}
