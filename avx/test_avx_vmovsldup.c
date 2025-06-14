#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <float.h>

// 测试vmovsldup指令
void test_vmovsldup() {
    printf("=== Testing vmovsldup ===\n");
    
    // 测试128位版本
    {
        float src[4] = {1.0f, 2.0f, 3.0f, 4.0f};
        float dst[4] = {0.0f};
        __m128 reg = _mm_loadu_ps(src);
        
        printf("Testing vmovsldup (128-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3]);
        
        // 使用内联汇编实现vmovsldup
        __asm__ __volatile__(
            "vmovsldup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm_storeu_ps(dst, reg);
        printf("After:  dst=[%.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3]);
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0] &&
           dst[2] == src[2] && dst[3] == src[2]) {
            printf("  PASS: 128-bit vmovsldup\n");
        } else {
            printf("  FAIL: 128-bit vmovsldup\n");
        }
    }
    
    // 测试256位版本
    {
        float src[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        float dst[8] = {0.0f};
        __m256 reg = _mm256_loadu_ps(src);
        
        printf("\nTesting vmovsldup (256-bit):\n");
        printf("Before: src=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               src[0], src[1], src[2], src[3],
               src[4], src[5], src[6], src[7]);
        
        // 使用内联汇编实现vmovsldup
        __asm__ __volatile__(
            "vmovsldup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm256_storeu_ps(dst, reg);
        printf("After:  dst=[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n", 
               dst[0], dst[1], dst[2], dst[3],
               dst[4], dst[5], dst[6], dst[7]);
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0] &&
           dst[2] == src[2] && dst[3] == src[2] &&
           dst[4] == src[4] && dst[5] == src[4] &&
           dst[6] == src[6] && dst[7] == src[6]) {
            printf("  PASS: 256-bit vmovsldup\n");
        } else {
            printf("  FAIL: 256-bit vmovsldup\n");
        }
    }
    
    // 测试内存操作数版本
    {
        float src[4] ALIGNED(16) = {1.5f, 2.5f, 3.5f, 4.5f};
        float dst[4] ALIGNED(16) = {0.0f};
        __m128 reg;
        
        printf("\nTesting vmovsldup (128-bit memory):\n");
        print_vector128("Before", _mm_load_ps(src));
        
        __asm__ __volatile__(
            "vmovsldup %1, %0"
            : "=x"(reg)
            : "m"(src)
            : 
        );
        
        _mm_store_ps(dst, reg);
        print_vector128("After ", _mm_load_ps(dst));
        
        if(dst[0] == src[0] && dst[1] == src[0] &&
           dst[2] == src[2] && dst[3] == src[2]) {
            printf("  PASS: 128-bit vmovsldup (memory)\n");
        } else {
            printf("  FAIL: 128-bit vmovsldup (memory)\n");
        }
    }

    // 测试边界条件
    {
        float test_cases[][4] = {
            {NAN, INFINITY, -INFINITY, 0.0f},
            {0.0f, -0.0f, FLT_MIN, FLT_MAX},
            {1.0e-30f, 1.0e30f, -1.0e-30f, -1.0e30f}
        };
        
        printf("\nTesting vmovsldup with special values:\n");
        
        for(size_t i = 0; i < 3; i++) {  // 明确指定测试用例数量
            float src[4] ALIGNED(16) = {
                test_cases[i][0], test_cases[i][1],
                test_cases[i][2], test_cases[i][3]
            };
            float dst[4] ALIGNED(16) = {0.0f};
            __m128 reg = _mm_load_ps(src);
            
            printf("\nTest case %zu:\n", i+1);
            print_vector128("Before", _mm_load_ps(src));
            
            __asm__ __volatile__(
                "vmovsldup %1, %0"
                : "=x"(reg)
                : "x"(reg)
                : 
            );
            
            _mm_store_ps(dst, reg);
            print_vector128("After ", _mm_load_ps(dst));
            
            // 检查MXCSR状态
            uint32_t mxcsr = get_mxcsr();
            print_mxcsr(mxcsr);
            
            // 验证结果
            int pass = 1;
            if(isnan(src[0])) {
                pass &= isnan(dst[0]) && isnan(dst[1]);
            } else {
                pass &= (dst[0] == src[0]) && (dst[1] == src[0]);
            }
            
            if(isnan(src[2])) {
                pass &= isnan(dst[2]) && isnan(dst[3]);
            } else {
                pass &= (dst[2] == src[2]) && (dst[3] == src[2]);
            }
            
            if(pass && !(mxcsr & 0x3F)) { // 检查异常标志
                printf("  PASS: Special values case %zu\n", i+1);
            } else {
                printf("  FAIL: Special values case %zu\n", i+1);
            }
        }
    }
}

int main() {
    test_vmovsldup();
    return 0;
}
