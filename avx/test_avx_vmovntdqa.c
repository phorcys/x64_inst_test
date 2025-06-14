#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmovntdqa指令
void test_vmovntdqa() {
    printf("=== Testing vmovntdqa ===\n");
    
    // 测试128位版本
    {
        uint32_t src[4] = {0x12345678, 0x87654321, 0xABCDEF01, 0x10FEDCBA};
        uint32_t* aligned_src = (uint32_t*)_mm_malloc(4*sizeof(uint32_t), 16);
        memcpy(aligned_src, src, 4*sizeof(uint32_t));
        __m128i reg;
        
        printf("Testing vmovntdqa (128-bit):\n");
        printf("Before: src=[0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               aligned_src[0], aligned_src[1], aligned_src[2], aligned_src[3]);
        
        // 使用内联汇编实现vmovntdqa
        __asm__ __volatile__(
            "vmovntdqa %1, %0"
            : "=x"(reg)
            : "m"(*aligned_src)
            : 
        );
        
        uint32_t dst[4];
        _mm_storeu_si128((__m128i*)dst, reg);
        
        printf("After:  dst=[0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(memcmp(aligned_src, dst, 4*sizeof(uint32_t)) == 0) {
            printf("  PASS: 128-bit vmovntdqa\n");
        } else {
            printf("  FAIL: 128-bit vmovntdqa\n");
        }
        _mm_free(aligned_src);
    }
    
    // 测试256位版本
    {
        uint32_t src[8] = {0x11111111, 0x22222222, 0x33333333, 0x44444444,
                          0x55555555, 0x66666666, 0x77777777, 0x88888888};
        uint32_t* aligned_src = (uint32_t*)_mm_malloc(8*sizeof(uint32_t), 32);
        memcpy(aligned_src, src, 8*sizeof(uint32_t));
        __m256i reg;
        
        printf("\nTesting vmovntdqa (256-bit):\n");
        printf("Before: src=[0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               aligned_src[0], aligned_src[1], aligned_src[2], aligned_src[3],
               aligned_src[4], aligned_src[5], aligned_src[6], aligned_src[7]);
        
        // 使用内联汇编实现vmovntdqa
        __asm__ __volatile__(
            "vmovntdqa %1, %0"
            : "=x"(reg)
            : "m"(*aligned_src)
            : 
        );
        
        uint32_t dst[8];
        _mm256_storeu_si256((__m256i*)dst, reg);
        
        printf("After:  dst=[0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               dst[0], dst[1], dst[2], dst[3],
               dst[4], dst[5], dst[6], dst[7]);
        
        // 验证结果
        if(memcmp(aligned_src, dst, 8*sizeof(uint32_t)) == 0) {
            printf("  PASS: 256-bit vmovntdqa\n");
        } else {
            printf("  FAIL: 256-bit vmovntdqa\n");
        }
        _mm_free(aligned_src);
    }
}

int main() {
    test_vmovntdqa();
    return 0;
}
