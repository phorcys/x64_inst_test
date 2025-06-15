#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>

static void test_vaddsubpd() {
    printf("--- Testing vaddsubpd (packed double-precision add/sub) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    double src1[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
    double src2[4] ALIGNED(32) = {0.5, 1.5, 2.5, 3.5};
    double result[4] ALIGNED(32) = {0};
    double expected[4] = {1.0-0.5, 2.0+1.5, 3.0-2.5, 4.0+3.5}; // 0.5, 3.5, 0.5, 7.5
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vaddsubpd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    
    printf("256-bit register-register test:\n");
    print_double_vec("Expected", expected, 4);
    print_double_vec("Result", result, 4);
    
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (!double_equal(result[i], expected[i], 1e-9)) {
            printf("Mismatch at position %d: expected %.9f, got %.9f\n", i, expected[i], result[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 256-bit register-register\n\n");
        passed_tests++;
    }
    
    // 128-bit register-register test
    printf("Test 2: 128-bit register-register\n");
    double src1_128[2] ALIGNED(16) = {10.0, 20.0};
    double src2_128[2] ALIGNED(16) = {1.0, 2.0};
    double result_128[2] ALIGNED(16) = {0};
    double expected_128[2] = {10.0-1.0, 20.0+2.0}; // 9.0, 22.0
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vaddsubpd %%xmm1, %%xmm0, %%xmm0\n\t"
        "vmovapd %%xmm0, %0\n\t"
        : "=m"(result_128)
        : "m"(src1_128), "m"(src2_128)
        : "xmm0", "xmm1"
    );
    
    printf("128-bit register-register test:\n");
    print_double_vec("Expected", expected_128, 2);
    print_double_vec("Result", result_128, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (!double_equal(result_128[i], expected_128[i], 1e-9)) {
            printf("Mismatch at position %d: expected %.9f, got %.9f\n", i, expected_128[i], result_128[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 2: 128-bit register-register\n\n");
        passed_tests++;
    }
    
    // Boundary values test
    printf("Test 3: Boundary values\n");
    double boundary1[4] ALIGNED(32) = {INFINITY, -INFINITY, NAN, 0.0};
    double boundary2[4] ALIGNED(32) = {1.0, 1.0, 1.0, 1.0};
    total_tests++;
    
    // Save original MXCSR state
    uint32_t original_mxcsr = get_mxcsr();
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vaddsubpd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(boundary1), "m"(boundary2)
        : "ymm0", "ymm1"
    );
    
    printf("--- Boundary Test Results ---\n");
    print_double_vec("Result", result, 4);
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(get_mxcsr());
    
    // Restore original MXCSR state
    set_mxcsr(original_mxcsr);
    
    pass = 1;
    if (!isinf(result[0])) {
        printf("Mismatch at position 0: expected INF, got %f\n", result[0]);
        pass = 0;
    }
    if (!isinf(result[1])) {
        printf("Mismatch at position 1: expected -INF, got %f\n", result[1]);
        pass = 0;
    }
    if (!isnan(result[2])) {
        printf("Mismatch at position 2: expected NAN, got %f\n", result[2]);
        pass = 0;
    }
    if (result[3] != 1.0) {
        printf("Mismatch at position 3: expected 1.0, got %f\n", result[3]);
        pass = 0;
    }
    
    if (pass) {
        printf("[PASS] Test 3: Boundary values\n\n");
        passed_tests++;
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vaddsubpd tests passed!\n");
    } else {
        printf("Some vaddsubpd tests failed\n");
    }
}

int main() {
    test_vaddsubpd();
    return 0;
}
