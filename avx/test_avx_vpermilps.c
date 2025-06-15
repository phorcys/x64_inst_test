#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static void test_vpermilps_128() {
    printf("=== Testing vpermilps (128-bit) ===\n");
    
    float src[4] ALIGNED(16) = {1.1f, 2.2f, 3.3f, 4.4f};
    float dst[4] ALIGNED(16) = {0};
    float expected[4] ALIGNED(16) = {0};
    
    // Test case 1: Duplicate first element (0x00)
    __m128 v1 = _mm_load_ps(src);
    __m128 r1;
    asm volatile("vpermilps $0x00, %1, %0" : "=x"(r1) : "x"(v1));
    _mm_store_ps(dst, r1);
    expected[0] = src[0]; expected[1] = src[0]; expected[2] = src[0]; expected[3] = src[0];
    print_float_vec("Src", src, 4);
    print_float_vec("Result", dst, 4);
    print_float_vec("Expected", expected, 4);
    printf("Test 1: %s\n", 
           float_equal(dst[0], expected[0], 0.0001f) &&
           float_equal(dst[1], expected[1], 0.0001f) &&
           float_equal(dst[2], expected[2], 0.0001f) &&
           float_equal(dst[3], expected[3], 0.0001f) ? "PASS" : "FAIL");
    
    // Test case 2: Reverse order (0x1B)
    asm volatile("vpermilps $0x1B, %1, %0" : "=x"(r1) : "x"(v1));
    _mm_store_ps(dst, r1);
    expected[0] = src[3]; expected[1] = src[2]; expected[2] = src[1]; expected[3] = src[0];
    print_float_vec("Result", dst, 4);
    print_float_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(float)*4) == 0 ? "PASS" : "FAIL");
    
    // Test case 3: With normal values (0x39 pattern: [1,2,3,0])
    src[0] = 1.0f; src[1] = 2.0f; src[2] = 3.0f; src[3] = 4.0f;
    v1 = _mm_load_ps(src);
    asm volatile("vpermilps $0x39, %1, %0" : "=x"(r1) : "x"(v1));
    _mm_store_ps(dst, r1);
    printf("Debug - Actual permutation values:\n");
    printf("dst[0]=%.1f dst[1]=%.1f dst[2]=%.1f dst[3]=%.1f\n", 
           dst[0], dst[1], dst[2], dst[3]);
    printf("Expected indices: [1,2,3,0] => [%.1f,%.1f,%.1f,%.1f]\n",
           src[1], src[2], src[3], src[0]);
    expected[0] = src[1]; expected[1] = src[2]; expected[2] = src[3]; expected[3] = src[0];
    print_float_vec("Result", dst, 4);
    print_float_vec("Expected", expected, 4);
    printf("Test 3: %s\n", 
           float_equal(dst[0], expected[0], 0.0001f) &&
           float_equal(dst[1], expected[1], 0.0001f) &&
           float_equal(dst[2], expected[2], 0.0001f) &&
           float_equal(dst[3], expected[3], 0.0001f) ? "PASS" : "FAIL");
}

static void test_vpermilps_256() {
    printf("\n=== Testing vpermilps (256-bit) ===\n");
    
    float src[8] ALIGNED(32) = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};
    float dst[8] ALIGNED(32) = {0};
    float expected[8] ALIGNED(32) = {0};
    
    // Test case 1: Duplicate first element in each lane (0x00)
    __m256 v1 = _mm256_load_ps(src);
    __m256 r1;
    asm volatile("vpermilps $0x00, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_ps(dst, r1);
    // First lane duplicates src[0]
    expected[0] = src[0]; expected[1] = src[0]; expected[2] = src[0]; expected[3] = src[0];
    // Second lane duplicates src[4]
    expected[4] = src[4]; expected[5] = src[4]; expected[6] = src[4]; expected[7] = src[4];
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
    
    // Test case 2: Complex permutation pattern (0x1B)
    asm volatile("vpermilps $0x1B, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_ps(dst, r1);
    for(int i=0; i<4; i++) {
        expected[i] = src[3-i];   // Reverse first lane
        expected[i+4] = src[7-i]; // Reverse second lane
    }
    print_float_vec("Result", dst, 8);
    print_float_vec("Expected", expected, 8);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(float)*8) == 0 ? "PASS" : "FAIL");
    
    // Test case 3: With normal values (0x39 pattern per lane: [1,2,3,0])
    src[0] = 1.0f; src[1] = 2.0f; src[2] = 3.0f; src[3] = 4.0f;
    src[4] = 5.0f; src[5] = 6.0f; src[6] = 7.0f; src[7] = 8.0f;
    v1 = _mm256_load_ps(src);
    asm volatile("vpermilps $0x39, %1, %0" : "=x"(r1) : "x"(v1));
    _mm256_store_ps(dst, r1);
    // First lane [1,2,3,0]
    expected[0] = src[1]; expected[1] = src[2]; expected[2] = src[3]; expected[3] = src[0];
    // Second lane [5,6,7,4]
    expected[4] = src[5]; expected[5] = src[6]; expected[6] = src[7]; expected[7] = src[4];
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
    test_vpermilps_128();
    test_vpermilps_256();
    return 0;
}
