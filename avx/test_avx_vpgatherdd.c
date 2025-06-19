#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpgatherdd_256() {
    printf("=== Testing vpgatherdd (256-bit) ===\n");
    
    int32_t base[8] ALIGNED(32) = {10, 20, 30, 40, 50, 60, 70, 80};
    int32_t idx[8] ALIGNED(32) = {0};
    int32_t dst[8] ALIGNED(32) = {0};
    int32_t expected[8] ALIGNED(32) = {0};
    __m256i vmask = _mm256_set1_epi32(0xFFFFFFFF);

    // Test case 1: Normal gather
    idx[0] = 1*4; idx[1] = 3*4; idx[2] = 5*4; idx[3] = 7*4;
    idx[4] = 0*4; idx[5] = 2*4; idx[6] = 4*4; idx[7] = 6*4;
    __m256i v_idx = _mm256_load_si256((__m256i*)idx);
    __m256i v_dst;
    v_dst = _mm256_setzero_si256();
    asm volatile(
        "vpgatherdd %1, (%2,%3,1), %0"
        : "+x"(v_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    _mm256_store_si256((__m256i*)dst, v_dst);
    expected[0] = base[1]; expected[1] = base[3];
    expected[2] = base[5]; expected[3] = base[7];
    expected[4] = base[0]; expected[5] = base[2];
    expected[6] = base[4]; expected[7] = base[6];
    print_int32_vec("Base", base, 8);
    print_int32_vec("Index", idx, 8);
    print_int32_vec("Result", dst, 8);
    print_int32_vec("Expected", expected, 8);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(int32_t)*8) == 0 ? "PASS" : "FAIL");

    // Test case 2: Duplicate indices
    idx[0] = 2*4; idx[1] = 2*4; idx[2] = 2*4; idx[3] = 2*4;
    idx[4] = 5*4; idx[5] = 5*4; idx[6] = 5*4; idx[7] = 5*4;
    v_idx = _mm256_load_si256((__m256i*)idx);
    v_dst = _mm256_setzero_si256();
    asm volatile(
        "vpgatherdd %1, (%2,%3,1), %0"
        : "+x"(v_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    _mm256_store_si256((__m256i*)dst, v_dst);
    expected[0] = base[2]; expected[1] = base[2];
    expected[2] = base[2]; expected[3] = base[2];
    expected[4] = base[5]; expected[5] = base[5];
    expected[6] = base[5]; expected[7] = base[5];
    print_int32_vec("Result", dst, 8);
    print_int32_vec("Expected", expected, 8);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(int32_t)*8) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpgatherdd_256();
    return 0;
}
