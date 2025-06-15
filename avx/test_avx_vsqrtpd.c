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

static void test_vsqrtpd() {
    printf("--- Testing vsqrtpd (square root of packed double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test with normal values
    printf("Test 1: Normal values (256-bit)\n");
    double src1[4] ALIGNED(32) = {4.0, 9.0, 16.0, 25.0};
    double expected[4] ALIGNED(32) = {2.0, 3.0, 4.0, 5.0};
    double result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vsqrtpd %%ymm0, %%ymm1\n\t"
        "vmovapd %%ymm1, %0\n\t"
        : "=m"(*result)
        : "m"(*src1)
        : "ymm0", "ymm1"
    );
    
    printf("Input:    ");
    print_double_vec("Input", src1, 4);
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
    
    // Boundary values test (including NaN input)
    printf("Test 2: Boundary values and NaN input\n");
    double boundary_src1[4] ALIGNED(32) = {
        0.0, 1.0, INFINITY, -1.0
    };
    double boundary_expected[4] ALIGNED(32) = {
        0.0, 1.0, INFINITY, NAN
    };
    double boundary_result[4] ALIGNED(32) = {0};
    
    double nan_src1[4] ALIGNED(32) = {NAN, NAN, NAN, NAN};
    double nan_expected[4] ALIGNED(32) = {NAN, NAN, NAN, NAN};
    double nan_result[4] ALIGNED(32) = {0};
    
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vsqrtpd %%ymm0, %%ymm1\n\t"
        "vmovapd %%ymm1, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1)
        : "ymm0", "ymm1"
    );
    
    // Execute test for NaN input
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vsqrtpd %%ymm0, %%ymm1\n\t"
        "vmovapd %%ymm1, %0\n\t"
        : "=m"(*nan_result)
        : "m"(*nan_src1)
        : "ymm0", "ymm1"
    );
    
    printf("Boundary Input:    ");
    print_double_vec("Input", boundary_src1, 4);
    printf("Boundary Expected: ");
    print_double_vec("Expected", boundary_expected, 4);
    printf("Boundary Result:   ");
    print_double_vec("Result", boundary_result, 4);
    
    printf("NaN Input:    ");
    print_double_vec("Input", nan_src1, 4);
    printf("NaN Expected: ");
    print_double_vec("Expected", nan_expected, 4);
    printf("NaN Result:   ");
    print_double_vec("Result", nan_result, 4);
    
    // print_double_vec 函数已经可以打印十六进制值
    // 不需要额外的十六进制输出
    
    int boundary_pass = 1;
    for (int i = 0; i < 4; i++) {
        if (i == 3) { // Position 3 should be NaN
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
    // 对于位置3的 NaN 值，我们只关心它是否是 NaN，不关心符号
    // 所以即使输出是 -nan，也视为通过
    
    // Check NaN input results
    int nan_pass = 1;
    for (int i = 0; i < 4; i++) {
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
        printf("All vsqrtpd tests passed!\n");
    } else {
        printf("Some vsqrtpd tests failed\n");
    }
}

int main() {
    test_vsqrtpd();
    return 0;
}
