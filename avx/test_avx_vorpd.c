#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

static void print_hex_double_vec(const char* name, double* vec, int count) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        uint64_t val;
        memcpy(&val, &vec[i], sizeof(uint64_t));
        printf("0x%016lX ", val);
    }
    printf("\n");
}

static void test_vorpd() {
    printf("--- Testing vorpd (bitwise OR of packed double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    // 使用memcpy设置位模式，避免浮点转换问题
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
        0xFEFCFEF8FEFCFEF0, 0xFFFFFFFFFFFFFFFF
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
        "vorpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_hex_double_vec("Expected", expected, 4);
    printf("Result:   ");
    print_hex_double_vec("Result", result, 4);
    
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
        0.0, -0.0, INFINITY, 0.0
    };
    
    uint64_t boundary_src2_bits[4] = {0, 0, 0, 0xFFFFFFFFFFFFFFFF};
    uint64_t boundary_expected_bits[4] = {0, 0x8000000000000000, 0x7FF0000000000000, 0xFFFFFFFFFFFFFFFF};
    
    memcpy(&boundary_src2[3], &boundary_src2_bits[3], sizeof(double));
    memcpy(&boundary_expected[3], &boundary_expected_bits[3], sizeof(double));
    double boundary_result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vorpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_hex_double_vec("Expected", boundary_expected, 4);
    printf("Result:   ");
    print_hex_double_vec("Result", boundary_result, 4);
    
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
        printf("All vorpd tests passed!\n");
    } else {
        printf("Some vorpd tests failed\n");
    }
}

int main() {
    test_vorpd();
    return 0;
}
