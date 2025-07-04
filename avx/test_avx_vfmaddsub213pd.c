#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

// 计算预期值：偶数索引元素为 a*b - c，奇数索引元素为 a*b + c
static void calculate_expected(fma_test_case_128* test_case, double expected[2]) {
    for (int i = 0; i < 2; i++) {
        if (i % 2 == 0) {
            expected[i] = fma(test_case->a[i], test_case->b[i], -test_case->c[i]);
        } else {
            expected[i] = fma(test_case->a[i], test_case->b[i], test_case->c[i]);
        }
    }
}

static void test_reg_reg_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_pd(fma_cases_128[t].a);
        __m128d vb = _mm_load_pd(fma_cases_128[t].b);
        __m128d vc = _mm_load_pd(fma_cases_128[t].c);
        
        // 内联汇编实现 VFMADDSUB213PD (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmaddsub213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_store_pd(res, va);
        
        double expected[2];
        calculate_expected(&fma_cases_128[t], expected);
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 128-bit reg-reg\n");
    fma_test_case_128 special_cases_128[] = {
        { {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, "All zeros" },
        { {1.0, 1.0}, {INFINITY, INFINITY}, {1.0, 1.0}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY}, {0.0, 0.0}, {INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN}, {1.0, 1.0}, {1.0, 1.0}, "NaN operand" },
        { {-0.0, -0.0}, {-0.0, -0.0}, {-0.0, -0.0}, "Negative zeros" },
        { {DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN}, "Denormal values" },
        { {DBL_MAX, DBL_MAX}, {2.0, 2.0}, {DBL_MAX, DBL_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_128) / sizeof(special_cases_128[0]);
    for (int t = 0; t < special_count; t++) {
        __m128d va = _mm_load_pd(special_cases_128[t].a);
        __m128d vb = _mm_load_pd(special_cases_128[t].b);
        __m128d vc = _mm_load_pd(special_cases_128[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_store_pd(res, va);
        
        double expected[2];
        calculate_expected(&special_cases_128[t], expected);
        
        printf("Special Test Case: %s (128-bit reg-reg)\n", special_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, special_cases_128[t].a[i], special_cases_128[t].b[i], special_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_load_pd(fma_cases_128[t].a);
        __m128d vb = _mm_load_pd(fma_cases_128[t].b);
        double* c_ptr = fma_cases_128[t].c;
        
        // 内联汇编实现 VFMADDSUB213PD (128位寄存器-内存)
        __asm__ __volatile__(
            "vfmaddsub213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*c_ptr)
        );
        
        double res[2];
        _mm_store_pd(res, va);
        
        double expected[2];
        calculate_expected(&fma_cases_128[t], expected);
        
        printf("Test Case: %s (128-bit reg-mem)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 128-bit reg-mem\n");
    fma_test_case_128 special_cases_128[] = {
        { {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, "All zeros" },
        { {1.0, 1.0}, {INFINITY, INFINITY}, {1.0, 1.0}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY}, {0.0, 0.0}, {INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN}, {1.0, 1.0}, {1.0, 1.0}, "NaN operand" },
        { {-0.0, -0.0}, {-0.0, -0.0}, {-0.0, -0.0}, "Negative zeros" },
        { {DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN}, "Denormal values" },
        { {DBL_MAX, DBL_MAX}, {2.0, 2.0}, {DBL_MAX, DBL_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_128) / sizeof(special_cases_128[0]);
    for (int t = 0; t < special_count; t++) {
        __m128d va = _mm_load_pd(special_cases_128[t].a);
        __m128d vb = _mm_load_pd(special_cases_128[t].b);
        double* c_ptr = special_cases_128[t].c;
        
        __asm__ __volatile__(
            "vfmaddsub213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*(const __m128d*)c_ptr)
        );
        
        double res[2];
        _mm_store_pd(res, va);
        
        double expected[2];
        calculate_expected(&special_cases_128[t], expected);
        
        printf("Special Test Case: %s (128-bit reg-mem)\n", special_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, special_cases_128[t].a[i], special_cases_128[t].b[i], special_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

// 计算预期值：偶数索引元素为 a*b - c，奇数索引元素为 a*b + c
static void calculate_expected_256(fma_test_case_256* test_case, double expected[4]) {
    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) {
            expected[i] = fma(test_case->a[i], test_case->b[i], -test_case->c[i]);
        } else {
            expected[i] = fma(test_case->a[i], test_case->b[i], test_case->c[i]);
        }
    }
}

static void test_reg_reg_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_load_pd(fma_cases_256[t].a);
        __m256d vb = _mm256_load_pd(fma_cases_256[t].b);
        __m256d vc = _mm256_load_pd(fma_cases_256[t].c);
        
        // 内联汇编实现 VFMADDSUB213PD (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmaddsub213pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_store_pd(res, va);
        
        double expected[4];
        calculate_expected_256(&fma_cases_256[t], expected);
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 256-bit reg-reg\n");
    fma_test_case_256 special_cases_256[] = {
        { {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, "All zeros" },
        { {1.0, 1.0, 1.0, 1.0}, {INFINITY, INFINITY, INFINITY, INFINITY}, {1.0, 1.0, 1.0, 1.0}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY, INFINITY, INFINITY}, {0.0, 0.0, 0.0, 0.0}, {INFINITY, INFINITY, INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN, NAN, NAN}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, "NaN operand" },
        { {-0.0, -0.0, -0.0, -0.0}, {-0.0, -0.0, -0.0, -0.0}, {-0.0, -0.0, -0.0, -0.0}, "Negative zeros" },
        { {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, "Denormal values" },
        { {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, {2.0, 2.0, 2.0, 2.0}, {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_256) / sizeof(special_cases_256[0]);
    for (int t = 0; t < special_count; t++) {
        __m256d va = _mm256_load_pd(special_cases_256[t].a);
        __m256d vb = _mm256_load_pd(special_cases_256[t].b);
        __m256d vc = _mm256_load_pd(special_cases_256[t].c);
        
        __asm__ __volatile__(
            "vfmaddsub213pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_store_pd(res, va);
        
        double expected[4];
        calculate_expected_256(&special_cases_256[t], expected);
        
        printf("Special Test Case: %s (256-bit reg-reg)\n", special_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, special_cases_256[t].a[i], special_cases_256[t].b[i], special_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_load_pd(fma_cases_256[t].a);
        __m256d vb = _mm256_load_pd(fma_cases_256[t].b);
        double* c_ptr = fma_cases_256[t].c;
        
        // 内联汇编实现 VFMADDSUB213PD (256位寄存器-内存)
        __asm__ __volatile__(
            "vfmaddsub213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*(const __m256d*)c_ptr)
        );
        
        double res[4];
        _mm256_store_pd(res, va);
        
        double expected[4];
        calculate_expected_256(&fma_cases_256[t], expected);
        
        printf("Test Case: %s (256-bit reg-mem)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
    
    // 添加特殊值测试
    printf("\nStarting Special Value Tests for 256-bit reg-mem\n");
    fma_test_case_256 special_cases_256[] = {
        { {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, "All zeros" },
        { {1.0, 1.0, 1.0, 1.0}, {INFINITY, INFINITY, INFINITY, INFINITY}, {1.0, 1.0, 1.0, 1.0}, "Infinity * finite + finite" },
        { {INFINITY, INFINITY, INFINITY, INFINITY}, {0.0, 0.0, 0.0, 0.0}, {INFINITY, INFINITY, INFINITY, INFINITY}, "Infinity + (0 * Infinity)" },
        { {NAN, NAN, NAN, NAN}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, "NaN operand" },
        { {-0.0, -0.0, -0.0, -0.0}, {-0.0, -0.0, -0.0, -0.0}, {-0.0, -0.0, -0.0, -0.0}, "Negative zeros" },
        { {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN}, "Denormal values" },
        { {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, {2.0, 2.0, 2.0, 2.0}, {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, "Overflow case" },
    };
    
    int special_count = sizeof(special_cases_256) / sizeof(special_cases_256[0]);
    for (int t = 0; t < special_count; t++) {
        __m256d va = _mm256_load_pd(special_cases_256[t].a);
        __m256d vb = _mm256_load_pd(special_cases_256[t].b);
        double* c_ptr = special_cases_256[t].c;
        
        __asm__ __volatile__(
            "vfmaddsub213pd %[c], %[b], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "m" (*(const __m256d*)c_ptr)
        );
        
        double res[4];
        _mm256_store_pd(res, va);
        
        double expected[4];
        calculate_expected_256(&special_cases_256[t], expected);
        
        printf("Special Test Case: %s (256-bit reg-mem)\n", special_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, special_cases_256[t].a[i], special_cases_256[t].b[i], special_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

int main() {
    printf("==================================\n");
    printf("VFMADDSUB213PD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_reg_reg_128();
    test_reg_mem_128();
    test_reg_reg_256();
    test_reg_mem_256();
    
    printf("All VFMADDSUB213PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
