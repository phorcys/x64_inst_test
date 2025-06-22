#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 14

typedef struct {
    double a[2];
    double b[2];
    double c[2];
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {{1.0, 2.0}, {5.0, 6.0}, {9.0, 10.0}, "Normal values"},
    // 零值
    {{0.0, 0.0}, {5.0, 6.0}, {9.0, 10.0}, "Zero values"},
    // 无穷大
    {{INFINITY, INFINITY}, {1.0, 1.0}, {1.0, 1.0}, "Infinity values"},
    // NaN
    {{NAN, NAN}, {1.0, 1.0}, {2.0, 2.0}, "NaN values"},
    // 边界值
    {{DBL_MIN, DBL_MIN}, {2.0, 2.0}, {DBL_MIN, DBL_MIN}, "Boundary values"},
    // 混合值
    {{1.0, 1.0}, {INFINITY, INFINITY}, {NAN, NAN}, "Mixed special values"},
    // 小值
    {{1e-300, 1e-300}, {2.0, 2.0}, {1e-300, 1e-300}, "Very small values"},
    // a为特殊值
    {{INFINITY, INFINITY}, {2.0, 2.0}, {1.0, 1.0}, "Special values in a"},
    // b为特殊值
    {{1.0, 1.0}, {INFINITY, INFINITY}, {5.0, 5.0}, "Special values in b"},
    // c为特殊值
    {{1.0, 1.0}, {5.0, 5.0}, {INFINITY, INFINITY}, "Special values in c"},
    // a和b为特殊值
    {{INFINITY, INFINITY}, {NAN, NAN}, {1.0, 1.0}, "Special values in a and b"},
    // a和c为特殊值
    {{INFINITY, INFINITY}, {1.0, 1.0}, {NAN, NAN}, "Special values in a and c"},
    // b和c为特殊值
    {{1.0, 1.0}, {INFINITY, INFINITY}, {NAN, NAN}, "Special values in b and c"},
    // 所有特殊值
    {{INFINITY, INFINITY}, {NAN, NAN}, {INFINITY, INFINITY}, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(cases[t].a);
        __m128d vb = _mm_loadu_pd(cases[t].b);
        __m128d vc = _mm_loadu_pd(cases[t].c);
        
        __asm__ __volatile__(
            "vfnmsub213pd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res[2];
        _mm_storeu_pd(res, va);
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : [%.17g, %.17g]\n", cases[t].a[0], cases[t].a[1]);
        printf("B     : [%.17g, %.17g]\n", cases[t].b[0], cases[t].b[1]);
        printf("C     : [%.17g, %.17g]\n", cases[t].c[0], cases[t].c[1]);
        printf("Result: [%.17g, %.17g]\n\n", res[0], res[1]);
    }
    
    printf("VFNMSUB213PD Register-Register Tests Completed\n\n");
}

static void test_reg_mem_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_loadu_pd(cases[t].a);
        __m128d vc = _mm_loadu_pd(cases[t].c);
        
        // 测试内存操作数
        __m128d va1 = va;
        __asm__ __volatile__(
            "vfnmsub213pd %[b], %[c], %[a]"
            : [a] "+x" (va1)
            : [b] "m" (cases[t].b), [c] "x" (vc)
        );
        
        double res[2];
        _mm_storeu_pd(res, va1);
        
        printf("Memory Operand Test: %s\n", cases[t].desc);
        printf("A     : [%.17g, %.17g]\n", cases[t].a[0], cases[t].a[1]);
        printf("B     : [%.17g, %.17g]\n", cases[t].b[0], cases[t].b[1]);
        printf("C     : [%.17g, %.17g]\n", cases[t].c[0], cases[t].c[1]);
        printf("Result: [%.17g, %.17g]\n\n", res[0], res[1]);
    }
    
    printf("VFNMSUB213PD Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFNMSUB213PD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    // 执行测试
    test_reg_reg_operand();
    test_reg_mem_operand();
    
    printf("All VFNMSUB213PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
