#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmaskmovps指令
void test_vmaskmovps() {
    printf("=== Testing vmaskmovps ===\n");
    
    // 测试128位版本
    {
        float src[4] = {1.0f, 2.0f, 3.0f, 4.0f};
        float dst[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        __m128 mask = _mm_castsi128_ps(_mm_set_epi32(0, 0xFFFFFFFF, 0, 0xFFFFFFFF));
        __m128 reg = _mm_setzero_ps();
        
        printf("Testing vmaskmovps (128-bit):\n");
        printf("Before: reg=[%.2f, %.2f, %.2f, %.2f]\n", 
               reg[0], reg[1], reg[2], reg[3]);
        
        // 使用内联汇编实现vmaskmovps
        reg = _mm_maskload_ps(src, _mm_castps_si128(mask));
        
        _mm_storeu_ps(dst, reg);
        printf("After:  reg=[%.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(dst[0] == 1.0f && dst[1] == 0.0f && 
           dst[2] == 3.0f && dst[3] == 0.0f) {
            printf("  PASS: 128-bit vmaskmovps\n");
        } else {
            printf("  FAIL: 128-bit vmaskmovps\n");
        }
    }
    
    // 测试256位版本
    {
        float src[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        float dst[8] = {0.0f};
        // 设置mask，每个32位元素对应一个mask位
        __m256 mask = _mm256_castsi256_ps(_mm256_set_epi32(
            0xFFFFFFFF, 0, 0xFFFFFFFF, 0,
            0, 0xFFFFFFFF, 0, 0xFFFFFFFF));
        __m256 reg = _mm256_setzero_ps();
        
        printf("\nTesting vmaskmovps (256-bit):\n");
        printf("Before: reg=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               reg[0], reg[1], reg[2], reg[3], 
               reg[4], reg[5], reg[6], reg[7]);
        
        // 使用内联汇编实现vmaskmovps
        reg = _mm256_maskload_ps(src, _mm256_castps_si256(mask));
        
        _mm256_storeu_ps(dst, reg);
        printf("After:  reg=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3],
               dst[4], dst[5], dst[6], dst[7]);
        
        // 验证结果
        // 根据实际输出：mask为1时加载，为0时清零
        if(dst[0] == 1.0f && dst[1] == 0.0f && 
           dst[2] == 3.0f && dst[3] == 0.0f &&
           dst[4] == 0.0f && dst[5] == 6.0f &&
           dst[6] == 0.0f && dst[7] == 8.0f) {
            printf("  PASS: 256-bit vmaskmovps\n");
        } else {
            printf("  FAIL: 256-bit vmaskmovps\n");
        }
    }
    
    // 测试边界条件：NaN和无穷大
    {
        float src[4] = {NAN, INFINITY, -INFINITY, 0.0f};
        float dst[4] = {0.0f};
        __m128 mask = _mm_castsi128_ps(_mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
        __m128 reg = _mm_setzero_ps();
        
        printf("\nTesting vmaskmovps with special values:\n");
        
        reg = _mm_maskload_ps(src, _mm_castps_si128(mask));
        
        _mm_storeu_ps(dst, reg);
        printf("Result: [%f, %f, %f, %f]\n", dst[0], dst[1], dst[2], dst[3]);
        
        if(isnan(dst[0])) {
            printf("  PASS: NaN value correctly loaded\n");
        } else {
            printf("  FAIL: NaN value not loaded correctly\n");
        }
        
        if(isinf(dst[1]) && dst[1] > 0) {
            printf("  PASS: +INFINITY value correctly loaded\n");
        } else {
            printf("  FAIL: +INFINITY value not loaded correctly\n");
        }
        
        if(isinf(dst[2]) && dst[2] < 0) {
            printf("  PASS: -INFINITY value correctly loaded\n");
        } else {
            printf("  FAIL: -INFINITY value not loaded correctly\n");
        }
        
        if(dst[3] == 0.0f) {
            printf("  PASS: 0 value not loaded (mask=0)\n");
        } else {
            printf("  FAIL: 0 value incorrectly loaded\n");
        }
    }

    // 测试未对齐内存访问
    {
        float data[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f}; // 故意不对齐
        float dst[4] = {0.0f};
        __m128 mask = _mm_castsi128_ps(_mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
        __m128 reg = _mm_setzero_ps();
        
        printf("\nTesting vmaskmovps with unaligned memory:\n");
        
        reg = _mm_maskload_ps((float*)&data[1], _mm_castps_si128(mask));
        
        _mm_storeu_ps(dst, reg);
        printf("Result: [%.2f, %.2f, %.2f, %.2f]\n", dst[0], dst[1], dst[2], dst[3]);
        
        if(dst[0] == 2.0f && dst[1] == 3.0f && 
           dst[2] == 4.0f && dst[3] == 5.0f) {
            printf("  PASS: Unaligned access works\n");
        } else {
            printf("  FAIL: Unaligned access failed\n");
        }
    }

    // 测试更复杂的mask模式
    {
        float src[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        float dst[8] = {0.0f};
        __m256 mask = _mm256_castsi256_ps(_mm256_set_epi32(
            0xFFFFFFFF, 0, 0xFFFFFFFF, 0,
            0, 0xFFFFFFFF, 0, 0xFFFFFFFF));
        __m256 reg = _mm256_set1_ps(10.0f); // 初始化为10.0f
        
        printf("\nTesting vmaskmovps with complex mask (256-bit):\n");
        printf("Before: reg=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               reg[0], reg[1], reg[2], reg[3], 
               reg[4], reg[5], reg[6], reg[7]);
        
        reg = _mm256_maskload_ps(src, _mm256_castps_si256(mask));
        
        _mm256_storeu_ps(dst, reg);
        printf("After:  reg=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3],
               dst[4], dst[5], dst[6], dst[7]);
        
        // 根据实际输出：mask为1时加载，为0时清零
        if(dst[0] == 1.0f && dst[1] == 0.0f && 
           dst[2] == 3.0f && dst[3] == 0.0f &&
           dst[4] == 0.0f && dst[5] == 6.0f &&
           dst[6] == 0.0f && dst[7] == 8.0f) {
            printf("  PASS: Complex mask pattern works\n");
        } else {
            printf("  FAIL: Complex mask pattern failed\n");
        }
    }
}

int main() {
    test_vmaskmovps();
    return 0;
}
