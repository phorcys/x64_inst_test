#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 12

typedef struct {
    double a[4];
    double b[4];
    double c[4];
    const char *desc;
} test_case_256;

test_case_256 cases_256[TEST_CASE_COUNT] = {
    // 正常值
    {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0},
        "Normal values (256-bit)"
    },
    // 零值
    {
        {0.0, -0.0, 0.0, -0.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0},
        "Zero values (256-bit)"
    },
    // 无穷大
    {
        {INFINITY, -INFINITY, 1.0, 2.0},
        {1.0, 1.0, INFINITY, -INFINITY},
        {1.0, 1.0, 1.0, 1.0},
        "Infinity values (256-bit)"
    },
    // NaN
    {
        {NAN, 1.0, 2.0, 3.0},
        {1.0, NAN, 2.0, 3.0},
        {1.0, 2.0, NAN, 3.0},
        "NaN values (256-bit)"
    },
    // 边界值
    {
        {DBL_MIN, DBL_MAX, -DBL_MIN, -DBL_MAX},
        {2.0, 2.0, 2.0, 2.0},
        {DBL_MIN, DBL_MAX, -DBL_MIN, -DBL_MAX},
        "Boundary values (256-bit)"
    },
    // 混合值
    {
        {1.0, INFINITY, NAN, 0.0},
        {INFINITY, NAN, 0.0, NAN},
        {NAN, 0.0, INFINITY, 1.0},
        "Mixed special values (256-bit)"
    },
    // 小值
    {
        {1e-300, 2e-300, 3e-300, 4e-300},
        {2.0, 3.0, 4.0, 5.0},
        {1e-300, 2e-300, 3e-300, 4e-300},
        "Very small values (256-bit)"
    },
    // a为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN},
        {2.0, 3.0, 4.0, 5.0},
        {1.0, 2.0, 3.0, 4.0},
        "Special values in a (256-bit)"
    },
    // b为特殊值
    {
        {1.0, 2.0, 3.0, 4.0},
        {INFINITY, -INFINITY, NAN, -NAN},
        {5.0, 6.0, 7.0, 8.0},
        "Special values in b (256-bit)"
    },
    // c为特殊值
    {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {INFINITY, -INFINITY, NAN, -NAN},
        "Special values in c (256-bit)"
    },
    // a和b为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN},
        {NAN, 0.0, INFINITY, -INFINITY},
        {1.0, 2.0, 3.0, 4.0},
        "Special values in a and b (256-bit)"
    },
    // a和c为特殊值
    {
        {INFINITY, -INFINITY, NAN, -NAN},
        {1.0, 2.0, 3.0, 4.0},
        {NAN, 0.0, INFINITY, -INFINITY},
        "Special values in a and c (256-bit)"
    },        
    // b和c为特殊值
    {
        {1.0, 2.0, 3.0, 4.0},
        {INFINITY, -INFINITY, NAN, -NAN},
        {NAN, 0.0, INFINITY, -INFINITY},
        "Special values in a and c (256-bit)"
    },                
    // 所有特殊值
    {
        {INFINITY, -INFINITY, NAN, 0.0},
        {-NAN, 0.0, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, NAN, 0.0},
        "All special values (256-bit)"
    }
};

typedef struct {
    double a[2];
    double b[2];
    double c[2];
    const char *desc;
} test_case128;
uint32_t TEST_CASE_COUNT_128 = 7;

test_case128 test_case_128[7] = {
    // 正常值
    {
        {1.0, 2.0},
        {5.0, 6.0},
        {9.0, 10.0},
        "Normal values (128-bit)"
    },
    // 零值
    {
        {0.0, -0.0},
        {5.0, 6.0},
        {9.0, 10.0},
        "Zero values (128-bit)"
    },
    // 无穷大
    {
        {INFINITY, -INFINITY},
        {1.0, 1.0},
        {1.0, 1.0},
        "Infinity values (128-bit)"
    },
    // NaN
    {
        {NAN, 1.0},
        {1.0, NAN},
        {1.0, 2.0},
        "NaN values (128-bit)"
    },
    // 边界值
    {
        {DBL_MIN, DBL_MAX},
        {2.0, 2.0},
        {DBL_MIN, DBL_MAX},
        "Boundary values (128-bit)"
    },
    // 混合值
    {
        {1.0, INFINITY},
        {INFINITY, NAN},
        {NAN, 0.0},
        "Mixed special values (128-bit)"
    },
    // 小值
    {
        {1e-300, 2e-300},
        {2.0, 3.0},
        {1e-300, 2e-300},
        "Very small values (128-bit)"
    }
};

// 输出双精度浮点向量计算结果
static void print_double_vector_result(const char *desc, double *a, double *b, double *c, double *res, int count) {
    printf("%s:\n", desc);
    for (int i = 0; i < count; i++) {
        printf("  Element %d: a=%.6f (0x%016lx), b=%.6f (0x%016lx), c=%.6f (0x%016lx)\n",
               i, a[i], *(uint64_t*)&a[i], 
               b[i], *(uint64_t*)&b[i], 
               c[i], *(uint64_t*)&c[i]);
        printf("    Result: %.6f (0x%016lx) [Calculation: a * b + c]\n",
               res[i], *(uint64_t*)&res[i]);
    }
    printf("\n");
}

static void test_256bit_reg_reg_operand() {
   
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(cases_256[t].a);
        __m256d vb = _mm256_loadu_pd(cases_256[t].b);
        __m256d vc = _mm256_loadu_pd(cases_256[t].c);
        
        __asm__ __volatile__(
            "vfmadd132pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_storeu_pd(res, va);
        
        print_double_vector_result(cases_256[t].desc, cases_256[t].a, cases_256[t].b, cases_256[t].c, res, 4);
    }
    
    printf("VFMADD132PD 256-bit Register-Register Tests Completed\n\n");
}

static void test_128bit_reg_reg_operand() {

    
    for (int t = 0; t < 7; t++) {
        __m128d va = _mm_loadu_pd(test_case_128[t].a);
        __m128d vb = _mm_loadu_pd(test_case_128[t].b);
        __m128d vc = _mm_loadu_pd(test_case_128[t].c);
        
        __asm__ __volatile__(
            "vfmadd132pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_storeu_pd(res, va);
        
        print_double_vector_result(test_case_128[t].desc, test_case_128[t].a, test_case_128[t].b, test_case_128[t].c, res, 2);
    }
    
    printf("VFMADD132PD 128-bit Register-Register Tests Completed\n\n");
}

static void test_256bit_reg_mem_operand() {
   
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(cases_256[t].a);
        __m256d vc = _mm256_loadu_pd(cases_256[t].c);
        
        // 测试不同的内存寻址方式
        double *b_ptr = cases_256[t].b;
        double b_aligned[4] __attribute__((aligned(32)));
        memcpy(b_aligned, cases_256[t].b, sizeof(b_aligned));
        
        // 测试1: 未对齐内存
        __m256d va1 = va;
        __asm__ __volatile__(
            "vfmadd132pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m256d*)b_ptr), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m256d va2 = va;
        __asm__ __volatile__(
            "vfmadd132pd %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*(const __m256d*)b_aligned), [c] "x" (vc)
        );
        
        double res1[4], res2[4];
        _mm256_storeu_pd(res1, va1);
        _mm256_storeu_pd(res2, va2);
        
        printf("Memory Operand Test: %s\n", cases_256[t].desc);
        print_double_vector_result("  Unaligned memory", cases_256[t].a, cases_256[t].b, cases_256[t].c, res1, 4);
        print_double_vector_result("  Aligned memory", cases_256[t].a, cases_256[t].b, cases_256[t].c, res2, 4);
    }
    
    printf("VFMADD132PD 256-bit Register-Memory Tests Completed\n\n");
}

static void test_128bit_reg_mem_operand() {

    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(test_case_128[t].a);
        __m128d vc = _mm_loadu_pd(test_case_128[t].c);
        
        // 测试不同的内存寻址方式
        double *b_ptr = test_case_128[t].b;
        double b_aligned[2] __attribute__((aligned(16)));
        memcpy(b_aligned, test_case_128[t].b, sizeof(b_aligned));
        
        // 测试1: 未对齐内存
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfmadd132pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*(const __m128d*)b_ptr), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m128d va2 = va;
        __asm__ __volatile__(
            "vfmadd132pd %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*(const __m128d*)b_aligned), [c] "x" (vc)
        );
        
        double res1[2], res2[2];
        _mm_storeu_pd(res1, va1);
        _mm_storeu_pd(res2, va2);
        
        printf("Memory Operand Test: %s\n", test_case_128[t].desc);
        print_double_vector_result("  Unaligned memory", test_case_128[t].a, test_case_128[t].b, test_case_128[t].c, res1, 2);
        print_double_vector_result("  Aligned memory", test_case_128[t].a, test_case_128[t].b, test_case_128[t].c, res2, 2);
    }
    
    printf("VFMADD132PD 128-bit Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADD132PD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_128bit_reg_reg_operand();
    test_256bit_reg_reg_operand();
    test_128bit_reg_mem_operand();
    test_256bit_reg_mem_operand();
    
    printf("All VFMADD132PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
