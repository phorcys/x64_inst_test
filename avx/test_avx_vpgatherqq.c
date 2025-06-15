#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cpuid.h>

static void test_vpgatherqq_256() {
    printf("=== Testing vpgatherqq (256-bit) ===\n");
    
    // Check CPU support for AVX2 (required for vpgatherqq)
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    if (!(ecx & (1 << 28)) || !(ebx & (1 << 5))) {
        printf("AVX2 not supported - skipping test\n");
        return;
    }
    
    int64_t base[4] ALIGNED(32) = {10, 20, 30, 40};
    int64_t idx[4] ALIGNED(32) = {0};
    int64_t dst[4] ALIGNED(32) = {0};
    int64_t expected[4] ALIGNED(32) = {0};
    __m256i vmask = _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF);

    // Test case 1: Normal gather
    idx[0] = 1; idx[1] = 3; idx[2] = 0; idx[3] = 2;
    __m256i v_idx = _mm256_load_si256((__m256i*)idx);
    __m256i v_dst;
    register __m256i tmp_dst asm("ymm1") = _mm256_setzero_si256();
    asm volatile(
        "vpgatherqq %1, (%2,%3,1), %0"
        : "+x"(tmp_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    v_dst = tmp_dst;
    _mm256_store_si256((__m256i*)dst, v_dst);
    expected[0] = base[1]; expected[1] = base[3];
    expected[2] = base[0]; expected[3] = base[2];
    print_int64_vec("Base", base, 4);
    print_int64_vec("Index", idx, 4);
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");

    // Test case 2: Duplicate indices
    idx[0] = 2; idx[1] = 2; idx[2] = 2; idx[3] = 2;
    v_idx = _mm256_load_si256((__m256i*)idx);
    tmp_dst = _mm256_setzero_si256();
    asm volatile(
        "vpgatherqq %1, (%2,%3,1), %0"
        : "+x"(tmp_dst)
        : "x"(vmask), "r"(base), "x"(v_idx)
        : "memory"
    );
    v_dst = tmp_dst;
    _mm256_store_si256((__m256i*)dst, v_dst);
    expected[0] = base[2]; expected[1] = base[2];
    expected[2] = base[2]; expected[3] = base[2];
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpgatherqq_256();
    return 0;
}
