#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define TOLERANCE 1e-6f

static void test_vaddps() {
    printf("--- Testing vaddps (packed single-precision addition) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test with normal values
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};
    float src2[8] ALIGNED(32) = {0.9f, 1.8f, 2.7f, 3.6f, 4.5f, 5.4f, 6.3f, 7.2f};
    float expected[8] ALIGNED(32) = {2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f};
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vaddps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", src1, 8);
    printf("Input2:   ");
    print_float_vec("Input2", src2, 8);
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (fabsf(result[i] - expected[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected[i], result[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 256-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: 256-bit register-register\n\n");
    }
    
    // Boundary values test (zeros, ones, NaN, infinity, negative)
    printf("Test 2: Boundary values\n");
    float boundary_src1[8] ALIGNED(32) = {
        0.0f, 1.0f, INFINITY, -1.0f, 
        NAN, -INFINITY, 100.0f, -100.0f
    };
    float boundary_src2[8] ALIGNED(32) = {
        0.0f, 0.0f, 1.0f, 1.0f, 
        1.0f, 1.0f, -50.0f, 50.0f
    };
    float boundary_expected[8] ALIGNED(32) = {
        0.0f, 1.0f, INFINITY, 0.0f,
        NAN, -INFINITY, 50.0f, -50.0f
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vaddps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", boundary_src1, 8);
    printf("Input2:   ");
    print_float_vec("Input2", boundary_src2, 8);
    printf("Expected: ");
    print_float_vec("Expected", boundary_expected, 8);
    printf("Result:   ");
    print_float_vec("Result", boundary_result, 8);
    
    // Check results with hex output for NaN
    printf("Expected (hex): ");
    print_hex_float_vec("Expected", boundary_expected, 8);
    printf("Result (hex):   ");
    print_hex_float_vec("Result", boundary_result, 8);
    
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i == 4) { // NaN position
            if (!isnan(boundary_result[i])) {
                printf("Mismatch at position %d: expected NaN, got %.6f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (i == 2 || i == 5) { // Infinity positions
            if (!isinf(boundary_result[i]) || 
                (i == 2 && boundary_result[i] < 0) ||
                (i == 5 && boundary_result[i] > 0)) {
                printf("Mismatch at position %d: expected %s, got %.6f\n", 
                       i, (i == 2) ? "INF" : "-INF", boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (fabsf(boundary_result[i] - boundary_expected[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, boundary_expected[i], boundary_result[i]);
            boundary_pass = 0;
        }
    }
    
    if (boundary_pass) {
        printf("[PASS] Test 2: Boundary values\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Boundary values\n");
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vaddps tests passed!\n");
    } else {
        printf("Some vaddps tests failed\n");
    }
}

int main() {
    test_vaddps();
    return 0;
}
