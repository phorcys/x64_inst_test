#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cpuid.h>

static void test_vpgatherqd_256() {
    printf("=== Testing vpgatherqd (256-bit) ===\n");
    
    // Check CPU support for AVX2 (required for vpgatherqd)
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    if (!(ecx & (1 << 28)) || !(ebx & (1 << 5))) {
        printf("AVX2 not supported - skipping test\n");
        return;
    }
    
    int32_t base[8] ALIGNED(32) = {10, 20, 30, 40, 50, 60, 70, 80};
    int64_t idx[4] ALIGNED(32) = {0};
    int32_t dst[4] ALIGNED(32) = {0};
    int32_t expected[4] ALIGNED(32) = {0};
    __m128i vmask = _mm_set1_epi32(0xFFFFFFFF);

    // Test case 1: Normal gather
    idx[0] = 1; idx[1] = 3; idx[2] = 5; idx[3] = 7;
    __m256i v_idx = _mm256_load_si256((__m256i*)idx);
    __m128i v_dst;
    register __m128i tmp_dst asm("xmm1") = _mm_setzero_si128();
    asm volatile(
        "vpgatherqd %1, (%2,%3,1), %0"
        : "+x"(tmp_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    v_dst = tmp_dst;
    _mm_store_si128((__m128i*)dst, v_dst);
    expected[0] = base[1]; expected[1] = base[3];
    expected[2] = base[5]; expected[3] = base[7];
    print_int32_vec("Base", base, 8);
    print_int64_vec("Index", idx, 4);
    print_int32_vec("Result", dst, 4);
    print_int32_vec("Expected", expected, 4);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(int32_t)*4) == 0 ? "PASS" : "FAIL");

    // Test case 2: Duplicate indices
    idx[0] = 2; idx[1] = 2; idx[2] = 2; idx[3] = 2;
    v_idx = _mm256_load_si256((__m256i*)idx);
    tmp_dst = _mm_setzero_si128();
    asm volatile(
        "vpgatherqd %1, (%2,%3,1), %0"
        : "+x"(tmp_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    v_dst = tmp_dst;
    _mm_store_si128((__m128i*)dst, v_dst);
    expected[0] = base[2]; expected[1] = base[2];
    expected[2] = base[2]; expected[3] = base[2];
    print_int32_vec("Result", dst, 4);
    print_int32_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(int32_t)*4) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpgatherqd_256();
    return 0;
}
