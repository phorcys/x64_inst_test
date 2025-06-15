#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define TOLERANCE 1e-6f

static void test_vaddsubps() {
    printf("--- Testing vaddsubps (packed single-precision add/sub) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float src2[8] ALIGNED(32) = {0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f};
    float result[8] ALIGNED(32) = {0};
    float expected[8] = {
        1.0f - 0.5f, 2.0f + 1.5f, 
        3.0f - 2.5f, 4.0f + 3.5f,
        5.0f - 4.5f, 6.0f + 5.5f,
        7.0f - 6.5f, 8.0f + 7.5f
    };
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vaddsubps %%ymm1, %%ymm0, %%ymm2\n\t"
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
    
    // 128-bit register-register test
    printf("Test 2: 128-bit register-register\n");
    float src1_128[4] ALIGNED(16) = {10.0f, 20.0f, 30.0f, 40.0f};
    float src2_128[4] ALIGNED(16) = {1.0f, 2.0f, 3.0f, 4.0f};
    float result_128[4] ALIGNED(16) = {0};
    float expected_128[4] = {
        10.0f - 1.0f, 20.0f + 2.0f,
        30.0f - 3.0f, 40.0f + 4.0f
    };
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vaddsubps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m"(*result_128)
        : "m"(*src1_128), "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", src1_128, 4);
    printf("Input2:   ");
    print_float_vec("Input2", src2_128, 4);
    printf("Expected: ");
    print_float_vec("Expected", expected_128, 4);
    printf("Result:   ");
    print_float_vec("Result", result_128, 4);
    
    pass = 1;
    for (int i = 0; i < 4; i++) {
        if (fabsf(result_128[i] - expected_128[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_128[i], result_128[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 2: 128-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: 128-bit register-register\n\n");
    }
    
    // Boundary values test
    printf("Test 3: Boundary values\n");
    float boundary1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 0.0f,
        -100.0f, 100.0f, 1.0e-20f, -1.0e-20f
    };
    float boundary2[8] ALIGNED(32) = {
        1.0f, 1.0f, 1.0f, 1.0f,
        50.0f, -50.0f, 1.0e-20f, 1.0e-20f
    };
    float expected_boundary[8] = {
        INFINITY - 1.0f,   // INF - 1.0f = INF
        -INFINITY + 1.0f,  // -INF + 1.0f = -INF
        NAN - 1.0f,        // NaN - 1.0f = NaN
        0.0f + 1.0f,       // 0.0f + 1.0f = 1.0f
        -100.0f - 50.0f,   // -150.0f
        100.0f + (-50.0f), // 50.0f
        1.0e-20f - 1.0e-20f, // 0.0f
        -1.0e-20f + 1.0e-20f // 0.0f
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    // Save original MXCSR state
    uint32_t original_mxcsr = get_mxcsr();
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vaddsubps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary1), "m"(*boundary2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", boundary1, 8);
    printf("Input2:   ");
    print_float_vec("Input2", boundary2, 8);
    printf("Expected: ");
    print_float_vec("Expected", expected_boundary, 8);
    printf("Result:   ");
    print_float_vec("Result", boundary_result, 8);
    
    // Check results with hex output for NaN
    printf("Expected (hex): ");
    print_hex_float_vec("Expected", expected_boundary, 8);
    printf("Result (hex):   ");
    print_hex_float_vec("Result", boundary_result, 8);
    
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i == 0) { // INF - 1.0f = INF
            if (!isinf(boundary_result[i]) || boundary_result[i] < 0) {
                printf("Mismatch at position %d: expected INF, got %f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (i == 1) { // -INF + 1.0f = -INF
            if (!isinf(boundary_result[i]) || boundary_result[i] > 0) {
                printf("Mismatch at position %d: expected -INF, got %f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (i == 2) { // NaN
            if (!isnan(boundary_result[i])) {
                printf("Mismatch at position %d: expected NaN, got %f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (fabsf(boundary_result[i] - expected_boundary[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_boundary[i], boundary_result[i]);
            boundary_pass = 0;
        }
    }
    
    if (boundary_pass) {
        printf("[PASS] Test 3: Boundary values\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 3: Boundary values\n\n");
    }
    
    // Restore original MXCSR state
    set_mxcsr(original_mxcsr);
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vaddsubps tests passed!\n");
    } else {
        printf("Some vaddsubps tests failed\n");
    }
}

int main() {
    test_vaddsubps();
    return 0;
}
