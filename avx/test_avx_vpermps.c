#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static void test_vpermps_256() {
    printf("=== Testing vpermps (256-bit) ===\n");
    
    float src[8] ALIGNED(32) = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};
    float dst[8] ALIGNED(32) = {0};
    float expected[8] ALIGNED(32) = {0};
    int32_t idx[8] ALIGNED(32) = {0};

    // Test case 1: Normal permutation
    idx[0]=1; idx[1]=0; idx[2]=3; idx[3]=2;
    idx[4]=5; idx[5]=4; idx[6]=7; idx[7]=6;
    __m256 v1 = _mm256_load_ps(src);
    __m256i v_idx = _mm256_load_si256((__m256i*)idx);
    __m256 r1;
    asm volatile("vpermps %1, %2, %0" : "=x"(r1) : "x"(v1), "x"(v_idx));
    _mm256_store_ps(dst, r1);
    expected[0]=src[1]; expected[1]=src[0]; 
    expected[2]=src[3]; expected[3]=src[2];
    expected[4]=src[5]; expected[5]=src[4];
    expected[6]=src[7]; expected[7]=src[6];
    print_float_vec("Src", src, 8);
    print_float_vec("Result", dst, 8);
    print_float_vec("Expected", expected, 8);
    printf("Test 1: %s\n", 
           float_equal(dst[0], expected[0], 0.0001f) &&
           float_equal(dst[1], expected[1], 0.0001f) &&
           float_equal(dst[2], expected[2], 0.0001f) &&
           float_equal(dst[3], expected[3], 0.0001f) &&
           float_equal(dst[4], expected[4], 0.0001f) &&
           float_equal(dst[5], expected[5], 0.0001f) &&
           float_equal(dst[6], expected[6], 0.0001f) &&
           float_equal(dst[7], expected[7], 0.0001f) ? "PASS" : "FAIL");

    // Test case 2: Duplicate elements
    idx[0]=0; idx[1]=0; idx[2]=1; idx[3]=1;
    idx[4]=2; idx[5]=2; idx[6]=3; idx[7]=3;
    v_idx = _mm256_load_si256((__m256i*)idx);
    asm volatile("vpermps %1, %2, %0" : "=x"(r1) : "x"(v1), "x"(v_idx));
    _mm256_store_ps(dst, r1);
    expected[0]=src[0]; expected[1]=src[0];
    expected[2]=src[1]; expected[3]=src[1];
    expected[4]=src[2]; expected[5]=src[2];
    expected[6]=src[3]; expected[7]=src[3];
    print_float_vec("Result", dst, 8);
    print_float_vec("Expected", expected, 8);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(float)*8) == 0 ? "PASS" : "FAIL");

    // Test case 3: Reverse order
    idx[0]=7; idx[1]=6; idx[2]=5; idx[3]=4;
    idx[4]=3; idx[5]=2; idx[6]=1; idx[7]=0;
    v_idx = _mm256_load_si256((__m256i*)idx);
    asm volatile("vpermps %1, %2, %0" : "=x"(r1) : "x"(v1), "x"(v_idx));
    _mm256_store_ps(dst, r1);
    for(int i=0; i<8; i++) {
        expected[i] = src[7-i];
    }
    print_float_vec("Result", dst, 8);
    print_float_vec("Expected", expected, 8);
    printf("Test 3: %s\n", 
           float_equal(dst[0], expected[0], 0.0001f) &&
           float_equal(dst[1], expected[1], 0.0001f) &&
           float_equal(dst[2], expected[2], 0.0001f) &&
           float_equal(dst[3], expected[3], 0.0001f) &&
           float_equal(dst[4], expected[4], 0.0001f) &&
           float_equal(dst[5], expected[5], 0.0001f) &&
           float_equal(dst[6], expected[6], 0.0001f) &&
           float_equal(dst[7], expected[7], 0.0001f) ? "PASS" : "FAIL");
}

int main() {
    test_vpermps_256();
    return 0;
}
