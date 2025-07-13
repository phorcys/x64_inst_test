#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <float.h>

// 打印256位浮点向量
static void print_m256(const char* name, __m256 vec) {
    float *vals = (float*)&vec;
    printf("%s: [%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f]\n", name, vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7]);
}

int main() {
    int errors = 0;
    
    printf("=== Testing vmaskmovps (conditional move based on mask) ===\n");

    printf("\n[Test 1: 256-bit REG mask operation]\n");
    {
        ALIGNED(32) float src[8] = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0};
        ALIGNED(32) float mask[8] = {-1.0, 0.0, -1.0, 0.0,-1.0, 0.0, -1.0, 0.0}; // -1.0表示设置所有位
        ALIGNED(32) float dest[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        ALIGNED(32) float expected[8] = {10.0, 0.0, 30.0, 0.0, 50.0, 0.0, 70.0, 0.0};
        
        __m256 dest_vec = _mm256_load_ps(dest);
        __m256 mask_vec = _mm256_load_ps(mask);
        
        CLEAR_FLAGS;
        print_m256("Dest    ", dest_vec);        
        // 内联汇编实现vmaskmovps
        asm volatile (
            "vmaskmovps %1, %2, %0"
            : "=x"(dest_vec)
            : "m"(src), "v"(mask_vec)
            :
        );
        
        // 打印输入和结果
        print_m256("Mask    ", mask_vec);
        print_m256("Source  ",_mm256_load_ps(src));
        print_m256("Result  ", dest_vec);
        print_m256("Expected", _mm256_load_ps(expected));
        

    }

    printf("\n[Test 2: 256-bit REG mask operation]\n");
    {
        ALIGNED(32) float src2[8] = {INFINITY, -INFINITY, NAN, -NAN, FLT_MAX, FLT_MIN, 0.0, -0.0};
        ALIGNED(32) float mask2[8] = {0.0, -1.0, 0.0, -1.0, 0.0, -1.0, 0.0, -1.0}; // -1.0表示设置所有位
        ALIGNED(32) float dest2[8] = {50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0};
        ALIGNED(32) float expected2[8] = {0.000000, -INFINITY, 0.000000, -NAN, 0.000000, 0.000000, 0.000000, -0.000000};
        
        __m256 dest_vec2 = _mm256_load_ps(dest2);
        __m256 mask_vec2 = _mm256_load_ps(mask2);
        
        CLEAR_FLAGS;
        print_m256("Dest    ", dest_vec2);        
        // 内联汇编实现vmaskmovps
        asm volatile (
            "vmaskmovps %1, %2, %0"
            : "=x"(dest_vec2)
            : "m"(src2), "v"(mask_vec2)
            :
        );
        
        // 打印输入和结果
        print_m256("Mask    ", mask_vec2);
        print_m256("Source  ", _mm256_load_ps(src2));
        print_m256("Result  ", dest_vec2);
        print_m256("Expected", _mm256_load_ps(expected2));
        

    }    
    printf("\n[Test 3: 256-bit MEM mask operation]\n");
    {
        ALIGNED(32) float src[8] = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0};
        ALIGNED(32) float mask[8] = {-1.0, 0.0, -1.0, 0.0,-1.0, 0.0, -1.0, 0.0}; // -1.0表示设置所有位
        ALIGNED(32) float dest[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        ALIGNED(32) float expected[8] = {10.0, 0.0, 30.0, 0.0, 50.0, 0.0, 70.0, 0.0};
        
        __m256 src_vec = _mm256_load_ps(src);
        __m256 mask_vec = _mm256_load_ps(mask);
        
        CLEAR_FLAGS;
        print_m256("Dest    ", _mm256_load_ps(dest));        
        // 内联汇编实现vmaskmovps
        asm volatile (
            "vmaskmovps %1, %2, %0"
            : "=m"(*dest)
            : "v"(src_vec), "v"(mask_vec)
            : "memory"
        );
        
        // 打印输入和结果
        print_m256("Mask    ", mask_vec);
        print_m256("Source  ", src_vec);
        print_m256("Result  ", _mm256_load_ps(dest));
        print_m256("Expected", _mm256_load_ps(expected));
        

    }
    
    printf("\n[Test 4: 256-bit MEM mask with boundary values]\n");
    {
        ALIGNED(32) float src1[8] = {INFINITY, -INFINITY, NAN, -NAN, FLT_MAX, FLT_MIN, 0.0, -0.0};
        ALIGNED(32) float mask1[8] = {0.0, -1.0, 0.0, -1.0, 0.0, -1.0, 0.0, -1.0}; // -1.0表示设置所有位
        ALIGNED(32) float dest1[8] = {50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0};
        ALIGNED(32) float expected1[8] = {50.0, -INFINITY, 70.0, -NAN, 90.0, 0.0, 110.0, -0.0};
        
        __m256 src_vec1 = _mm256_load_ps(src1);
        __m256 mask_vec1 = _mm256_load_ps(mask1);
        
        CLEAR_FLAGS;
        print_m256("Dest    ", _mm256_load_ps(dest1));

        asm volatile (
            "vmaskmovps %1, %2, %0"
            : "=m"(*dest1)
            : "v"(src_vec1), "v"(mask_vec1)
            : "memory"
        );
        
        // 打印输入和结果
        print_m256("Mask    ", mask_vec1);
        print_m256("Source  ", src_vec1);
        print_m256("Result  ", _mm256_load_ps(dest1));
        print_m256("Expected", _mm256_load_ps(expected1));
        

    }
    
    printf("\n=== Tests complete: %d errors ===\n", errors);
    return errors ? 1 : 0;
}
