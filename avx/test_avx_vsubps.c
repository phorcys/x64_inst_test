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

static void test_vsubps() {
    printf("--- Testing vsubps (subtract packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test with normal values
    printf("Test 1: Normal values (256-bit)\n");
    float src1[8] ALIGNED(32) = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f};
    float src2[8] ALIGNED(32) = {3.0f, 5.0f, 7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f};
    float expected[8] ALIGNED(32) = {7.0f, 15.0f, 23.0f, 31.0f, 39.0f, 47.0f, 55.0f, 63.0f};
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vsubps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input A:  ");
    print_float_vec("A", src1, 8);
    printf("Input B:  ");
    print_float_vec("B", src2, 8);
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
    
    // Boundary values test (zeros, ones, infinity, NaN)
    printf("Test 2: Boundary values\n");
    float boundary_src1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 1.0e38f,
        0.0f, -0.0f, 1.0f, -1.0f
    };
    float boundary_src2[8] ALIGNED(32) = {
        INFINITY, -1.0f, 0.0f, 1.0e37f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    float boundary_expected[8] ALIGNED(32) = {
        NAN, -INFINITY, NAN, 9.0e37f,
        0.0f, -0.0f, 0.0f, -2.0f
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vsubps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Boundary Input A:  ");
    print_float_vec("A", boundary_src1, 8);
    printf("Boundary Input B:  ");
    print_float_vec("B", boundary_src2, 8);
    printf("Boundary Expected: ");
    print_float_vec("Expected", boundary_expected, 8);
    printf("Boundary Result:   ");
    print_float_vec("Result", boundary_result, 8);
    
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i == 0 || i == 2) { // Positions with expected NaN
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
    
    if (boundary_pass) {
        printf("[PASS] Test 2: Boundary values\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Boundary values\n\n");
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vsubps tests passed!\n");
    } else {
        printf("Some vsubps tests failed\n");
    }
}

int main() {
    test_vsubps();
    return 0;
}
