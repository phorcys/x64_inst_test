#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpgatherdq_256() {
    printf("=== Testing vpgatherdq (256-bit) ===\n");
    
    int64_t base[4] ALIGNED(32) = {10, 20, 30, 40};
    int32_t idx[4] ALIGNED(32) = {0};
    int64_t dst[4] ALIGNED(32) = {0};
    int64_t expected[4] ALIGNED(32) = {0};
    __m256i vmask = _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF);

    // Test case 1: Normal gather
    idx[0] = 1*8; idx[1] = 3*8; idx[2] = 0*8; idx[3] = 2*8;
    __m128i v_idx = _mm_load_si128((__m128i*)idx);
    __m256i v_dst = _mm256_setzero_si256();
    asm volatile(
        "vpgatherdq %1, (%2,%3,1), %0"
        : "+x"(v_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    _mm256_store_si256((__m256i*)dst, v_dst);
    expected[0] = base[1]; expected[1] = base[3];
    expected[2] = base[0]; expected[3] = base[2];
    print_int64_vec("Base", base, 4);
    print_int32_vec("Index", idx, 4);
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");

    // Test case 2: Duplicate indices
    idx[0] = 2*8; idx[1] = 2*8; idx[2] = 2*8; idx[3] = 2*8;
    v_idx = _mm_load_si128((__m128i*)idx);
    v_dst = _mm256_setzero_si256();
    asm volatile(
        "vpgatherdq %1, (%2,%3,1), %0"
        : "+x"(v_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    _mm256_store_si256((__m256i*)dst, v_dst);
    expected[0] = base[2]; expected[1] = base[2];
    expected[2] = base[2]; expected[3] = base[2];
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpgatherdq_256();
    return 0;
}
