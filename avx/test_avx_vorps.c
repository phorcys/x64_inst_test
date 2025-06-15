#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vorps() {
    printf("--- Testing vorps (bitwise OR of packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {0};
    float src2[8] ALIGNED(32) = {0};
    float expected[8] ALIGNED(32) = {0};
    
    uint32_t src1_bits[8] = {
        0xAAAAAAAA, 0x55555555, 
        0x12345678, 0x9ABCDEF0,
        0x11111111, 0x22222222,
        0x33333333, 0x44444444
    };
    uint32_t src2_bits[8] = {
        0x55555555, 0xAAAAAAAA, 
        0xFEDCBA98, 0x76543210,
        0x55555555, 0xAAAAAAAA,
        0xFFFFFFFF, 0x00000000
    };
    uint32_t expected_bits[8] = {
        0xFFFFFFFF, 0xFFFFFFFF, 
        0xFEFCFEF8, 0xFEFCFEF0,
        0x55555555, 0xAAAAAAAA,
        0xFFFFFFFF, 0x44444444
    };
    
    for (int i = 0; i < 8; i++) {
        memcpy(&src1[i], &src1_bits[i], sizeof(float));
        memcpy(&src2[i], &src2_bits[i], sizeof(float));
        memcpy(&expected[i], &expected_bits[i], sizeof(float));
    }
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vorps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_hex_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_hex_float_vec("Result", result, 8);
    
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (memcmp(&result[i], &expected[i], sizeof(float))) {
            printf("Mismatch at position %d: expected 0x%08X, got 0x%08X\n", 
                   i, *(uint32_t*)&expected[i], *(uint32_t*)&result[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 256-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: 256-bit register-register\n\n");
    }
    
    // Boundary values test (zeros, ones, NaN)
    printf("Test 2: Boundary values\n");
    // Define bit patterns for all boundary values
    uint32_t boundary_src1_bits[8] = {
        0x00000000, // 0.0f
        0x80000000, // -0.0f
        0x7F800000, // INFINITY
        0x7FC00000, // NAN (quiet NaN)
        0x3F800000, // 1.0f
        0x40000000, // 2.0f
        0x40400000, // 3.0f
        0x40800000  // 4.0f
    };
    uint32_t boundary_src2_bits[8] = {
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000,
        0xFFFFFFFF, 
        0x00000000, 
        0x55555555, 
        0xAAAAAAAA
    };
    uint32_t boundary_expected_bits[8] = {
        0x00000000, // 0.0f | 0.0f = 0.0f
        0x80000000, // -0.0f | 0.0f = -0.0f
        0x7F800000, // INFINITY | 0.0f = INFINITY
        0x7FC00000, // NAN | 0.0f = NAN
        0xFFFFFFFF, // 1.0f | 0xFFFFFFFF = 0xFFFFFFFF
        0x40000000, // 2.0f | 0.0f = 2.0f
        0x55555555, // 3.0f | 0x55555555 = 0x55555555
        0xEAAAAAAA  // 4.0f | 0xAAAAAAAA = 0xEAAAAAAA
    };
    
    // Copy bit patterns to float arrays
    float boundary_src1[8] ALIGNED(32) = {0};
    float boundary_src2[8] ALIGNED(32) = {0};
    float boundary_expected[8] ALIGNED(32) = {0};
    for (int i = 0; i < 8; i++) {
        memcpy(&boundary_src1[i], &boundary_src1_bits[i], sizeof(float));
        memcpy(&boundary_src2[i], &boundary_src2_bits[i], sizeof(float));
        memcpy(&boundary_expected[i], &boundary_expected_bits[i], sizeof(float));
    }
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vorps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_hex_float_vec("Expected", boundary_expected, 8);
    printf("Result:   ");
    print_hex_float_vec("Result", boundary_result, 8);
    
    // Check results
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (memcmp(&boundary_result[i], &boundary_expected[i], sizeof(float))) {
            printf("Mismatch at position %d: expected 0x%08X, got 0x%08X\n", 
                   i, *(uint32_t*)&boundary_expected[i], *(uint32_t*)&boundary_result[i]);
            boundary_pass = 0;
        }
    }
    
    if (boundary_pass) {
        printf("[PASS] Boundary values output check\n");
        passed_tests++;
    } else {
        printf("[FAIL] Boundary values output check\n");
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vorps tests passed!\n");
    } else {
        printf("Some vorps tests failed\n");
    }
}

int main() {
    test_vorps();
    return 0;
}
