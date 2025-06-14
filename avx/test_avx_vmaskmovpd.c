#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试vmaskmovpd指令
void test_vmaskmovpd() {
    printf("=== Testing vmaskmovpd ===\n");
    
    // 测试128位版本
    {
        double src[2] = {1.0, 2.0};
        double dst[2] = {0.0, 0.0};
        __m128d mask = _mm_castsi128_pd(_mm_set_epi64x(0x0000000000000000, 0xFFFFFFFFFFFFFFFF));
        __m128d reg = _mm_setzero_pd();
        
        printf("Testing vmaskmovpd (128-bit):\n");
        printf("Before: reg=[%.2f, %.2f], mask=[0x%llx, 0x%llx]\n", 
               reg[0], reg[1], 
               _mm_extract_epi64(_mm_castpd_si128(mask), 0),
               _mm_extract_epi64(_mm_castpd_si128(mask), 1));
        
        // 使用内联汇编实现vmaskmovpd
        __asm__ __volatile__(
            "vmaskmovpd %1, %2, %0"
            : "=x"(reg)
            : "m"(src), "x"(mask)
            : 
        );
        
        _mm_storeu_pd(dst, reg);
        printf("After:  reg=[%.2f, %.2f]\n", dst[0], dst[1]);
        
        // 验证结果
        if(dst[0] == 1.0 && dst[1] == 0.0) {
            printf("  PASS: 128-bit vmaskmovpd\n");
        } else {
            printf("  FAIL: 128-bit vmaskmovpd\n");
        }
    }
    
    // 测试256位版本
    {
        double src[4] = {1.0, 2.0, 3.0, 4.0};
        double dst[4] = {0.0, 0.0, 0.0, 0.0};
        __m256d mask = _mm256_castsi256_pd(_mm256_set_epi64x(
            0xFFFFFFFFFFFFFFFF, 0x0000000000000000,
            0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
        __m256d reg = _mm256_setzero_pd();
        
        printf("\nTesting vmaskmovpd (256-bit):\n");
        printf("Before: reg=[%.2f, %.2f, %.2f, %.2f]\n", 
               reg[0], reg[1], reg[2], reg[3]);
        
        // 使用内联汇编实现vmaskmovpd
        __asm__ __volatile__(
            "vmaskmovpd %1, %2, %0"
            : "=x"(reg)
            : "m"(src), "x"(mask)
            : 
        );
        
        _mm256_storeu_pd(dst, reg);
        printf("After:  reg=[%.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(dst[0] == 1.0 && dst[1] == 2.0 && dst[2] == 0.0 && dst[3] == 4.0) {
            printf("  PASS: 256-bit vmaskmovpd\n");
        } else {
            printf("  FAIL: 256-bit vmaskmovpd\n");
        }
    }
    
    // 测试边界条件：NaN和无穷大
    {
        double src[2] = {NAN, INFINITY};
        double dst[2] = {0.0, 0.0};
        __m128d mask = _mm_castsi128_pd(_mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
        __m128d reg = _mm_setzero_pd();
        
        printf("\nTesting vmaskmovpd with special values:\n");
        
        __asm__ __volatile__(
            "vmaskmovpd %1, %2, %0"
            : "=x"(reg)
            : "m"(src), "x"(mask)
            : 
        );
        
        _mm_storeu_pd(dst, reg);
        printf("Result: [%f, %f]\n", dst[0], dst[1]);
        
        if(isnan(dst[0])) {
            printf("  PASS: NaN value correctly loaded\n");
        } else {
            printf("  FAIL: NaN value not loaded correctly\n");
        }
        
        if(isinf(dst[1])) {
            printf("  PASS: INFINITY value correctly loaded\n");
        } else {
            printf("  FAIL: INFINITY value not loaded correctly\n");
        }
    }

    // 测试未对齐内存访问
    {
        double data[3] = {1.0, 2.0, 3.0}; // 故意不对齐
        double dst[2] = {0.0, 0.0};
        __m128d mask = _mm_castsi128_pd(_mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
        __m128d reg = _mm_setzero_pd();
        
        printf("\nTesting vmaskmovpd with unaligned memory:\n");
        
        __asm__ __volatile__(
            "vmaskmovpd %1, %2, %0"
            : "=x"(reg)
            : "m"(*(double*)&data[1]), "x"(mask)
            : 
        );
        
        _mm_storeu_pd(dst, reg);
        printf("Result: [%.2f, %.2f]\n", dst[0], dst[1]);
        
        if(dst[0] == 2.0 && dst[1] == 3.0) {
            printf("  PASS: Unaligned access works\n");
        } else {
            printf("  FAIL: Unaligned access failed\n");
        }
    }

    // 测试更复杂的mask模式
    {
        double src[4] = {1.0, 2.0, 3.0, 4.0};
        double dst[4] = {0.0, 0.0, 0.0, 0.0};
        // 注意：mask的位顺序与元素顺序相反
        __m256d mask = _mm256_castsi256_pd(_mm256_set_epi64x(
            0xFFFFFFFFFFFFFFFF, 0x0000000000000000, // 对应元素3和2
            0xFFFFFFFFFFFFFFFF, 0x0000000000000000)); // 对应元素1和0
        __m256d reg = _mm256_set_pd(10.0, 20.0, 30.0, 40.0); // 初始化为不同值
        
        printf("\nTesting vmaskmovpd with complex mask (256-bit):\n");
        printf("Before: reg=[%.2f, %.2f, %.2f, %.2f]\n", 
               reg[0], reg[1], reg[2], reg[3]);
        printf("Mask:   [0x%016llx, 0x%016llx, 0x%016llx, 0x%016llx]\n",
              _mm256_extract_epi64(_mm256_castpd_si256(mask), 0),
              _mm256_extract_epi64(_mm256_castpd_si256(mask), 1),
              _mm256_extract_epi64(_mm256_castpd_si256(mask), 2),
              _mm256_extract_epi64(_mm256_castpd_si256(mask), 3));
        
        __asm__ __volatile__(
            "vmaskmovpd %1, %2, %0"
            : "=x"(reg)
            : "m"(src), "x"(mask)
            : 
        );
        
        _mm256_storeu_pd(dst, reg);
        printf("After:  reg=[%.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 实际观察到的行为：mask为1时从内存加载，为0时清零寄存器值
        // 当前mask设置：元素3和1的mask为1，元素2和0的mask为0
        if(dst[0] == 0.0 && dst[1] == 2.0 && dst[2] == 0.0 && dst[3] == 4.0) {
            printf("  PASS: Complex mask pattern works\n");
        } else {
            printf("  FAIL: Complex mask pattern failed (Got [%.2f, %.2f, %.2f, %.2f])\n",
                  dst[0], dst[1], dst[2], dst[3]);
        }
    }
}

int main() {
    test_vmaskmovpd();
    return 0;
}
