#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define TOLERANCE 1e-10

static int compare_double(double a, double b) {
    if (isnan(a) && isnan(b)) {
        return 1;
    }
    if (isinf(a) && isinf(b) && (signbit(a) == signbit(b))) {
        return 1;
    }
    return fabs(a - b) < TOLERANCE;
}

static void test_vsubpd() {
    printf("--- Testing vsubpd (subtract packed double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test with normal values
    printf("Test 1: Normal values (256-bit)\n");
    double src1[4] ALIGNED(32) = {10.0, 20.0, 30.0, 40.0};
    double src2[4] ALIGNED(32) = {3.0, 5.0, 7.0, 9.0};
    double expected[4] ALIGNED(32) = {7.0, 15.0, 23.0, 31.0};
    double result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vsubpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input A:  ");
    print_double_vec("A", src1, 4);
    printf("Input B:  ");
    print_double_vec("B", src2, 4);
    printf("Expected: ");
    print_double_vec("Expected", expected, 4);
    printf("Result:   ");
    print_double_vec("Result", result, 4);
    
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (!compare_double(result[i], expected[i])) {
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
    double boundary_src1[4] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 1.0e308
    };
    double boundary_src2[4] ALIGNED(32) = {
        INFINITY, -1.0, 0.0, 1.0e307
    };
    double boundary_expected[4] ALIGNED(32) = {
        NAN, -INFINITY, NAN, 9.0e307
    };
    double boundary_result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vsubpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Boundary Input A:  ");
    print_double_vec("A", boundary_src1, 4);
    printf("Boundary Input B:  ");
    print_double_vec("B", boundary_src2, 4);
    printf("Boundary Expected: ");
    print_double_vec("Expected", boundary_expected, 4);
    printf("Boundary Result:   ");
    print_double_vec("Result", boundary_result, 4);
    
    int boundary_pass = 1;
    for (int i = 0; i < 4; i++) {
        if (i == 0 || i == 2) { // Positions with expected NaN
            if (!isnan(boundary_result[i])) {
                printf("Mismatch at position %d: expected NaN, got %.6f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (!compare_double(boundary_result[i], boundary_expected[i])) {
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
        printf("All vsubpd tests passed!\n");
    } else {
        printf("Some vsubpd tests failed\n");
    }
}

int main() {
    test_vsubpd();
    return 0;
}
