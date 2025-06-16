#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <float.h>
#include <math.h>

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
    
    // 测试边界条件
    {
        double test_cases[][4] = {
            {NAN, -INFINITY, INFINITY, -0.0},
            {0.0, -0.0, DBL_MIN, -DBL_MAX},
            {1.0e-300, -1.0e300, 1.0e300, -1.0e-300}
        };
        
        printf("\nTesting vmovmskpd with special values:\n");
        
        for(size_t i = 0; i < 3; i++) {
            double src[4] ALIGNED(32) = {
                test_cases[i][0], test_cases[i][1],
                test_cases[i][2], test_cases[i][3]
            };
            __m256d reg = _mm256_load_pd(src);
            int mask;
            
            printf("\nTest case %zu:\n", i+1);
            print_vector256d("Before", reg);
            
            __asm__ __volatile__(
                "vmovmskpd %1, %0"
                : "=r"(mask)
                : "x"(reg)
                : 
            );
            
            printf("After:  mask=0x%x\n", mask);
            
            // 检查MXCSR状态
            uint32_t mxcsr = get_mxcsr();
            print_mxcsr(mxcsr);
            
            // 验证结果
            int expected_mask = 0;
            if(src[1] < 0 || signbit(src[1])) expected_mask |= 0x2;
            if(src[2] < 0 || signbit(src[2])) expected_mask |= 0x4;
            if(src[3] < 0 || signbit(src[3])) expected_mask |= 0x8;
            
            if((mask == expected_mask) && !(mxcsr & 0x3F)) {
                printf("  PASS: Special values case %zu\n", i+1);
            } else {
                printf("  FAIL: Special values case %zu\n", i+1);
            }
        }
    }
}

int main() {
    test_vmovmskpd();
    return 0;
}
