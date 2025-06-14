#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmovmskpd指令
void test_vmovmskpd() {
    printf("=== Testing vmovmskpd ===\n");
    
    // 测试128位版本
    {
        double src[2] = {1.0, -2.0};
        __m128d reg = _mm_loadu_pd(src);
        int mask;
        
        printf("Testing vmovmskpd (128-bit):\n");
        printf("Before: src=[%.2f, %.2f]\n", src[0], src[1]);
        
        // 使用内联汇编实现vmovmskpd
        __asm__ __volatile__(
            "vmovmskpd %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        // 验证结果
        if(mask == 0x2) {  // 第二个元素符号位为1
            printf("  PASS: 128-bit vmovmskpd\n");
        } else {
            printf("  FAIL: 128-bit vmovmskpd\n");
        }
    }
    
    // 测试256位版本
    {
        double src[4] = {1.0, -2.0, -3.0, 4.0};
        __m256d reg = _mm256_loadu_pd(src);
        int mask;
        
        printf("\nTesting vmovmskpd (256-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovmskpd
        __asm__ __volatile__(
            "vmovmskpd %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        // 验证结果
        if(mask == 0x6) {  // 第2、3个元素符号位为1
            printf("  PASS: 256-bit vmovmskpd\n");
        } else {
            printf("  FAIL: 256-bit vmovmskpd\n");
        }
    }
    
    // 测试边界条件：NaN和无穷大
    {
        double src[2] = {NAN, -INFINITY};
        __m128d reg = _mm_loadu_pd(src);
        int mask;
        
        printf("\nTesting vmovmskpd with special values:\n");
        printf("Before: src=[%f, %f]\n", src[0], src[1]);
        
        __asm__ __volatile__(
            "vmovmskpd %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        if(mask == 0x2) {  // 第二个元素符号位为1
            printf("  PASS: Special values handled correctly\n");
        } else {
            printf("  FAIL: Special values not handled correctly\n");
        }
    }
}

int main() {
    test_vmovmskpd();
    return 0;
}
