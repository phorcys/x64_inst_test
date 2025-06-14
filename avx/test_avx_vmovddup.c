#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <float.h>

// 测试vmovddup指令
void test_vmovddup() {
    printf("=== Testing vmovddup ===\n");
    
    // 测试128位版本 - 寄存器操作数
    {
        double src[2] ALIGNED(16) = {1.0, 2.0};
        double dst[2] ALIGNED(16) = {0.0};
        __m128d reg = _mm_load_pd(src);
        
        printf("Testing vmovddup (128-bit register):\n");
        print_vector128("Before", _mm_load_pd(src));
        
        // 使用内联汇编实现vmovddup
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm_store_pd(dst, reg);
        print_vector128("After ", _mm_load_pd(dst));
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0]) {
            printf("  PASS: 128-bit vmovddup (register)\n");
        } else {
            printf("  FAIL: 128-bit vmovddup (register)\n");
        }
    }
    
    // 测试128位版本 - 内存操作数
    {
        double src[2] ALIGNED(16) = {3.0, 4.0};
        double dst[2] ALIGNED(16) = {0.0};
        __m128d reg;
        
        printf("\nTesting vmovddup (128-bit memory):\n");
        print_vector128("Before", _mm_load_pd(src));
        
        // 使用内联汇编实现vmovddup
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "m"(src[0])
            : 
        );
        
        _mm_store_pd(dst, reg);
        print_vector128("After ", _mm_load_pd(dst));
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0]) {
            printf("  PASS: 128-bit vmovddup (memory)\n");
        } else {
            printf("  FAIL: 128-bit vmovddup (memory)\n");
        }
    }
    
    // 测试256位版本 - 寄存器操作数
    {
        double src[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
        double dst[4] ALIGNED(32) = {0.0};
        __m256d reg = _mm256_load_pd(src);
        
        printf("\nTesting vmovddup (256-bit register):\n");
        print_vector256("Before", _mm256_load_pd(src));
        
        // 使用内联汇编实现vmovddup
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "x"(reg)
            : 
        );
        
        _mm256_store_pd(dst, reg);
        print_vector256("After ", _mm256_load_pd(dst));
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0] &&
           dst[2] == src[2] && dst[3] == src[2]) {
            printf("  PASS: 256-bit vmovddup (register)\n");
        } else {
            printf("  FAIL: 256-bit vmovddup (register)\n");
        }
    }
    
    // 测试256位版本 - 内存操作数
    {
        double src[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0};
        double dst[4] ALIGNED(32) = {0.0};
        __m256d reg;
        
        printf("\nTesting vmovddup (256-bit memory):\n");
        print_vector256("Before", _mm256_load_pd(src));
        
        // 使用内联汇编实现vmovddup
        __asm__ __volatile__(
            "vmovddup %1, %0"
            : "=x"(reg)
            : "m"(src[0])
            : 
        );
        
        _mm256_store_pd(dst, reg);
        print_vector256("After ", _mm256_load_pd(dst));
        
        // 验证结果
        if(dst[0] == src[0] && dst[1] == src[0] &&
           dst[2] == src[2] && dst[3] == src[2]) {
            printf("  PASS: 256-bit vmovddup (memory)\n");
        } else {
            printf("  FAIL: 256-bit vmovddup (memory)\n");
        }
    }
    
    // 测试边界条件
    {
        double test_cases[4][2] = {
            {NAN, INFINITY},
            {0.0, -0.0},
            {1.0e-300, 1.0e+300},
            {-1.0e-300, -1.0e+300}
        };
        
        printf("\nTesting vmovddup with special values:\n");
        
        for(size_t i = 0; i < 4; i++) {
            double src[2] ALIGNED(16) = {test_cases[i][0], test_cases[i][1]};
            double dst[2] ALIGNED(16) = {0.0};
            __m128d reg = _mm_load_pd(src);
            
            printf("\nTest case %zu:\n", i+1);
            print_vector128("Before", _mm_load_pd(src));
            
            __asm__ __volatile__(
                "vmovddup %1, %0"
                : "=x"(reg)
                : "x"(reg)
                : 
            );
            
            _mm_store_pd(dst, reg);
            print_vector128("After ", _mm_load_pd(dst));
            
            // 检查MXCSR状态
            uint32_t mxcsr = get_mxcsr();
            print_mxcsr(mxcsr);
            
            // 验证结果
            int pass = 1;
            if(isnan(src[0])) {
                pass &= isnan(dst[0]) && isnan(dst[1]);
            } else if(src[0] == 0.0 || src[0] == -0.0) {
                pass &= (dst[0] == src[0]) && (dst[1] == src[0]);
            } else {
                pass &= (dst[0] == src[0]) && (dst[1] == src[0]);
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
    test_vmovddup();
    return 0;
}
