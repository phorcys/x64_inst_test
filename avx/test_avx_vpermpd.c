#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static void test_vpermpd_256() {
    printf("=== Testing vpermpd (256-bit) ===\n");
    
    double src[4] ALIGNED(32) = {1.1, 2.2, 3.3, 4.4};
    double dst[4] ALIGNED(32) = {0};
    double expected[4] ALIGNED(32) = {0};
    
    // Test case 1: Duplicate first element (0x00)
    __m256d v1 = _mm256_load_pd(src);
    __m256d r1;
    asm volatile("vpermpd $0x00, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_pd(dst, r1);
    expected[0] = src[0]; expected[1] = src[0]; 
    expected[2] = src[0]; expected[3] = src[0];
    print_double_vec("Src", src, 4);
    print_double_vec("Result", dst, 4);
    print_double_vec("Expected", expected, 4);
    printf("Test 1: %s\n", 
           double_equal(dst[0], expected[0], 0.0001) &&
           double_equal(dst[1], expected[1], 0.0001) &&
           double_equal(dst[2], expected[2], 0.0001) &&
           double_equal(dst[3], expected[3], 0.0001) ? "PASS" : "FAIL");
    
    // Test case 2: Rotate elements (0x39)
    asm volatile("vpermpd $0x39, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_pd(dst, r1);
    expected[0] = src[1]; expected[1] = src[2];
    expected[2] = src[3]; expected[3] = src[0];
    print_double_vec("Result", dst, 4);
    print_double_vec("Expected", expected, 4);
    printf("Test 2: %s\n", 
           double_equal(dst[0], expected[0], 0.0001) &&
           double_equal(dst[1], expected[1], 0.0001) &&
           double_equal(dst[2], expected[2], 0.0001) &&
           double_equal(dst[3], expected[3], 0.0001) ? "PASS" : "FAIL");
    
    // Test case 3: Reverse order (0x1B)
    asm volatile("vpermpd $0x1B, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_pd(dst, r1);
    expected[0] = src[3]; expected[1] = src[2];
    expected[2] = src[1]; expected[3] = src[0];
    print_double_vec("Result", dst, 4);
    print_double_vec("Expected", expected, 4);
    printf("Test 3: %s\n", 
           double_equal(dst[0], expected[0], 0.0001) &&
           double_equal(dst[1], expected[1], 0.0001) &&
           double_equal(dst[2], expected[2], 0.0001) &&
           double_equal(dst[3], expected[3], 0.0001) ? "PASS" : "FAIL");
}

int main() {
    test_vpermpd_256();
    return 0;
}
