#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmovntdq指令
void test_vmovntdq() {
    printf("=== Testing vmovntdq ===\n");
    
    // 测试128位版本
    {
        uint32_t src[4] = {0x12345678, 0x87654321, 0xABCDEF01, 0x10FEDCBA};
        uint32_t dst[4] = {0};
        __m128i reg = _mm_loadu_si128((__m128i*)src);
        
        printf("Testing vmovntdq (128-bit):\n");
        printf("Before: src=[0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovntdq
        __asm__ __volatile__(
            "vmovntdq %1, %0"
            : "=m"(dst)
            : "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(memcmp(src, dst, sizeof(src)) == 0) {
            printf("  PASS: 128-bit vmovntdq\n");
        } else {
            printf("  FAIL: 128-bit vmovntdq\n");
        }
    }
    
    // 测试256位版本
    {
        uint32_t src[8] ALIGNED(32) = {0x11111111, 0x22222222, 0x33333333, 0x44444444,
                                     0x55555555, 0x66666666, 0x77777777, 0x88888888};
        uint32_t dst[8] ALIGNED(32) = {0};
        __m256i reg = _mm256_load_si256((__m256i*)src);
        
        printf("\nTesting vmovntdq (256-bit):\n");
        printf("Before: src=[0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               src[0], src[1], src[2], src[3],
               src[4], src[5], src[6], src[7]);
        
        // 使用内联汇编实现vmovntdq
        __asm__ __volatile__(
            "vmovntdq %1, %0"
            : "=m"(dst)
            : "x"(reg)
            : "memory"
        );
        
        printf("After:  dst=[0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               dst[0], dst[1], dst[2], dst[3],
               dst[4], dst[5], dst[6], dst[7]);
        
        // 验证结果
        if(memcmp(src, dst, sizeof(src)) == 0) {
            printf("  PASS: 256-bit vmovntdq\n");
        } else {
            printf("  FAIL: 256-bit vmovntdq\n");
        }
    }
}

int main() {
    test_vmovntdq();
    return 0;
}
