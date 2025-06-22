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
    {{0.0f, 0.0f, 0.0f, 0.0f}, {5.0f, 6.0f, 7.0f, 8.0f}, {9.0f, 10.0f, 11.0f, 12.0f}, "Zero values"},
    // 无穷大
    {{INFINITY, INFINITY, INFINITY, INFINITY}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, "Infinity values"},
    // NaN
    {{NAN, NAN, NAN, NAN}, {1.0f, 1.0f, 1.0f, 1.0f}, {2.0f, 2.0f, 2.0f, 2.0f}, "NaN values"},
    // 边界值
    {{FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, {2.0f, 2.0f, 2.0f, 2.0f}, {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN}, "Boundary values"},
    // 混合值
    {{1.0f, 1.0f, 1.0f, 1.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, {NAN, NAN, NAN, NAN}, "Mixed special values"},
    // 小值
    {{1e-30f, 1e-30f, 1e-30f, 1e-30f}, {2.0f, 2.0f, 2.0f, 2.0f}, {1e-30f, 1e-30f, 1e-30f, 1e-30f}, "Very small values"},
    // a为特殊值
    {{INFINITY, INFINITY, INFINITY, INFINITY}, {2.0f, 2.0f, 2.0f, 2.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, "Special values in a"},
    // b为特殊值
    {{1.0f, 1.0f, 1.0f, 1.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, {5.0f, 5.0f, 5.0f, 5.0f}, "Special values in b"},
    // c为特殊值
    {{1.0f, 1.0f, 1.0f, 1.0f}, {5.0f, 5.0f, 5.0f, 5.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, "Special values in c"},
    // a和b为特殊值
    {{INFINITY, INFINITY, INFINITY, INFINITY}, {NAN, NAN, NAN, NAN}, {1.0f, 1.0f, 1.0f, 1.0f}, "Special values in a and b"},
    // a和c为特殊值
    {{INFINITY, INFINITY, INFINITY, INFINITY}, {1.0f, 1.0f, 1.0f, 1.0f}, {NAN, NAN, NAN, NAN}, "Special values in a and c"},
    // b和c为特殊值
    {{1.0f, 1.0f, 1.0f, 1.0f}, {INFINITY, INFINITY, INFINITY, INFINITY}, {NAN, NAN, NAN, NAN}, "Special values in b and c"},
    // 所有特殊值
    {{INFINITY, INFINITY, INFINITY, INFINITY}, {NAN, NAN, NAN, NAN}, {INFINITY, INFINITY, INFINITY, INFINITY}, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases[t].a);
        __m128 vb = _mm_loadu_ps(cases[t].b);
        __m128 vc = _mm_loadu_ps(cases[t].c);
        
        __asm__ __volatile__(
            "vfnmsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        float res[4];
        _mm_storeu_ps(res, va);
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : [%.7g, %.7g, %.7g, %.7g]\n", cases[t].a[0], cases[t].a[1], cases[t].a[2], cases[t].a[3]);
        printf("B     : [%.7g, %.7g, %.7g, %.7g]\n", cases[t].b[0], cases[t].b[1], cases[t].b[2], cases[t].b[3]);
        printf("C     : [%.7g, %.7g, %.7g, %.7g]\n", cases[t].c[0], cases[t].c[1], cases[t].c[2], cases[t].c[3]);
        printf("Result: [%.7g, %.7g, %.7g, %.7g]\n\n", res[0], res[1], res[2], res[3]);
    }
    
    printf("VFNMSUB132PS Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128 va = _mm_loadu_ps(cases[t].a);
        __m128 vc = _mm_loadu_ps(cases[t].c);
        
        // 准备内存操作数
        float b[4] = {cases[t].b[0], cases[t].b[1], cases[t].b[2], cases[t].b[3]};
        
        // 测试内存操作数
        __m128 va1 = va;
        __asm__ __volatile__(
            "vfnmsub132ps %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (b), [c] "x" (vc)
        );
        
        float res[4];
        _mm_storeu_ps(res, va1);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        printf("A     : [%.7g, %.7g, %.7g, %.7g]\n", cases[t].a[0], cases[t].a[1], cases[t].a[2], cases[t].a[3]);
        printf("B     : [%.7g, %.7g, %.7g, %.7g]\n", cases[t].b[0], cases[t].b[1], cases[t].b[2], cases[t].b[3]);
        printf("C     : [%.7g, %.7g, %.7g, %.7g]\n", cases[t].c[0], cases[t].c[1], cases[t].c[2], cases[t].c[3]);
        printf("Result: [%.7g, %.7g, %.7g, %.7g]\n\n", res[0], res[1], res[2], res[3]);
    }
    
    printf("VFNMSUB132PS Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFNMSUB132PS Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFNMSUB132PS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
