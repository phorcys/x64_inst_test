#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <float.h>
#include <math.h>

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
    
    // 测试256位版本
    {
        float src[8] = {1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f};
        __m256 reg = _mm256_loadu_ps(src);
        int mask;
        
        printf("\nTesting vmovmskps (256-bit):\n");
        print_vector256("Before", reg);
        
        // 使用内联汇编实现vmovmskps
        __asm__ __volatile__(
            "vmovmskps %1, %0"
            : "=r"(mask)
            : "x"(reg)
            : 
        );
        
        printf("After:  mask=0x%x\n", mask);
        
        // 检查MXCSR状态
        uint32_t mxcsr = get_mxcsr();
        print_mxcsr(mxcsr);
        
        // 验证结果
        if(mask == 0xaa && !(mxcsr & 0x3F)) {  // 第2、4、6、8个元素符号位为1 (二进制10101010)
            printf("  PASS: 256-bit vmovmskps\n");
        } else {
            printf("  FAIL: 256-bit vmovmskps\n");
        }
    }
    
    // 测试边界条件
    {
        float test_cases[][8] = {
            {NAN, -INFINITY, INFINITY, -0.0f, 0.0f, -0.0f, FLT_MIN, -FLT_MAX},
            {1.0e-30f, -1.0e30f, 1.0e30f, -1.0e-30f, 0.0f, -0.0f, FLT_MIN, -FLT_MAX}
        };
        
        printf("\nTesting vmovmskps with special values:\n");
        
        for(size_t i = 0; i < 2; i++) {
            float src[8] ALIGNED(32) = {
                test_cases[i][0], test_cases[i][1],
                test_cases[i][2], test_cases[i][3],
                test_cases[i][4], test_cases[i][5],
                test_cases[i][6], test_cases[i][7]
            };
            __m256 reg = _mm256_load_ps(src);
            int mask;
            
            printf("\nTest case %zu:\n", i+1);
            print_vector256("Before", reg);
            
            __asm__ __volatile__(
                "vmovmskps %1, %0"
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
            if(src[3] < 0 || signbit(src[3])) expected_mask |= 0x8;
            if(src[5] < 0 || signbit(src[5])) expected_mask |= 0x20;
            if(src[7] < 0 || signbit(src[7])) expected_mask |= 0x80;
            
            if((mask == expected_mask) && !(mxcsr & 0x3F)) {
                printf("  PASS: Special values case %zu\n", i+1);
            } else {
                printf("  FAIL: Special values case %zu\n", i+1);
            }
        }
    }
}

int main() {
    test_vmovmskps();
    return 0;
}
