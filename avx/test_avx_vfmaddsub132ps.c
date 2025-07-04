#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

// 计算预期值：偶数索引元素为 a*b - c，奇数索引元素为 a*b + c
static void calculate_expected(fma_test_case_128_ps* test_case, float expected[4]) {
    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) {
            expected[i] = fmaf(test_case->a[i], test_case->b[i], -test_case->c[i]);
        } else {
            expected[i] = fmaf(test_case->a[i], test_case->b[i], test_case->c[i]);
        }
    }
}

static void test_reg_reg_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ps(fma_cases_128_ps[t].a);
        __m128 vb = _mm_load_ps(fma_cases_128_ps[t].b);
        __m128 vc = _mm_load_ps(fma_cases_128_ps[t].c);
        
        // 内联汇编实现 VFMADDSUB132PS (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, va);
        
        float expected[4];
        calculate_expected(&fma_cases_128_ps[t], expected);
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128_ps[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_128_ps[t].a[i], fma_cases_128_ps[t].b[i], fma_cases_128_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 128-bit reg-reg\n");
    struct {
        float a[4], b[4], c[4];
        const char* desc;
    } special_cases_128[] = {
        { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, "All zeros" },
        { {1.0f, 1.0f, 1.0f, 1.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, {1.0f, 1.0f, 1.0f, 1.0f}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY, INFINITY, INFINITY}, {0.0f, 0.0f, 0.0f, 0.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN, NAN, NAN}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, "NaN operand" },
        { {-0.0f, -0.0f, -0.0f, -0.0f}, {-0.0f, -0.0f, -0.0f, -0.0f}, {-0.0f, -0.0f, -0.0f, -0.0f}, "Negative zeros" },
        { {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, "Denormal values" },
        { {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, {2.0f, 2.0f, 2.0f, 2.0f}, {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_128) / sizeof(special_cases_128[0]);
    for (int t = 0; t < special_count; t++) {
        __m128 va = _mm_load_ps(special_cases_128[t].a);
        __m128 vb = _mm_load_ps(special_cases_128[t].b);
        __m128 vc = _mm_load_ps(special_cases_128[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, va);
        
        float expected[4];
        calculate_expected(&special_cases_128[t], expected);
        
        printf("Special Test Case: %s (128-bit reg-reg)\n", special_cases_128[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, special_cases_128[t].a[i], special_cases_128[t].b[i], special_cases_128[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_load_ps(fma_cases_128_ps[t].a);
        float* b_ptr = fma_cases_128_ps[t].b;
        __m128 vc = _mm_load_ps(fma_cases_128_ps[t].c);
        
        // 内联汇编实现 VFMADDSUB132PS (128位寄存器-内存)
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, va);
        
        float expected[4];
        calculate_expected(&fma_cases_128_ps[t], expected);
        
        printf("Test Case: %s (128-bit reg-mem)\n", fma_cases_128_ps[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_128_ps[t].a[i], fma_cases_128_ps[t].b[i], fma_cases_128_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 128-bit reg-mem\n");
    struct {
        float a[4], b[4], c[4];
        const char* desc;
    } special_cases_128[] = {
        { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, "All zeros" },
        { {1.0f, 1.0f, 1.0f, 1.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, {1.0f, 1.0f, 1.0f, 1.0f}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY, INFINITY, INFINITY}, {0.0f, 0.0f, 0.0f, 0.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN, NAN, NAN}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, "NaN operand" },
        { {-0.0f, -0.0f, -0.0f, -0.0f}, {-0.0f, -0.0f, -0.0f, -0.0f}, {-0.0f, -0.0f, -0.0f, -0.0f}, "Negative zeros" },
        { {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, "Denormal values" },
        { {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, {2.0f, 2.0f, 2.0f, 2.0f}, {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_128) / sizeof(special_cases_128[0]);
    for (int t = 0; t < special_count; t++) {
        __m128 va = _mm_load_ps(special_cases_128[t].a);
        float* b_ptr = special_cases_128[t].b;
        __m128 vc = _mm_load_ps(special_cases_128[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, va);
        
        float expected[4];
        calculate_expected(&special_cases_128[t], expected);
        
        printf("Special Test Case: %s (128-bit reg-mem)\n", special_cases_128[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, special_cases_128[t].a[i], special_cases_128[t].b[i], special_cases_128[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

// 计算预期值：偶数索引元素为 a*b - c，奇数索引元素为 a*b + c
static void calculate_expected_256(fma_test_case_256_ps* test_case, float expected[8]) {
    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            expected[i] = fmaf(test_case->a[i], test_case->b[i], -test_case->c[i]);
        } else {
            expected[i] = fmaf(test_case->a[i], test_case->b[i], test_case->c[i]);
        }
    }
}

static void test_reg_reg_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_load_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_load_ps(fma_cases_256_ps[t].b);
        __m256 vc = _mm256_load_ps(fma_cases_256_ps[t].c);
        
        // 内联汇编实现 VFMADDSUB132PS (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, va);
        
        float expected[8];
        calculate_expected_256(&fma_cases_256_ps[t], expected);
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256_ps[t].desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_256_ps[t].a[i], fma_cases_256_ps[t].b[i], fma_cases_256_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 256-bit reg-reg\n");
    struct {
        float a[8], b[8], c[8];
        const char* desc;
    } special_cases_256[] = {
        { {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, 
          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, 
          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, "All zeros" },
        { {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, 
          {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}, 
          {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}, 
          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, 
          {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, 
          {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, 
          {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, "NaN operand" },
        { {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}, 
          {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}, 
          {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}, "Negative zeros" },
        { {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, 
          {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, 
          {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, "Denormal values" },
        { {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, 
          {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f}, 
          {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_256) / sizeof(special_cases_256[0]);
    for (int t = 0; t < special_count; t++) {
        __m256 va = _mm256_load_ps(special_cases_256[t].a);
        __m256 vb = _mm256_load_ps(special_cases_256[t].b);
        __m256 vc = _mm256_load_ps(special_cases_256[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, va);
        
        float expected[8];
        calculate_expected_256(&special_cases_256[t], expected);
        
        printf("Special Test Case: %s (256-bit reg-reg)\n", special_cases_256[t].desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, special_cases_256[t].a[i], special_cases_256[t].b[i], special_cases_256[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_load_ps(fma_cases_256_ps[t].a);
        float* b_ptr = fma_cases_256_ps[t].b;
        __m256 vc = _mm256_load_ps(fma_cases_256_ps[t].c);
        
        // 内联汇编实现 VFMADDSUB132PS (256位寄存器-内存)
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, va);
        
        float expected[8];
        calculate_expected_256(&fma_cases_256_ps[t], expected);
        
        printf("Test Case: %s (256-bit reg-mem)\n", fma_cases_256_ps[t].desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, fma_cases_256_ps[t].a[i], fma_cases_256_ps[t].b[i], fma_cases_256_ps[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 256-bit reg-mem\n");
    struct {
        float a[8], b[8], c[8];
        const char* desc;
    } special_cases_256[] = {
        { {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, 
          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, 
          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, "All zeros" },
        { {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, 
          {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}, 
          {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}, 
          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, 
          {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}, 
          {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, 
          {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, "NaN operand" },
        { {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}, 
          {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}, 
          {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}, "Negative zeros" },
        { {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, 
          {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, 
          {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, "Denormal values" },
        { {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, 
          {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f}, 
          {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_256) / sizeof(special_cases_256[0]);
    for (int t = 0; t < special_count; t++) {
        __m256 va = _mm256_load_ps(special_cases_256[t].a);
        float* b_ptr = special_cases_256[t].b;
        __m256 vc = _mm256_load_ps(special_cases_256[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, va);
        
        float expected[8];
        calculate_expected_256(&special_cases_256[t], expected);
        
        printf("Special Test Case: %s (256-bit reg-mem)\n", special_cases_256[t].desc);
        for (int i = 0; i < 8; i++) {
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                   i, special_cases_256[t].a[i], special_cases_256[t].b[i], special_cases_256[t].c[i]);
            printf("Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

int main() {
    printf("==================================\n");
    printf("VFMADDSUB132PS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_reg_reg_128();
    test_reg_mem_128();
    test_reg_reg_256();
    test_reg_mem_256();
    
    printf("All VFMADDSUB132PS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
