#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 14

typedef struct {
    double a;
    double b;
    double c;
    const char *desc;
} test_case;

test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {1.0, 2.0, 3.0, "Normal values"},
    // 零值
    {0.0, -0.0, 0.0, "Zero values"},
    // 无穷大
    {INFINITY, 1.0, 1.0, "Infinity values"},
    // NaN
    {NAN, 2.0, 3.0, "NaN values"},
    // 边界值
    {DBL_MIN, -DBL_MIN, DBL_MIN, "Boundary values"},
    // 混合值
    {1.0, NAN, INFINITY, "Mixed special values"},
    // 小值
    {1e-300, 2e-300, 3e-300, "Very small values"},
    // a为特殊值
    {INFINITY, 2.0, 3.0, "Special value in a"},
    // b为特殊值
    {1.0, NAN, 2.0, "Special value in b"},
    // c为特殊值
    {1.0, 2.0, -INFINITY, "Special value in c"},
    // a和b为特殊值
    {INFINITY, NAN, 1.0, "Special values in a and b"},
    // a和c为特殊值
    {NAN, 1.0, INFINITY, "Special values in a and c"},
    // 所有特殊值
    {INFINITY, NAN, -INFINITY, "All special values"}
};

static void test_reg_reg_operand() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m128d va = _mm_set_sd(cases[t].a);
        __m128d vb = _mm_set_sd(cases[t].b);
        __m128d vc = _mm_set_sd(cases[t].c);
        
        __asm__ __volatile__(
            "vfmsub231sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
        
        double res;
        _mm_store_sd(&res, va);
        
        printf("Test Case: %s\n", cases[t].desc);
        printf("A     : %.17g\n", cases[t].a);
        printf("B     : %.17g\n", cases[t].b);
        printf("C     : %.17g\n", cases[t].c);
        printf("Result: %.17g\n\n", res);
    }
    
    printf("VFMSUB231SD Register-Register Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("VFMSUB231SD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_reg_reg_operand();
    
    printf("All VFMSUB231SD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
