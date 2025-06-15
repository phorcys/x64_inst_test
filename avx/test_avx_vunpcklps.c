#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define EPSILON 1e-6f

static void print_floats(const char* label, float* arr, int count) {
    printf("%s: [", label);
    for (int i = 0; i < count; i++) {
        if (isnan(arr[i])) printf("NaN");
        else if (isinf(arr[i])) printf("%sInf", arr[i] < 0 ? "-" : "+");
        else printf("%.1f", arr[i]);
        if (i < count-1) printf(", ");
    }
    printf("]\n");
}

static void test_vunpcklps_128() {
    printf("=== Testing vunpcklps (128-bit) ===\n");
    
    float a[4] ALIGNED(16) = {0};
    float b[4] ALIGNED(16) = {0};
    float dst[4] ALIGNED(16) = {0};
    float expected[4] ALIGNED(16) = {0};
    
    // 测试1: 基本解包
    a[0] = 1.0f; a[1] = 2.0f; a[2] = 3.0f; a[3] = 4.0f;
    b[0] = 5.0f; b[1] = 6.0f; b[2] = 7.0f; b[3] = 8.0f;
    // 低位解包：交替取低位元素
    // [a0, b0, a1, b1]
    expected[0] = a[0];
    expected[1] = b[0];
    expected[2] = a[1];
    expected[3] = b[1];
    
    __m128 v_a = _mm_load_ps(a);
    __m128 v_b = _mm_load_ps(b);
    __m128 v_dst;
    
    asm volatile(
        "vunpcklps %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm_store_ps(dst, v_dst);
    
    print_floats("A", a, 4);
    print_floats("B", b, 4);
    print_floats("Result", dst, 4);
    print_floats("Expected", expected, 4);
    
    // 处理特殊值比较
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i])) {
            pass = pass && isnan(dst[i]);
        } else if (isinf(expected[i])) {
            pass = pass && isinf(dst[i]) && (signbit(dst[i]) == signbit(expected[i]));
        } else {
            pass = pass && float_equal(dst[i], expected[i], EPSILON);
        }
    }
    
    printf("Test: %s\n\n", pass ? "PASS" : "FAIL");
}

static void test_vunpcklps_256() {
    printf("\n=== Testing vunpcklps (256-bit) ===\n");
    
    float a[8] ALIGNED(32) = {0};
    float b[8] ALIGNED(32) = {0};
    float dst[8] ALIGNED(32) = {0};
    float expected[8] ALIGNED(32) = {0};
    
    // 测试1: 基本解包
    for (int i = 0; i < 8; i++) {
        a[i] = i + 1.0f;
        b[i] = i + 9.0f;
    }
    // 低位解包：每个128位通道交替取低位元素
    // 通道0: [a0, b0, a1, b1]
    // 通道1: [a4, b4, a5, b5]
    expected[0] = a[0];
    expected[1] = b[0];
    expected[2] = a[1];
    expected[3] = b[1];
    expected[4] = a[4];
    expected[5] = b[4];
    expected[6] = a[5];
    expected[7] = b[5];
    
    __m256 v_a = _mm256_load_ps(a);
    __m256 v_b = _mm256_load_ps(b);
    __m256 v_dst;
    
    asm volatile(
        "vunpcklps %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm256_store_ps(dst, v_dst);
    
    print_floats("A", a, 8);
    print_floats("B", b, 8);
    print_floats("Result", dst, 8);
    print_floats("Expected", expected, 8);
    
    // 处理特殊值比较
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (isnan(expected[i])) {
            pass = pass && isnan(dst[i]);
        } else if (isinf(expected[i])) {
            pass = pass && isinf(dst[i]) && (signbit(dst[i]) == signbit(expected[i]));
        } else {
            pass = pass && float_equal(dst[i], expected[i], EPSILON);
        }
    }
    
    printf("Test: %s\n\n", pass ? "PASS" : "FAIL");
}

int main() {
    test_vunpcklps_128();
    test_vunpcklps_256();
    return 0;
}
