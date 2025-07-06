#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <fenv.h>   
#include <float.h>

void test_vcvtps2dq() {
    printf("--- Testing vcvtps2dq ---\n");
    
    // 测试数据
    float src1[4] ALIGNED(16) = {
        1.5f, -2.5f, 3.75f, -4.25f
    };
    
    float src2[8] ALIGNED(32) = {
        2147483520.0f,  // 接近INT_MAX
        -2147483520.0f, // 接近INT_MIN
        0.0f,          // 正零
        -0.0f,         // 负零
        NAN,
        1234.5678f,
        -9876.5432f,
        0.0f
    };

    float src3[8] ALIGNED(32) = {
        +NAN,
        -NAN,
        FLT_MIN,
        FLT_MAX,
        INFINITY,     // 正无穷
        -INFINITY,    // 负无穷
        -FLT_MIN,
        -FLT_MAX 
    };
    
    int32_t dst1[4] ALIGNED(16) = {0};
    int32_t dst2[8] ALIGNED(32) = {0};
    
        // 定义所有舍入模式 (MXCSR控制位)
    const uint32_t rounding_modes[] = {0x0000, 0x2000, 0x4000, 0x6000};
    const char* mode_names[] = {
        "Round to nearest (even)",
        "Round down (-inf)",
        "Round up (+inf)",
        "Round toward zero"
    };
    
    // 保存原始MXCSR
    uint32_t original_mxcsr = get_mxcsr();
    
    // 测试所有四种舍入模式
    for (int mode_idx = 0; mode_idx < 4; mode_idx++) {
        uint32_t mode = rounding_modes[mode_idx];
        printf("\n=== Testing mode: %s ===\n", mode_names[mode_idx]);
        
        // 设置当前舍入模式
        uint32_t new_mxcsr = (original_mxcsr & ~0x6000) | mode;
        set_mxcsr(new_mxcsr);
            
        // 测试128位版本
        printf("\n[128-bit version]\n");
        __m128 xmm0 = _mm_load_ps(src1);
        __m128i xmm1;
        
        __asm__ __volatile__(
            "vcvtps2dq %1, %0"
            : "=x"(xmm1)
            : "x"(xmm0)
        );
        
        _mm_store_si128((__m128i*)dst1, xmm1);
        print_float_vec("Input ", src1, 4);
        print_int32_vec("Result", dst1, 4);
        print_int32_vec_hex("Result", dst1, 4);
        
        // 测试256位版本
        printf("\n[256-bit version]\n");
        __m256 ymm0 = _mm256_load_ps(src2);
        __m256i ymm1;
        
        __asm__ __volatile__(
            "vcvtps2dq %1, %0"
            : "=x"(ymm1)
            : "x"(ymm0)
        );
        
        _mm256_store_si256((__m256i*)dst2, ymm1);
        print_float_vec("Input ", src2, 8);
        print_int32_vec("Result", dst2, 8);
        print_int32_vec_hex("Result", dst2, 8);

        printf("\n[256-bit edge val version]\n");
        ymm0 = _mm256_load_ps(src3);
        
        __asm__ __volatile__(
            "vcvtps2dq %1, %0"
            : "=x"(ymm1)
            : "x"(ymm0)
        );
        
        _mm256_store_si256((__m256i*)dst2, ymm1);
        print_float_vec("Input ", src3, 8);
        print_int32_vec("Result", dst2, 8);
        print_int32_vec_hex("Result", dst2, 8);
    }
    
    printf("\n--- vcvtps2dq test completed ---\n");
}

int main() {
    test_vcvtps2dq();
    return 0;
}
