#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmovmskps指令
void test_vmovmskps() {
    printf("=== Testing vmovmskps ===\n");
    
    // 测试128位版本
    {
        float src[4] = {1.0f, -2.0f, 3.0f, -4.0f};
        __m128 reg = _mm_loadu_ps(src);
        int mask;
        
        printf("Testing vmovmskps (128-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovmskps
        __asm__ __volatile__(
            "vmovmskps %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        // 验证结果
        if(mask == 0xA) {  // 第2、4个元素符号位为1 (二进制1010)
            printf("  PASS: 128-bit vmovmskps\n");
        } else {
            printf("  FAIL: 128-bit vmovmskps\n");
        }
    }
    
    // 测试256位版本 (暂时注释，需要进一步验证)
    /*
    {
        float src[8] = {1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f};
        __m256 reg = _mm256_loadu_ps(src);
        int mask;
        
        printf("\nTesting vmovmskps (256-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3],
               src[4], src[5], src[6], src[7]);
        
        // 使用内联汇编实现vmovmskps
        __asm__ __volatile__(
            "vmovmskps %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        // 验证结果
        if(mask == 0x55) {  // 第2、4、6、8个元素符号位为1 (二进制01010101)
            printf("  PASS: 256-bit vmovmskps\n");
        } else {
            printf("  FAIL: 256-bit vmovmskps\n");
        }
    }
    */
    
    // 测试边界条件：NaN和无穷大
    {
        float src[4] = {NAN, -INFINITY, INFINITY, -0.0f};
        __m128 reg = _mm_loadu_ps(src);
        int mask;
        
        printf("\nTesting vmovmskps with special values:\n");
        printf("Before: src=[%f, %f, %f, %f]\n", src[0], src[1], src[2], src[3]);
        
        __asm__ __volatile__(
            "vmovmskps %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        if(mask == 0xA) {  // 第2、4个元素符号位为1
            printf("  PASS: Special values handled correctly\n");
        } else {
            printf("  FAIL: Special values not handled correctly\n");
        }
    }
}

int main() {
    test_vmovmskps();
    return 0;
}
