#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

static void test_vxorpd() {
    printf("--- Testing vxorpd (bitwise XOR of packed double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    double src1[4] ALIGNED(32) = {0};
    double src2[4] ALIGNED(32) = {0};
    double expected[4] ALIGNED(32) = {0};
    
    uint64_t src1_bits[4] = {
        0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
        0x123456789ABCDEF0, 0xF0F0F0F0F0F0F0F0
    };
    uint64_t src2_bits[4] = {
        0x5555555555555555, 0xAAAAAAAAAAAAAAAA,
        0xFEDCBA9876543210, 0x0F0F0F0F0F0F0F0F
    };
    uint64_t expected_bits[4] = {
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
        0xECE8ECE0ECE8ECE0, 0xFFFFFFFFFFFFFFFF
    };
    
    for (int i = 0; i < 4; i++) {
        memcpy(&src1[i], &src1_bits[i], sizeof(double));
        memcpy(&src2[i], &src2_bits[i], sizeof(double));
        memcpy(&expected[i], &expected_bits[i], sizeof(double));
    }
    double result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vxorpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_double_vec_hex("Expected", expected, 4);
    printf("Result:   ");
    print_double_vec_hex("Result", result, 4);
    
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (memcmp(&result[i], &expected[i], sizeof(double))) {
            printf("Mismatch at position %d: expected 0x%016lX, got 0x%016lX\n", 
                   i, *(uint64_t*)&expected[i], *(uint64_t*)&result[i]);
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
    double boundary_src1[4] ALIGNED(32) = {
        0.0, -0.0, INFINITY, NAN
    };
    double boundary_src2[4] ALIGNED(32) = {
        0.0, 0.0, 0.0, 0.0
    };
    double boundary_expected[4] ALIGNED(32) = {
        0.0, -0.0, INFINITY, NAN
    };
    
    // Set NaN at position 3
    uint64_t nan_bits = 0x7FF8000000000000; // Quiet NaN
    uint64_t all_ones_bits = 0xFFFFFFFFFFFFFFFF;
    
    memcpy(&boundary_src1[3], &nan_bits, sizeof(double));
    
    // Set last element to all ones pattern
    memcpy(&boundary_src2[3], &all_ones_bits, sizeof(double));
    
    // Expected results: 
    // - Positions 0-2: same as boundary_src1
    // - Position 3: nan_bits XOR all_ones_bits = 0x7FF8000000000000 ^ 0xFFFFFFFFFFFFFFFF = 0x8007FFFFFFFFFFFF
    uint64_t expected_bits_3 = 0x8007FFFFFFFFFFFF;
    memcpy(&boundary_expected[3], &expected_bits_3, sizeof(double));
    
    double boundary_result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vxorpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_double_vec_hex("Expected", boundary_expected, 4);
    printf("Result:   ");
    print_double_vec_hex("Result", boundary_result, 4);
    
    // Check results
    int boundary_pass = 1;
    for (int i = 0; i < 4; i++) {
        if (memcmp(&boundary_result[i], &boundary_expected[i], sizeof(double))) {
            printf("Mismatch at position %d: expected 0x%016lX, got 0x%016lX\n", 
                   i, *(uint64_t*)&boundary_expected[i], *(uint64_t*)&boundary_result[i]);
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
        printf("All vxorpd tests passed!\n");
    } else {
        printf("Some vxorpd tests failed\n");
    }
}

int main() {
    test_vxorpd();
    return 0;
}
