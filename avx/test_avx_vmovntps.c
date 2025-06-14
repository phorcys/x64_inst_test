#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 测试vmovntps指令
void test_vmovntps() {
    printf("=== Testing vmovntps ===\n");
    
    // 测试128位版本
    {
        float src[4] = {1.234f, -2.345f, 3.456f, -4.567f};
        float* dst = (float*)_mm_malloc(4*sizeof(float), 16);
        if(!dst) {
            printf("Memory allocation failed\n");
            return;
        }
        memset(dst, 0, 4*sizeof(float));
        __m128 reg = _mm_loadu_ps(src);
        
        printf("Testing vmovntps (128-bit):\n");
        printf("Before: src=[%.3f, %.3f, %.3f, %.3f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovntps
        __asm__ __volatile__(
            "vmovntps %1, (%0)"
            : 
            : "r"(dst), "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[%.3f, %.3f, %.3f, %.3f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(memcmp(src, dst, 4*sizeof(float)) == 0) {
            printf("  PASS: 128-bit vmovntps\n");
        } else {
            printf("  FAIL: 128-bit vmovntps\n");
        }
        _mm_free(dst);
    }
    
    // 测试256位版本
    {
        float src[8] = {1.1f, -2.2f, 3.3f, -4.4f, 5.5f, -6.6f, 7.7f, -8.8f};
        float* dst = (float*)_mm_malloc(8*sizeof(float), 32);
        if(!dst) {
            printf("Memory allocation failed\n");
            return;
        }
        memset(dst, 0, 8*sizeof(float));
        __m256 reg = _mm256_loadu_ps(src);
        
        printf("\nTesting vmovntps (256-bit):\n");
        printf("Before: src=[%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n",
               src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7]);
        
        // 使用内联汇编实现vmovntps
        __asm__ __volatile__(
            "vmovntps %1, (%0)"
            : 
            : "r"(dst), "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n",
               dst[0], dst[1], dst[2], dst[3], dst[4], dst[5], dst[6], dst[7]);
        
        // 验证结果
        if(memcmp(src, dst, 8*sizeof(float)) == 0) {
            printf("  PASS: 256-bit vmovntps\n");
        } else {
            printf("  FAIL: 256-bit vmovntps\n");
        }
        _mm_free(dst);
    }
    
    // 测试特殊浮点值
    {
        float src[4] = {NAN, -INFINITY, 0.0f, -0.0f};
        float* dst = (float*)_mm_malloc(4*sizeof(float), 16);
        if(!dst) {
            printf("Memory allocation failed\n");
            return;
        }
        memset(dst, 0, 4*sizeof(float));
        __m128 reg = _mm_loadu_ps(src);
        
        printf("\nTesting vmovntps with special values:\n");
        printf("Before: src=[%f, %f, %f, %f]\n", src[0], src[1], src[2], src[3]);
        
        __asm__ __volatile__(
            "vmovntps %1, (%0)"
            : 
            : "r"(dst), "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[%f, %f, %f, %f]\n", dst[0], dst[1], dst[2], dst[3]);
        
        // 验证特殊值
        int pass = 1;
        pass &= isnan(dst[0]);
        pass &= isinf(dst[1]) && signbit(dst[1]);
        pass &= (dst[2] == 0.0f) && !signbit(dst[2]);
        pass &= (dst[3] == 0.0f) && signbit(dst[3]);
        
        if(pass) {
            printf("  PASS: Special values handled correctly\n");
        } else {
            printf("  FAIL: Special values not handled correctly\n");
        }
        _mm_free(dst);
    }
}

int main() {
    test_vmovntps();
    return 0;
}
