#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <float.h>
#include "avx.h"

#define TEST_CASE_COUNT 15

typedef struct {
    double a;
    double b;
    double c;
    double expected;
    const char *desc;
    int use_memory;
} test_case;

// 辅助函数：将双精度浮点数转换为可读字符串
static const char* get_double_str(double d) {
    if (isnan(d)) return "nan";
    if (isinf(d)) return d < 0 ? "-inf" : "inf";
    if (d == 0.0) return signbit(d) ? "-0.0" : "0.0";
    return "value";
}

// 比较双精度浮点数，处理特殊值
static int compare_doubles(double res, double expected) {
    if (isnan(res) && isnan(expected)) return 1;
    if (isinf(res) && isinf(expected) && signbit(res) == signbit(expected)) return 1;
    if (res == 0.0 && expected == 0.0 && signbit(res) == signbit(expected)) return 1;
    return fabs(res - expected) <= 1e-12 * fabs(expected);
}

// 统一的测试用例
static test_case cases[TEST_CASE_COUNT] = {
    // 正常值
    {1.0, 0.5, 1.0, 1.0*0.5+1.0, "Normal values (Register)", 0},
    {1.0, 0.5, 1.0, 1.0*0.5+1.0, "Normal values (Memory)", 1},
    
    // 零值
    {0.0, 1.0, 0.1, 0.0*1.0+0.1, "Zero values (a=0)", 0},
    {-0.0, 2.0, 0.2, -0.0*2.0+0.2, "Zero values (a=-0)", 1},
    
    // 无穷大
    {INFINITY, 1.0, 1.0, INFINITY, "Infinity values (a=inf)", 0},
    {-INFINITY, 1.0, 1.0, -INFINITY, "Infinity values (a=-inf)", 1},
    
    // NaN
    {NAN, 1.0, 1.0, NAN, "NaN values (a=nan)", 0},
    {1.0, NAN, 2.0, NAN, "NaN values (b=nan)", 1},
    
    // 边界值
    {DBL_MIN, 2.0, DBL_MIN, DBL_MIN*2.0 + DBL_MIN, "Boundary values (min)", 0},
    {DBL_MAX, 0.5, DBL_MAX, DBL_MAX*0.5 + DBL_MAX, "Boundary values (max)", 1},
    
    // 混合特殊值
    {INFINITY, NAN, 0.0, NAN, "Mixed special values (inf, nan)", 0},
    {NAN, INFINITY, 1.0, NAN, "Mixed special values (nan, inf)", 1},
    {0.0, INFINITY, 0.0, NAN, "Mixed special values (0, inf)", 0},
    {INFINITY, 0.0, 1.0, NAN, "Mixed special values (inf, 0)", 1},
    
    // 极小值
    {1e-308, 2.0, 1e-308, 1e-308*2.0+1e-308, "Very small values", 0}
};

static void run_test_case(const test_case *tc) {
    __m128d va = _mm_load_sd(&tc->a);
    __m128d vc = _mm_load_sd(&tc->c);
    double result;
    
    if (tc->use_memory) {
        __asm__ __volatile__(
            "vfmadd132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (tc->b), [c] "x" (vc)
        );
    } else {
        __m128d vb = _mm_load_sd(&tc->b);
        __asm__ __volatile__(
            "vfmadd132sd %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
    }
    
    _mm_store_sd(&result, va);
    
    // 特殊值处理
    const char* a_str = get_double_str(tc->a);
    const char* b_str = get_double_str(tc->b);
    const char* c_str = get_double_str(tc->c);
    const char* exp_str = get_double_str(tc->expected);
    const char* res_str = get_double_str(result);
    
    printf("%s\n", tc->desc);
    printf("  a=%s (0x%016lx), b=%s (0x%016lx), c=%s (0x%016lx)\n", 
           a_str, *(uint64_t*)&tc->a, b_str, *(uint64_t*)&tc->b, c_str, *(uint64_t*)&tc->c);
    printf("    Expected: %s (0x%016lx), Result: %s (0x%016lx) - %s\n", 
           exp_str, *(uint64_t*)&tc->expected, res_str, *(uint64_t*)&result,
           compare_doubles(result, tc->expected) ? "PASS" : "FAIL");
    printf("\n");
}
static void run_all_tests() {
    int total_pass = 1;
    
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        run_test_case(&cases[t]);
        // 检查测试结果
        double result = 0.0;
        __m128d va = _mm_load_sd(&cases[t].a);
        __m128d vc = _mm_load_sd(&cases[t].c);
        
        if (cases[t].use_memory) {
            __asm__ __volatile__(
                "vfmadd132sd %[b], %[c], %[a]"
                : [a] "+x" (va)
                : [b] "m" (cases[t].b), [c] "x" (vc)
            );
        } else {
            __m128d vb = _mm_load_sd(&cases[t].b);
            __asm__ __volatile__(
                "vfmadd132sd %[b], %[c], %[a]"
                : [a] "+x" (va)
                : [b] "x" (vb), [c] "x" (vc)
            );
        }
        
        _mm_store_sd(&result, va);
        if (!compare_doubles(result, cases[t].expected)) {
            total_pass = 0;
        }
    }
    
    printf("VFMADD132SD Tests: %s\n\n", total_pass ? "ALL PASS" : "SOME FAIL");
}

int main() {
    printf("==================================\n");
    printf("VFMADD132SD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    run_all_tests();
    return 0;
}
