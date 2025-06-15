#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define TOLERANCE 1e-5f

static int compare_float(float a, float b) {
    if (isnan(a) && isnan(b)) {
        return 1;
    }
    if (isinf(a) && isinf(b) && (signbit(a) == signbit(b))) {
        return 1;
    }
    return fabs(a - b) < TOLERANCE;
}

static void test_vsqrtps() {
    printf("--- Testing vsqrtps (square root of packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test with normal values
    printf("Test 1: Normal values (256-bit)\n");
    float src1[8] ALIGNED(32) = {4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f};
    float expected[8] ALIGNED(32) = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vsqrtps %%ymm0, %%ymm1\n\t"
        "vmovaps %%ymm1, %0\n\t"
        : "=m"(*result)
        : "m"(*src1)
        : "ymm0", "ymm1"
    );
    
    printf("Input:    ");
    print_float_vec("Input", src1, 8);
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (!compare_float(result[i], expected[i])) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected[i], result[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: Normal values (256-bit)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: Normal values (256-bit)\n\n");
    }
    
    // Boundary values test (including NaN input)
    printf("Test 2: Boundary values and NaN input\n");
    float boundary_src1[8] ALIGNED(32) = {
        0.0f, 1.0f, INFINITY, -1.0f, 
        0.25f, 2.25f, 100.0f, 10000.0f
    };
    float boundary_expected[8] ALIGNED(32) = {
        0.0f, 1.0f, INFINITY, NAN,
        0.5f, 1.5f, 10.0f, 100.0f
    };
    float boundary_result[8] ALIGNED(32) = {0};
    
    float nan_src1[8] ALIGNED(32) = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    float nan_expected[8] ALIGNED(32) = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    float nan_result[8] ALIGNED(32) = {0};
    
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vsqrtps %%ymm0, %%ymm1\n\t"
        "vmovaps %%ymm1, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1)
        : "ymm0", "ymm1"
    );
    
    // Execute test for NaN input
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vsqrtps %%ymm0, %%ymm1\n\t"
        "vmovaps %%ymm1, %0\n\t"
        : "=m"(*nan_result)
        : "m"(*nan_src1)
        : "ymm0", "ymm1"
    );
    
    printf("Boundary Input:    ");
    print_float_vec("Input", boundary_src1, 8);
    printf("Boundary Expected: ");
    print_float_vec("Expected", boundary_expected, 8);
    printf("Boundary Result:   ");
    print_float_vec("Result", boundary_result, 8);
    
    printf("NaN Input:    ");
    print_float_vec("Input", nan_src1, 8);
    printf("NaN Expected: ");
    print_float_vec("Expected", nan_expected, 8);
    printf("NaN Result:   ");
    print_float_vec("Result", nan_result, 8);
    
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i == 3) { // Position 3 should be NaN
            if (!isnan(boundary_result[i])) {
                printf("Mismatch at position %d: expected NaN, got %.6f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (!compare_float(boundary_result[i], boundary_expected[i])) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, boundary_expected[i], boundary_result[i]);
            boundary_pass = 0;
        }
    }
    
    // Check NaN input results
    int nan_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (!isnan(nan_result[i])) {
            printf("Mismatch at NaN position %d: expected NaN, got %.6f\n", 
                   i, nan_result[i]);
            nan_pass = 0;
        }
    }
    
    // Only pass if both boundary and NaN tests pass
    boundary_pass = boundary_pass && nan_pass;
    
    if (boundary_pass) {
        printf("[PASS] Test 2: Boundary values and NaN input\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Boundary values and NaN input\n\n");
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vsqrtps tests passed!\n");
    } else {
        printf("Some vsqrtps tests failed\n");
    }
}

int main() {
    test_vsqrtps();
    return 0;
}
