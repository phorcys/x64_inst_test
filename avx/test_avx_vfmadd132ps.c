#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 15
#define FLOAT_TOLERANCE 1e-6f

typedef struct {
    float a[8];
    float b[8];
    float c[8];
    float expected[8];
    const char *desc;
} test_case;

// 比较两个单精度浮点向量是否相等（带容差）
static int compare_float_vector(float *res, float *expected, const char *desc, float *a, float *b, float *c, int count) {
    int pass = 1;
    
    printf("%s:\n", desc);
    for (int i = 0; i < count; i++) {
        int is_nan = isnan(res[i]) && isnan(expected[i]);
        int is_inf = isinf(res[i]) && isinf(expected[i]) && (signbit(res[i]) == signbit(expected[i]));
        int is_equal = fabsf(res[i] - expected[i]) <= FLOAT_TOLERANCE;
        
        int ok = is_nan || is_inf || is_equal;
        
        printf("  Element %d: a=%.6f (0x%08x), b=%.6f (0x%08x), c=%.6f (0x%08x)\n",
               i, a[i], *(uint32_t*)&a[i], 
               b[i], *(uint32_t*)&b[i], 
               c[i], *(uint32_t*)&c[i]);
        printf("    Expected: %.6f (0x%08x), Result: %.6f (0x%08x) - %s\n",
               expected[i], *(uint32_t*)&expected[i],
               res[i], *(uint32_t*)&res[i],
               ok ? "PASS" : "FAIL");
        
        if (!ok) pass = 0;
    }
    printf("\n");
    return pass;
}

static void test_256bit() {
    test_case cases[TEST_CASE_COUNT] = {
        // 128位正常值
        {
            {1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {0.5f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {1.0f*0.5f+1.0f, 0.5f*1.0f+1.0f, 1.0f*0.5f+1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            "Normal values (128-bit)"
        },
        // 256位正常值
        {
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
            {0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f},
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
            {1.0f*0.5f+1.0f, 2.0f*1.5f+2.0f, 3.0f*2.5f+3.0f, 4.0f*3.5f+4.0f,
             5.0f*4.5f+5.0f, 6.0f*5.5f+6.0f, 7.0f*6.5f+7.0f, 8.0f*7.5f+8.0f},
            "Normal values (256-bit)"
        },
        // 零值
        {
            {0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f},
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
            {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f},
            {0.0f*1.0f+0.1f, -0.0f*2.0f+0.2f, 0.0f*3.0f+0.3f, -0.0f*4.0f+0.4f,
             0.0f*5.0f+0.5f, -0.0f*6.0f+0.6f, 0.0f*7.0f+0.7f, -0.0f*8.0f+0.8f},
            "Zero values"
        },
        // 无穷大
        {
            {INFINITY, -INFINITY, 1.0f, 2.0f, INFINITY, -INFINITY, 1.0f, 2.0f},
            {1.0f, 1.0f, INFINITY, -INFINITY, 1.0f, 1.0f, INFINITY, -INFINITY},
            {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
            {INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 1.0f*INFINITY+1.0f, 2.0f*(-INFINITY)+1.0f,
             INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 1.0f*INFINITY+1.0f, 2.0f*(-INFINITY)+1.0f},
            "Infinity values"
        },
        // NaN
        {
            {NAN, 1.0f, 2.0f, 3.0f, NAN, 1.0f, 2.0f, 3.0f},
            {1.0f, NAN, 2.0f, 3.0f, 1.0f, NAN, 2.0f, 3.0f},
            {1.0f, 2.0f, NAN, 3.0f, 1.0f, 2.0f, NAN, 3.0f},
            {NAN, NAN, NAN, 3.0f*3.0f+3.0f, NAN, NAN, NAN, 3.0f*3.0f+3.0f},
            "NaN values"
        },
        // 边界值
        {
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {2.0f, 0.5f, 2.0f, 0.5f, 2.0f, 0.5f, 2.0f, 0.5f},
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {FLT_MIN*2.0f+FLT_MIN, FLT_MAX*0.5f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*0.5f-FLT_MAX,
             FLT_MIN*2.0f+FLT_MIN, FLT_MAX*0.5f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*0.5f-FLT_MAX},
            "Boundary values"
        },
        // 混合特殊值
        {
            {INFINITY, NAN, 0.0f, -0.0f, INFINITY, NAN, 0.0f, -0.0f},
            {NAN, INFINITY, 1.0f, 1.0f, NAN, INFINITY, 1.0f, 1.0f},
            {0.0f, 0.0f, INFINITY, -INFINITY, 0.0f, 0.0f, INFINITY, -INFINITY},
            {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
            "Mixed special values"
        },
        // 极小值
        {
            {1e-30f, 2e-30f, 3e-30f, 4e-30f, 5e-30f, 6e-30f, 7e-30f, 8e-30f},
            {2.0f, 3.0f, 4.0f, 5.0f, 2.0f, 3.0f, 4.0f, 5.0f},
            {1e-30f, 2e-30f, 3e-30f, 4e-30f, 5e-30f, 6e-30f, 7e-30f, 8e-30f},
            {1e-30f*2.0f+1e-30f, 2e-30f*3.0f+2e-30f, 3e-30f*4.0f+3e-30f, 4e-30f*5.0f+4e-30f,
             5e-30f*2.0f+5e-30f, 6e-30f*3.0f+6e-30f, 7e-30f*4.0f+7e-30f, 8e-30f*5.0f+8e-30f},
            "Very small values"
        },
        // 128位边界值
        {
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, 0.0f, 0.0f, 0.0f, 0.0f}, 
            {2.0f, 0.5f, 2.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f}, 
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, 0.0f, 0.0f, 0.0f, 0.0f},
            {FLT_MIN*2.0f+FLT_MIN, FLT_MAX*0.5f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*0.5f-FLT_MAX, 0.0f, 0.0f, 0.0f, 0.0f},
            "Boundary values (128-bit)"
        },
        // 128位特殊值
        {
            {INFINITY, -INFINITY, 0.0f, -0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, INFINITY, -INFINITY, 0.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 0.0f*INFINITY+0.0f, -0.0f*(-INFINITY)+0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            "Special values (128-bit)"
        }
    };
    
    int total_pass = 1;
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(cases[t].a);
        __m256 vb = _mm256_loadu_ps(cases[t].b);
        __m256 vc = _mm256_loadu_ps(cases[t].c);
        
        __asm__ __volatile__(
            "vfmadd132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_storeu_ps(res, va);
        
        int case_pass = compare_float_vector(res, cases[t].expected, cases[t].desc, 
                                            cases[t].a, cases[t].b, cases[t].c, 8);
        if (!case_pass) total_pass = 0;
    }
    
    printf("VFMADD132PS 256-bit Register-Register Tests: %s\n\n", total_pass ? "ALL PASS" : "SOME FAIL");
}

static void test_128bit() {
    test_case cases[TEST_CASE_COUNT] = {
        // 正常值
        {
            {1.0f, 2.0f, 3.0f, 4.0f},
            {0.5f, 1.5f, 2.5f, 3.5f},
            {1.0f, 2.0f, 3.0f, 4.0f},
            {1.0f*0.5f+1.0f, 2.0f*1.5f+2.0f, 3.0f*2.5f+3.0f, 4.0f*3.5f+4.0f},
            "Normal values (128-bit)"
        },
        // 零值
        {
            {0.0f, -0.0f, 0.0f, -0.0f},
            {1.0f, 2.0f, 3.0f, 4.0f},
            {0.1f, 0.2f, 0.3f, 0.4f},
            {0.0f*1.0f+0.1f, -0.0f*2.0f+0.2f, 0.0f*3.0f+0.3f, -0.0f*4.0f+0.4f},
            "Zero values (128-bit)"
        },
        // 无穷大
        {
            {INFINITY, -INFINITY, 1.0f, 2.0f},
            {1.0f, 1.0f, INFINITY, -INFINITY},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 1.0f*INFINITY+1.0f, 2.0f*(-INFINITY)+1.0f},
            "Infinity values (128-bit)"
        },
        // NaN
        {
            {NAN, 1.0f, 2.0f, 3.0f},
            {1.0f, NAN, 2.0f, 3.0f},
            {1.0f, 2.0f, NAN, 3.0f},
            {NAN, NAN, NAN, 3.0f*3.0f+3.0f},
            "NaN values (128-bit)"
        },
        // 边界值
        {
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {2.0f, 2.0f, 2.0f, 2.0f},
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {FLT_MIN*2.0f+FLT_MIN, FLT_MAX*2.0f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*2.0f-FLT_MAX},
            "Boundary values (128-bit)"
        },
        // 混合值
        {
            {1.0f, INFINITY, NAN, 0.0f},
            {INFINITY, NAN, 0.0f, NAN},
            {NAN, 0.0f, INFINITY, 1.0f},
            {NAN, NAN, NAN, NAN},
            "Mixed special values (128-bit)"
        },
        // 小值
        {
            {1e-30f, 2e-30f, 3e-30f, 4e-30f},
            {2.0f, 3.0f, 4.0f, 5.0f},
            {1e-30f, 2e-30f, 3e-30f, 4e-30f},
            {1e-30f*2.0f+1e-30f, 2e-30f*3.0f+2e-30f, 3e-30f*4.0f+3e-30f, 4e-30f*5.0f+4e-30f},
            "Very small values (128-bit)"
        }
    };
    
    int total_pass = 1;
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases[t].a);
        __m128 vb = _mm_loadu_ps(cases[t].b);
        __m128 vc = _mm_loadu_ps(cases[t].c);
        
        __asm__ __volatile__(
            "vfmadd132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_storeu_ps(res, va);
        
        int case_pass = compare_float_vector(res, cases[t].expected, cases[t].desc, 
                                            cases[t].a, cases[t].b, cases[t].c, 4);
        if (!case_pass) total_pass = 0;
    }
    
    printf("VFMADD132PS 128-bit Register-Register Tests: %s\n\n", total_pass ? "ALL PASS" : "SOME FAIL");
}

static void test_256bit_mem_operand() {
    test_case cases[TEST_CASE_COUNT] = {
        // 正常值
        {
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
            {0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f},
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
            {1.0f*0.5f+1.0f, 2.0f*1.5f+2.0f, 3.0f*2.5f+3.0f, 4.0f*3.5f+4.0f,
             5.0f*4.5f+5.0f, 6.0f*5.5f+6.0f, 7.0f*6.5f+7.0f, 8.0f*7.5f+8.0f},
            "Normal values (256-bit memory)"
        },
        // 零值
        {
            {0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f},
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
            {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f},
            {0.0f*1.0f+0.1f, -0.0f*2.0f+0.2f, 0.0f*3.0f+0.3f, -0.0f*4.0f+0.4f,
             0.0f*5.0f+0.5f, -0.0f*6.0f+0.6f, 0.0f*7.0f+0.7f, -0.0f*8.0f+0.8f},
            "Zero values (256-bit memory)"
        },
        // 无穷大
        {
            {INFINITY, -INFINITY, 1.0f, 2.0f, INFINITY, -INFINITY, 1.0f, 2.0f},
            {1.0f, 1.0f, INFINITY, -INFINITY, 1.0f, 1.0f, INFINITY, -INFINITY},
            {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
            {INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 1.0f*INFINITY+1.0f, 2.0f*(-INFINITY)+1.0f,
             INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 1.0f*INFINITY+1.0f, 2.0f*(-INFINITY)+1.0f},
            "Infinity values (256-bit memory)"
        },
        // NaN
        {
            {NAN, 1.0f, 2.0f, 3.0f},
            {1.0f, NAN, 2.0f, 3.0f},
            {1.0f, 2.0f, NAN, 3.0f},
            {NAN, NAN, NAN, 3.0f*3.0f+3.0f},
            "NaN values (128-bit)"
        },
        // 边界值
        {
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f},
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX, FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {FLT_MIN*2.0f+FLT_MIN, FLT_MAX*2.0f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*2.0f-FLT_MAX,
             FLT_MIN*2.0f+FLT_MIN, FLT_MAX*2.0f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*2.0f-FLT_MAX},
            "Boundary values (256-bit memory)"
        },
        // 混合值
        {
            {1.0f, INFINITY, NAN, 0.0f, 1.0f, INFINITY, NAN, 0.0f},
            {INFINITY, NAN, 0.0f, NAN, INFINITY, NAN, 0.0f, NAN},
            {NAN, 0.0f, INFINITY, 1.0f, NAN, 0.0f, INFINITY, 1.0f},
            {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
            "Mixed special values (256-bit memory)"
        },
        // 小值
        {
            {1e-30f, 2e-30f, 3e-30f, 4e-30f, 5e-30f, 6e-30f, 7e-30f, 8e-30f},
            {2.0f, 3.0f, 4.0f, 5.0f, 2.0f, 3.0f, 4.0f, 5.0f},
            {1e-30f, 2e-30f, 3e-30f, 4e-30f, 5e-30f, 6e-30f, 7e-30f, 8e-30f},
            {1e-30f*2.0f+1e-30f, 2e-30f*3.0f+2e-30f, 3e-30f*4.0f+3e-30f, 4e-30f*5.0f+4e-30f,
             5e-30f*2.0f+5e-30f, 6e-30f*3.0f+6e-30f, 7e-30f*4.0f+7e-30f, 8e-30f*5.0f+8e-30f},
            "Very small values (256-bit memory)"
        }
    };
    
    int total_pass = 1;
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(cases[t].a);
        __m256 vc = _mm256_loadu_ps(cases[t].c);
        
        // 测试不同的内存寻址方式
        float *b_ptr = cases[t].b;
        float b_aligned[8] __attribute__((aligned(32)));
        memcpy(b_aligned, cases[t].b, sizeof(b_aligned));
        
        // 测试1: 未对齐内存
        __m256 va1 = va;
        __asm__ __volatile__(
            "vfmadd132ps %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m256 va2 = va;
        __asm__ __volatile__(
            "vfmadd132ps %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*b_aligned), [c] "x" (vc)
        );
        
        float res1[8], res2[8];
        _mm256_storeu_ps(res1, va1);
        _mm256_storeu_ps(res2, va2);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        int case_pass1 = compare_float_vector(res1, cases[t].expected, "  Unaligned memory", 
                                            cases[t].a, cases[t].b, cases[t].c, 8);
        int case_pass2 = compare_float_vector(res2, cases[t].expected, "  Aligned memory", 
                                            cases[t].a, cases[t].b, cases[t].c, 8);
        
        if (!case_pass1 || !case_pass2) total_pass = 0;
    }
    
    printf("VFMADD132PS 256-bit Memory Operand Tests: %s\n\n", total_pass ? "ALL PASS" : "SOME FAIL");
}

static void test_128bit_mem_operand() {
    test_case cases[TEST_CASE_COUNT] = {
        // 正常值
        {
            {1.0f, 2.0f, 3.0f, 4.0f},
            {0.5f, 1.5f, 2.5f, 3.5f},
            {1.0f, 2.0f, 3.0f, 4.0f},
            {1.0f*0.5f+1.0f, 2.0f*1.5f+2.0f, 3.0f*2.5f+3.0f, 4.0f*3.5f+4.0f},
            "Normal values (128-bit memory)"
        },
        // 零值
        {
            {0.0f, -0.0f, 0.0f, -0.0f},
            {1.0f, 2.0f, 3.0f, 4.0f},
            {0.1f, 0.2f, 0.3f, 0.4f},
            {0.0f*1.0f+0.1f, -0.0f*2.0f+0.2f, 0.0f*3.0f+0.3f, -0.0f*4.0f+0.4f},
            "Zero values (128-bit memory)"
        },
        // 无穷大
        {
            {INFINITY, -INFINITY, 1.0f, 2.0f},
            {1.0f, 1.0f, INFINITY, -INFINITY},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {INFINITY*1.0f+1.0f, (-INFINITY)*1.0f+1.0f, 1.0f*INFINITY+1.0f, 2.0f*(-INFINITY)+1.0f},
            "Infinity values (128-bit memory)"
        },
        // NaN
        {
            {NAN, 1.0f, 2.0f, 3.0f},
            {1.0f, NAN, 2.0f, 3.0f},
            {1.0f, 2.0f, NAN, 3.0f},
            {NAN, NAN, NAN, 3.0f*3.0f+3.0f},
            "NaN values (128-bit memory)"
        },
        // 边界值
        {
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {2.0f, 2.0f, 2.0f, 2.0f},
            {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX},
            {FLT_MIN*2.0f+FLT_MIN, FLT_MAX*2.0f+FLT_MAX, -FLT_MIN*2.0f-FLT_MIN, -FLT_MAX*2.0f-FLT_MAX},
            "Boundary values (128-bit memory)"
        },
        // 混合值
        {
            {1.0f, INFINITY, NAN, 0.0f},
            {INFINITY, NAN, 0.0f, NAN},
            {NAN, 0.0f, INFINITY, 1.0f},
            {NAN, NAN, NAN, NAN},
            "Mixed special values (128-bit memory)"
        },
        // 小值
        {
            {1e-30f, 2e-30f, 3e-30f, 4e-30f},
            {2.0f, 3.0f, 4.0f, 5.0f},
            {1e-30f, 2e-30f, 3e-30f, 4e-30f},
            {1e-30f*2.0f+1e-30f, 2e-30f*3.0f+2e-30f, 3e-30f*4.0f+3e-30f, 4e-30f*5.0f+4e-30f},
            "Very small values (128-bit memory)"
        }
    };
    
    int total_pass = 1;
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases[t].a);
        __m128 vc = _mm_loadu_ps(cases[t].c);
        
        // 测试不同的内存寻址方式
        float *b_ptr = cases[t].b;
        float b_aligned[4] __attribute__((aligned(16)));
        memcpy(b_aligned, cases[t].b, sizeof(b_aligned));
        
        // 测试1: 未对齐内存
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfmadd132ps %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (*b_ptr), [c] "x" (vc)
        );
        
        // 测试2: 对齐内存
        __m128 va2 = va;
        __asm__ __volatile__(
            "vfmadd132ps %[b], %[c], %[a]"
            : [a] "+x" (va2)
            : [b] "m" (*b_aligned), [c] "x" (vc)
        );
        
        float res1[4], res2[4];
        _mm_storeu_ps(res1, va1);
        _mm_storeu_ps(res2, va2);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        int case_pass1 = compare_float_vector(res1, cases[t].expected, "  Unaligned memory", 
                                            cases[t].a, cases[t].b, cases[t].c, 4);
        int case_pass2 = compare_float_vector(res2, cases[t].expected, "  Aligned memory", 
                                            cases[t].a, cases[t].b, cases[t].c, 4);
        
        if (!case_pass1 || !case_pass2) total_pass = 0;
    }
    
    printf("VFMADD132PS 128-bit Memory Operand Tests: %s\n\n", total_pass ? "ALL PASS" : "SOME FAIL");
}

int main() {
    printf("==================================\n");
    printf("VFMADD132PS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_128bit();
    test_256bit();
    test_128bit_mem_operand();
    test_256bit_mem_operand();
    
    return 0;
}
