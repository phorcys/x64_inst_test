#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define EPSILON 1e-9

static void print_doubles(const char* label, double* arr, int count) {
    printf("%s: [", label);
    for (int i = 0; i < count; i++) {
        if (isnan(arr[i])) printf("NaN");
        else if (isinf(arr[i])) printf("%sInf", arr[i] < 0 ? "-" : "+");
        else printf("%.1f", arr[i]);
        if (i < count-1) printf(", ");
    }
    printf("]\n");
}

static void test_vunpckhpd_128() {
    printf("=== Testing vunpckhpd (128-bit) ===\n");
    
    double a[2] ALIGNED(16) = {0};
    double b[2] ALIGNED(16) = {0};
    double dst[2] ALIGNED(16) = {0};
    double expected[2] ALIGNED(16) = {0};
    
    // 测试1: 基本解包
    a[0] = 1.0; a[1] = 2.0;
    b[0] = 3.0; b[1] = 4.0;
    expected[0] = a[1]; // 高64位
    expected[1] = b[1]; // 高64位
    
    __m128d v_a = _mm_load_pd(a);
    __m128d v_b = _mm_load_pd(b);
    __m128d v_dst;
    
    asm volatile(
        "vunpckhpd %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm_store_pd(dst, v_dst);
    
    print_doubles("A", a, 2);
    print_doubles("B", b, 2);
    print_doubles("Result", dst, 2);
    print_doubles("Expected", expected, 2);
    
    // 处理特殊值比较
    int pass = 1;
    if (isnan(expected[0])) {
        pass = pass && isnan(dst[0]);
    } else if (isinf(expected[0])) {
        pass = pass && isinf(dst[0]) && (signbit(dst[0]) == signbit(expected[0]));
    } else {
        pass = pass && double_equal(dst[0], expected[0], EPSILON);
    }
    
    if (isnan(expected[1])) {
        pass = pass && isnan(dst[1]);
    } else if (isinf(expected[1])) {
        pass = pass && isinf(dst[1]) && (signbit(dst[1]) == signbit(expected[1]));
    } else {
        pass = pass && double_equal(dst[1], expected[1], EPSILON);
    }
    
    printf("Test: %s\n\n", pass ? "PASS" : "FAIL");
    
    // 测试2: 边界值（NaN, Inf）
    a[0] = NAN; a[1] = INFINITY;
    b[0] = -INFINITY; b[1] = -0.0;
    expected[0] = a[1]; // 高64位
    expected[1] = b[1]; // 高64位
    
    v_a = _mm_load_pd(a);
    v_b = _mm_load_pd(b);
    
    asm volatile(
        "vunpckhpd %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm_store_pd(dst, v_dst);
    
    print_doubles("A", a, 2);
    print_doubles("B", b, 2);
    print_doubles("Result", dst, 2);
    print_doubles("Expected", expected, 2);
    
    // 处理特殊值比较
    pass = 1;
    if (isnan(expected[0])) {
        pass = pass && isnan(dst[0]);
    } else if (isinf(expected[0])) {
        pass = pass && isinf(dst[0]) && (signbit(dst[0]) == signbit(expected[0]));
    } else {
        pass = pass && double_equal(dst[0], expected[0], EPSILON);
    }
    
    if (isnan(expected[1])) {
        pass = pass && isnan(dst[1]);
    } else if (isinf(expected[1])) {
        pass = pass && isinf(dst[1]) && (signbit(dst[1]) == signbit(expected[1]));
    } else {
        pass = pass && double_equal(dst[1], expected[1], EPSILON);
    }
    
    printf("Test: %s\n\n", pass ? "PASS" : "FAIL");
}

static void test_vunpckhpd_256() {
    printf("\n=== Testing vunpckhpd (256-bit) ===\n");
    
    double a[4] ALIGNED(32) = {0};
    double b[4] ALIGNED(32) = {0};
    double dst[4] ALIGNED(32) = {0};
    double expected[4] ALIGNED(32) = {0};
    
    // 测试1: 基本解包
    a[0] = 1.0; a[1] = 2.0; a[2] = 5.0; a[3] = 6.0;
    b[0] = 3.0; b[1] = 4.0; b[2] = 7.0; b[3] = 8.0;
    // 通道0: [a1, b1]
    // 通道1: [a3, b3]
    expected[0] = a[1]; // 通道0高64位
    expected[1] = b[1]; // 通道0高64位
    expected[2] = a[3]; // 通道1高64位
    expected[3] = b[3]; // 通道1高64位
    
    __m256d v_a = _mm256_load_pd(a);
    __m256d v_b = _mm256_load_pd(b);
    __m256d v_dst;
    
    asm volatile(
        "vunpckhpd %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
        :
    );
    
    _mm256_store_pd(dst, v_dst);
    
    print_doubles("A", a, 4);
    print_doubles("B", b, 4);
    print_doubles("Result", dst, 4);
    print_doubles("Expected", expected, 4);
    printf("Test: %s\n\n", 
           double_equal(dst[0], expected[0], EPSILON) && 
           double_equal(dst[1], expected[1], EPSILON) && 
           double_equal(dst[2], expected[2], EPSILON) && 
           double_equal(dst[3], expected[3], EPSILON) ? "PASS" : "FAIL");
}

int main() {
    test_vunpckhpd_128();
    test_vunpckhpd_256();
    return 0;
}
