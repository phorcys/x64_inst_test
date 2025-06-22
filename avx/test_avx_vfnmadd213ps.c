#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 14

typedef struct {
    float a[8];
    float b[8];
    float c[8];
    const char *desc;
} test_case_256;

test_case_256 cases_256[TEST_CASE_COUNT] = {
    // 正常值
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "Normal values (256-bit)"
    },
    // 零值
    {
        {0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f},
        {5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "Zero values (256-bit)"
    },
    // 无穷大
    {
        {INFINITY, -INFINITY, 1.0f, 2.0f, INFINITY, -INFINITY, 1.0f, 2.0f},
        {1.0f, 1.0f, INFINITY, -INFINITY, 1.0f, 1.0f, INFINITY, -INFINITY},
        {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        "Infinity values (256-bit)"
    },
    // NaN
    {
        {NAN, 1.0f, 2.0f, 3.0f, NAN, 1.0f, 2.0f, 3.0f},
        {1.0f, NAN, 2.0f, 3.0f, 1.0f, NAN, 2.0f, 3.0f},
        {1.0f, 2.0f, NAN, 3.0f, 1.0f, 2.0f, NAN, 3.0f},
        "NaN values (256-bit)"
    },
    // 边界值
    {
        {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
        {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f},
        {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
        "Boundary values (256-bit)"
    },
    // 混合值
    {
        {1.0f, INFINITY, NAN, 0.0f, 1.0f, INFINITY, NAN, 0.0f},
        {INFINITY, NAN, 0.0f, NAN, INFINITY, NAN, 0.0f, NAN},
        {NAN, 0.0f, INFINITY, 1.0f, NAN, 0.0f, INFINITY, 1.0f},
        "Mixed special values (256-bit)"
    },
    // 小值
    {
        {1e-30f, 2e-30f, 3e-30f, 4e-30f, 1e-30f, 2e-30f, 3e-30f, 4e-30f},
        {2.0f, 3.0f, 4.0f, 5.0f, 2.0f, 3.0f, 4.0f, 5.0f},
        {1e-30f, 2e-30f, 3e-30f, 4e-30f, 1e-30f, 2e-30f, 3e-30f, 4e-30f},
        "Very small values (256-bit)"
    },
    // a为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN, INFINITY, -INFINITY, NAN, -NAN},
        {2.0f, 3.0f, 4.0f, 5.0f, 2.0f, 3.0f, 4.0f, 5.0f},
        {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        "Special values in a (256-bit)"
    },
    // b为特殊值
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        {INFINITY, -INFINITY, NAN, -NAN, INFINITY, -INFINITY, NAN, -NAN},
        {5.0f, 6.0f, 7.0f, 8.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        "Special values in b (256-bit)"
    },
    // c为特殊值
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        {5.0f, 6.0f, 7.0f, 8.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {INFINITY, -INFINITY, NAN, -NAN, INFINITY, -INFINITY, NAN, -NAN},
        "Special values in c (256-bit)"
    },
    // a和b为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN, INFINITY, -INFINITY, NAN, -NAN},
        {NAN, 0.0f, INFINITY, -INFINITY, NAN, 0.0f, INFINITY, -INFINITY},
        {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        "Special values in a and b (256-bit)"
    },
    // a和c为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN, INFINITY, -INFINITY, NAN, -NAN},
        {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        {NAN, 0.0f, INFINITY, -INFINITY, NAN, 0.0f, INFINITY, -INFINITY},
        "Special values in a and c (256-bit)"
    },
    // 所有特殊值
    {
        {INFINITY, -INFINITY, NAN, 0.0f, INFINITY, -INFINITY, NAN, 0.0f},
        {-NAN, 0.0f, INFINITY, -INFINITY, -NAN, 0.0f, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, NAN, 0.0f, INFINITY, -INFINITY, NAN, 0.0f},
        "All special values (256-bit)"
    }
};

typedef struct {
    float a[4];
    float b[4];
    float c[4];
    const char *desc;
} test_case128;

test_case128 cases_128[TEST_CASE_COUNT] = {
    // 正常值
    {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f},
        "Normal values (128-bit)"
    },
    // 零值
    {
        {0.0f, -0.0f, 0.0f, -0.0f},
        {5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f},
        "Zero values (128-bit)"
    },
    // 无穷大
    {
        {INFINITY, -INFINITY, 1.0f, 2.0f},
        {1.0f, 1.0f, INFINITY, -INFINITY},
        {1.0f, 1.0f, 1.0f, 1.0f},
        "Infinity values (128-bit)"
    },
    // NaN
    {
        {NAN, 1.0f, 2.0f, 3.0f},
        {1.0f, NAN, 2.0f, 3.0f},
        {1.0f, 2.0f, NAN, 3.0f},
        "NaN values (128-bit)"
    },
    // 边界值
    {
        {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
        {2.0f, 2.0f, 2.0f, 2.0f},
        {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
        "Boundary values (128-bit)"
    },
    // 混合值
    {
        {1.0f, INFINITY, NAN, 0.0f},
        {INFINITY, NAN, 0.0f, NAN},
        {NAN, 0.0f, INFINITY, 1.0f},
        "Mixed special values (128-bit)"
    },
    // 小值
    {
        {1e-30f, 2e-30f, 3e-30f, 4e-30f},
        {2.0f, 3.0f, 4.0f, 5.0f},
        {1e-30f, 2e-30f, 3e-30f, 4e-30f},
        "Very small values (128-bit)"
    },
    // a为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN},
        {2.0f, 3.0f, 4.0f, 5.0f},
        {1.0f, 2.0f, 3.0f, 4.0f},
        "Special values in a (128-bit)"
    },
    // b为特殊值
    {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {INFINITY, -INFINITY, NAN, -NAN},
        {5.0f, 6.0f, 7.0f, 8.0f},
        "Special values in b (128-bit)"
    },
    // c为特殊值
    {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {5.0f, 6.0f, 7.0f, 8.0f},
        {INFINITY, -INFINITY, NAN, -NAN},
        "Special values in c (128-bit)"
    },
    // a和b为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN},
        {NAN, 0.0f, INFINITY, -INFINITY},
        {1.0f, 2.0f, 3.0f, 4.0f},
        "Special values in a and b (128-bit)"
    },
    // a和c为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN},
        {1.0f, 2.0f, 3.0f, 4.0f},
        {NAN, 0.0f, INFINITY, -INFINITY},
        "Special values in a and c (128-bit)"
    },
    // 所有特殊值
    {
        {INFINITY, -INFINITY, NAN, 0.0f},
        {-NAN, 0.0f, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, NAN, 0.0f},
        "All special values (128-bit)"
    }
};

static void test_256bit_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(cases_256[t].a);
        __m256 vb = _mm256_loadu_ps(cases_256[t].b);
        __m256 vc = _mm256_loadu_ps(cases_256[t].c);
        
        __asm__ __volatile__(
            "vfnmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        printf("Test Case: %s\n", cases_256[t].desc);
        print_float_vec("A     :", cases_256[t].a, 8);
        print_float_vec("B     :", cases_256[t].b, 8);
        print_float_vec("C     :", cases_256[t].c, 8);
        print_float_vec("Result:", res, 8);
        printf("\n");
    }
    
    printf("VFNMADD213PS 256-bit Register-Register Tests Completed\n\n");
}

static void test_128bit_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases_128[t].a);
        __m128 vb = _mm_loadu_ps(cases_128[t].b);
        __m128 vc = _mm_loadu_ps(cases_128[t].c);
        
        __asm__ __volatile__(
            "vfnmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_storeu_ps(res, va);
        
        printf("Test Case: %s\n", cases_128[t].desc);
        print_float_vec("A     :", cases_128[t].a, 4);
        print_float_vec("B     :", cases_128[t].b, 4);
        print_float_vec("C     :", cases_128[t].c, 4);
        print_float_vec("Result:", res, 4);    
        printf("\n");
    }
    
    printf("VFNMADD213PS 128-bit Register-Register Tests Completed\n\n");
}

static void test_256bit_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(cases_256[t].a);
        __m256 vc = _mm256_loadu_ps(cases_256[t].c);
        
        // 测试不同的内存寻址方式
        float *b_ptr = cases_256[t].b;
        float b_aligned[8] __attribute__((aligned(32)));
        memcpy(b_aligned, cases_256[t].b, sizeof(b_aligned));
        
        // 测试1: 未对齐内存
        __m256 va1 = va;
        __asm__ __volatile__(
            "vfnmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m256*)b_ptr), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m256 va2 = va;
        __asm__ __volatile__(
            "vfnmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*(const __m256*)b_aligned), [c] "x" (vc)
        );
        
        float res1[8], res2[8];
        _mm256_storeu_ps(res1, va1);
        _mm256_storeu_ps(res2, va2);
        
        printf("Memory Operand Test: %s\n", cases_256[t].desc);
        print_float_vec("A     :", cases_256[t].a, 8);
        print_float_vec("B     :", cases_256[t].b, 8);
        print_float_vec("C     :", cases_256[t].c, 8);
        print_float_vec("Unaligned memory:", res1, 8);
        print_float_vec("Aligned memory:", res2, 8);
        printf("\n");
    }
    
    printf("VFNMADD213PS 256-bit Register-Memory Tests Completed\n\n");
}

static void test_128bit_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases_128[t].a);
        __m128 vc = _mm_loadu_ps(cases_128[t].c);
        
        // 测试不同的内存寻址方式
        float *b_ptr = cases_128[t].b;
        float b_aligned[4] __attribute__((aligned(16)));
        memcpy(b_aligned, cases_128[t].b, sizeof(b_aligned));
        
        // 测试1: 未对齐内存
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfnmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m128*)b_ptr), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m128 va2 = va;
        __asm__ __volatile__(
            "vfnmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*(const __m128*)b_aligned), [c] "x" (vc)
        );
        
        float res1[4], res2[4];
        _mm_storeu_ps(res1, va1);
        _mm_storeu_ps(res2, va2);
        
        printf("Memory Operand Test: %s\n", cases_128[t].desc);
        print_float_vec("A     :", cases_128[t].a, 4);
        print_float_vec("B     :", cases_128[t].b, 4);
        print_float_vec("C     :", cases_128[t].c, 4);
        print_float_vec("Unaligned memory:", res1, 4);
        print_float_vec("Aligned memory:", res2, 4);
        printf("\n");
    }
    
    printf("VFNMADD213PS 128-bit Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFNMADD213PS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_128bit_reg_reg_operand();
    test_256bit_reg_reg_operand();
    test_128bit_reg_mem_operand();
    test_256bit_reg_mem_operand();
    
    printf("All VFNMADD213PS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
