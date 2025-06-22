#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"

// 辅助函数：将浮点数转换为可读字符串
static const char* get_float_str(float f) {
    if (isnan(f)) return "nan";
    if (isinf(f)) return f < 0 ? "-inf" : "inf";
    if (f == 0.0f) return "0.0";
    return "value";
}

// 辅助函数：将float转换为uint32表示
static uint32_t float_to_uint(float f) {
    union { float f; uint32_t u; } u;
    u.f = f;
    return u.u;
}

// 比较浮点数，处理特殊值
static int compare_floats(float a, float b) {
    if (isnan(a) && isnan(b)) return 1;
    if (isinf(a) && isinf(b) && signbit(a) == signbit(b)) return 1;
    return fabsf(a - b) < 0.0001f * fabsf(a);
}


// 测试单个用例的函数
static void test_case(const char *description, float a, float b, float c, int use_memory) {
    float expected = a * b + c;
    
    __m128 va = _mm_load_ss(&a);
    __m128 vc = _mm_load_ss(&c);
    float result;
    
    if (use_memory) {
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "m" (b), [c] "x" (vc)
        );
    } else {
        __m128 vb = _mm_load_ss(&b);
        __asm__ __volatile__(
            "vfmadd132ss %[b], %[c], %[a]"
            : [a] "+x" (va)
            : [b] "x" (vb), [c] "x" (vc)
        );
    }
    
    _mm_store_ss(&result, va);
    
    // 特殊值处理
    const char* a_str = get_float_str(a);
    const char* b_str = get_float_str(b);
    const char* c_str = get_float_str(c);
    const char* exp_str = get_float_str(expected);
    const char* res_str = get_float_str(result);
    
    printf("%s\n", description);
    printf("  a=%s (0x%08x), b=%s (0x%08x), c=%s (0x%08x)\n", 
           a_str, float_to_uint(a), b_str, float_to_uint(b), c_str, float_to_uint(c));
    printf("    Expected: %s (0x%08x), Result: %s (0x%08x) - %s\n", 
           exp_str, float_to_uint(expected), res_str, float_to_uint(result),
           compare_floats(expected, result) ? "PASS" : "FAIL");
    printf("\n");
}

int main() {
    printf("=============================\n");
    printf("VFMADD132SS Comprehensive Tests\n");
    printf("=============================\n\n");

    // 正常值
    test_case("Normal values (Register-Register):", 1.0f, 0.5f, 1.0f, 0);
    test_case("Normal values (Register-Memory):", 1.0f, 0.5f, 1.0f, 1);

    // 零值
    test_case("Zero values (a=0):", 0.0f, 1.0f, 0.1f, 0);
    test_case("Zero values (a=-0):", -0.0f, 2.0f, 0.2f, 1);

    // 无穷大
    const float inf = 1.0f / 0.0f; // 正无穷
    test_case("Infinity values (a=inf):", inf, 1.0f, 1.0f, 0);
    test_case("Infinity values (a=-inf):", -inf, 1.0f, 1.0f, 1);

    // NaN
    const float nan = 0.0f / 0.0f; // NaN
    test_case("NaN values (a=nan):", nan, 1.0f, 1.0f, 0);
    test_case("NaN values (b=nan):", 1.0f, nan, 2.0f, 1);

    // 边界值
    const float min_normal = 1.17549435e-38f; // 最小正规数
    test_case("Boundary values (min normal):", min_normal, 2.0f, min_normal, 0);
    
    const float max_float = 3.40282347e+38f; // 最大浮点数
    test_case("Boundary values (max float):", max_float, 2.0f, max_float, 1);

    // 混合特殊值
    test_case("Mixed special values (a=inf, b=nan):", inf, nan, 0.0f, 0);
    test_case("Mixed special values (a=nan, b=inf):", nan, inf, 1.0f, 1);

    // 极小值
    const float tiny = 1.0e-30f;
    test_case("Very small values:", tiny, 2.0f, tiny, 0);

    return 0;
}
