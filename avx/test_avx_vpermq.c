#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpermq_256() {
    printf("=== Testing vpermq (256-bit) ===\n");
    
    int64_t src[4] ALIGNED(32) = {0x1111111111111111, 0x2222222222222222, 
                                  0x3333333333333333, 0x4444444444444444};
    int64_t dst[4] ALIGNED(32) = {0};
    int64_t expected[4] ALIGNED(32) = {0};

    // Test case 1: Normal permutation (0x1B: reverse order)
    __m256i v1 = _mm256_load_si256((__m256i*)src);
    __m256i r1;
    asm volatile("vpermq $0x1B, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_si256((__m256i*)dst, r1);
    expected[0] = src[3]; expected[1] = src[2];
    expected[2] = src[1]; expected[3] = src[0];
    print_int64_vec("Src", src, 4);
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");

    // Test case 2: Duplicate elements (0x00: duplicate first element)
    asm volatile("vpermq $0x00, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_si256((__m256i*)dst, r1);
    expected[0] = src[0]; expected[1] = src[0];
    expected[2] = src[0]; expected[3] = src[0];
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");

    // Test case 3: Rotate elements (0x39: rotate right by 1)
    asm volatile("vpermq $0x39, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_si256((__m256i*)dst, r1);
    expected[0] = src[1]; expected[1] = src[2];
    expected[2] = src[3]; expected[3] = src[0];
    print_int64_vec("Result", dst, 4);
    print_int64_vec("Expected", expected, 4);
    printf("Test 3: %s\n", memcmp(dst, expected, sizeof(int64_t)*4) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpermq_256();
    return 0;
}
