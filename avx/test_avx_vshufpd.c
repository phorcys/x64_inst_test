#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vshufpd_128() {
    printf("=== Testing vshufpd (128-bit) ===\n");
    
    double a[2] ALIGNED(16) = {1.0, 2.0};
    double b[2] ALIGNED(16) = {3.0, 4.0};
    double dst[2] ALIGNED(16) = {0};
    double expected[2] ALIGNED(16) = {0};
    
    // Test case 1: Select a[0] and b[0]
    __m128d v_a = _mm_load_pd(a);
    __m128d v_b = _mm_load_pd(b);
    __m128d v_dst;
    asm volatile(
        "vshufpd $0x0, %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm_store_pd(dst, v_dst);
    expected[0] = a[0]; expected[1] = b[0];
    print_double_vec("Result", dst, 2);
    print_double_vec("Expected", expected, 2);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(double)*2) == 0 ? "PASS" : "FAIL");

    // Test case 2: Select a[1] and b[1]
    asm volatile(
        "vshufpd $0x3, %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm_store_pd(dst, v_dst);
    expected[0] = a[1]; expected[1] = b[1];
    print_double_vec("Result", dst, 2);
    print_double_vec("Expected", expected, 2);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(double)*2) == 0 ? "PASS" : "FAIL");
}

static void test_vshufpd_256() {
    printf("=== Testing vshufpd (256-bit) ===\n");
    
    double a[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
    double b[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0};
    double dst[4] ALIGNED(32) = {0};
    double expected[4] ALIGNED(32) = {0};
    
    // Test case 1: Complex shuffle pattern
    __m256d v_a = _mm256_load_pd(a);
    __m256d v_b = _mm256_load_pd(b);
    __m256d v_dst;
    asm volatile(
        "vshufpd $0x5, %2, %1, %0"  // 0101 pattern (low:01, high:01)
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm256_store_pd(dst, v_dst);
    // 根据Intel手册修正预期值:
    // 低128位: 选择源1[1]和源2[0]
    // 高128位: 选择源1[3]和源2[2]
    expected[0] = a[1]; expected[1] = b[0];
    expected[2] = a[3]; expected[3] = b[2];
    print_double_vec("Result", dst, 4);
    print_double_vec("Expected", expected, 4);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(double)*4) == 0 ? "PASS" : "FAIL");

    // Test case 2: Special float values
    // 重新初始化数组以避免残留值影响
    double a2[4] ALIGNED(32) = {0.0/0.0, 1.0/0.0, 3.0, 4.0};
    double b2[4] ALIGNED(32) = {-1.0/0.0, 0.0, 7.0, 8.0};
    v_a = _mm256_load_pd(a2);
    v_b = _mm256_load_pd(b2);
    asm volatile(
        "vshufpd $0xA, %2, %1, %0"  // 1010 pattern (low:10, high:10)
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm256_store_pd(dst, v_dst);
    expected[0] = a2[0]; expected[1] = b2[1];
    expected[2] = a2[2]; expected[3] = b2[3];
    print_double_vec("Result", dst, 4);
    print_double_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(double)*4) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vshufpd_128();
    test_vshufpd_256();
    return 0;
}
