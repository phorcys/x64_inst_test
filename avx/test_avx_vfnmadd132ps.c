#include "avx.h"
#include <stdio.h>

// vfnmadd132ps指令测试
void test_vfnmadd132ps() {
    printf("=== Testing vfnmadd132ps ===\n");
    
    // 测试128位版本
    {
        __m128 dst = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
        __m128 src1 = _mm_set_ps(2.0f, 3.0f, 4.0f, 5.0f);
        __m128 src2 = _mm_set_ps(0.5f, 1.5f, 2.5f, 3.5f);
        
        __m128 result;
        asm volatile (
            "vmovaps %1, %%xmm0\n\t"
            "vmovaps %2, %%xmm1\n\t"
            "vmovaps %3, %%xmm2\n\t"
            "vfnmadd132ps %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovaps %%xmm0, %0\n\t"
            : "=x"(result)
            : "x"(dst), "x"(src1), "x"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        print_vector128("128-bit result", result);
        
        float expected[4];
        for (int i = 0; i < 4; i++) {
            expected[i] = -(dst[i] * src1[i]) + src2[i];
        }
        print_float_vec("128-bit expected", expected, 4);
    }
    
    // 测试256位版本
    {
        __m256 dst = _mm256_set_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        __m256 src1 = _mm256_set_ps(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        __m256 src2 = _mm256_set_ps(0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f);
        
        __m256 result;
        asm volatile (
            "vmovaps %1, %%ymm0\n\t"
            "vmovaps %2, %%ymm1\n\t"
            "vmovaps %3, %%ymm2\n\t"
            "vfnmadd132ps %%ymm1, %%ymm2, %%ymm0\n\t"
            "vmovaps %%ymm0, %0\n\t"
            : "=x"(result)
            : "x"(dst), "x"(src1), "x"(src2)
            : "ymm0", "ymm1", "ymm2"
        );
        
        print_vector256("256-bit result", result);
        
        float expected[8];
        for (int i = 0; i < 8; i++) {
            expected[i] = -(dst[i] * src1[i]) + src2[i];
        }
        print_float_vec("256-bit expected", expected, 8);
    }
    
    // 测试特殊值(NaN, Inf)
    {
        __m128 dst = _mm_set_ps(INFINITY, -INFINITY, NAN, 0.0f);
        __m128 src1 = _mm_set_ps(1.0f, -1.0f, 1.0f, 0.0f);
        __m128 src2 = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
        
        __m128 result;
        asm volatile (
            "vmovaps %1, %%xmm0\n\t"
            "vmovaps %2, %%xmm1\n\t"
            "vmovaps %3, %%xmm2\n\t"
            "vfnmadd132ps %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovaps %%xmm0, %0\n\t"
            : "=x"(result)
            : "x"(dst), "x"(src1), "x"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        print_vector128("Special values result", result);
    }
    
    printf("\n");
}

int main() {
    test_vfnmadd132ps();
    return 0;
}
