#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 14

typedef struct {
    float a[4];
    float b[4];
    float c[4];
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {{1.0f, 2.0f, 3.0f, 4.0f}, {5.0f, 6.0f, 7.0f, 8.0f}, {9.0f, 10.0f, 11.0f, 12.0f}, "Normal values"},
    // 零值
    {{0.0f, -0.0f, 0.0f, -0.0f}, {0.0f, -0.0f, 0.0f, -0.0f}, {0.0f, -0.0f, 0.0f, -0.0f}, "Zero values"},
    // 无穷大
    {{INFINITY, -INFINITY, 1.0f, 2.0f}, {1.0f, 1.0f, INFINITY, -INFINITY}, {1.0f, 1.0f, 1.0f, 1.0f}, "Infinity values"},
    // NaN
    {{NAN, 1.0f, 2.0f, 3.0f}, {1.0f, NAN, 2.0f, 3.0f}, {1.0f, 2.0f, NAN, 3.0f}, "NaN values"},
    // 边界值
    {{FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX}, {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX}, {FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX}, "Boundary values"},
    // 混合值
    {{1.0f, INFINITY, NAN, 0.0f}, {INFINITY, NAN, 0.0f, NAN}, {NAN, 0.0f, INFINITY, 1.0f}, "Mixed special values"},
    // 小值
    {{1e-30f, 2e-30f, 3e-30f, 4e-30f}, {5e-30f, 6e-30f, 7e-30f, 8e-30f}, {9e-30f, 10e-30f, 11e-30f, 12e-30f}, "Very small values"},
    // a为特殊值
    {{INFINITY, NAN, 1.0f, 2.0f}, {3.0f, 4.0f, 5.0f, 6.0f}, {7.0f, 8.0f, 9.0f, 10.0f}, "Special value in a"},
    // b为特殊值
    {{1.0f, 2.0f, 3.0f, 4.0f}, {NAN, INFINITY, -INFINITY, NAN}, {5.0f, 6.0f, 7.0f, 8.0f}, "Special value in b"},
    // c为特殊值
    {{1.0f, 2.0f, 3.0f, 4.0f}, {5.0f, 6.0f, 7.0f, 8.0f}, {-INFINITY, NAN, INFINITY, -NAN}, "Special value in c"},
    // a和b为特殊值
    {{INFINITY, NAN, -INFINITY, NAN}, {NAN, INFINITY, NAN, -INFINITY}, {1.0f, 2.0f, 3.0f, 4.0f}, "Special values in a and b"},
    // a和c为特殊值
    {{NAN, INFINITY, -NAN, -INFINITY}, {1.0f, 2.0f, 3.0f, 4.0f}, {INFINITY, NAN, -INFINITY, NAN}, "Special values in a and c"},
    // 所有特殊值
    {{INFINITY, NAN, -INFINITY, NAN}, {NAN, INFINITY, NAN, -INFINITY}, {-INFINITY, NAN, INFINITY, -NAN}, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases[t].a);
        __m128 vb = _mm_loadu_ps(cases[t].b);
        __m128 vc = _mm_loadu_ps(cases[t].c);
        
        __asm__ __volatile__(
            "vfmadd213ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_storeu_ps(res, va);
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : %.9g %.9g %.9g %.9g\n", cases[t].a[0], cases[t].a[1], cases[t].a[2], cases[t].a[3]);
        printf("B     : %.9g %.9g %.9g %.9g\n", cases[t].b[0], cases[t].b[1], cases[t].b[2], cases[t].b[3]);
        printf("C     : %.9g %.9g %.9g %.9g\n", cases[t].c[0], cases[t].c[1], cases[t].c[2], cases[t].c[3]);
        printf("Result: %.9g %.9g %.9g %.9g\n\n", res[0], res[1], res[2], res[3]);
    }
    
    printf("VFMADD213PS Register-Register Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMADD213PS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_reg_reg_operand();
    
    printf("All VFMADD213PS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
