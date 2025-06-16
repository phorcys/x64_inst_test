#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vhsubps() {
    printf("--- Testing vhsubps (horizontal subtract packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {
        10.0f, 3.0f, 8.0f, 2.0f, 15.0f, 5.0f, 20.0f, 4.0f
    };
    float src2[8] ALIGNED(32) = {
        15.0f, 5.0f, 20.0f, 4.0f, 10.0f, 3.0f, 8.0f, 2.0f
    };
    float expected[8] ALIGNED(32) = {
        7.0f, 6.0f, 10.0f, 16.0f, 10.0f, 16.0f, 7.0f, 6.0f
    };
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vhsubps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (fabsf(result[i] - expected[i]) > 1e-6f) {
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
    
    // Boundary values test (infinity, NaN)
    printf("Test 2: Boundary values\n");
    float boundary_src1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 1.0f, 6.0f, 5.0f, 8.0f, 7.0f
    };
    float boundary_src2[8] ALIGNED(32) = {
        5.0f, 6.0f, 7.0f, 8.0f, INFINITY, -INFINITY, NAN, 1.0f
    };
    float boundary_expected[8] ALIGNED(32) = {
        INFINITY, NAN, -1.0f, -1.0f, 1.0f, 1.0f, INFINITY, NAN
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    // Set MXCSR to default before boundary test
    unsigned int mxcsr_default = 0x1F80;
    __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_default));
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vhsubps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", boundary_expected, 8);
    printf("Result:   ");
    print_float_vec("Result", boundary_result, 8);
    
    // Check results
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        int is_nan_expected = isnan(boundary_expected[i]);
        int is_nan_result = isnan(boundary_result[i]);
        
        if (is_nan_expected && is_nan_result) continue;
        
        if (fabsf(boundary_result[i] - boundary_expected[i]) > 1e-6f) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, boundary_expected[i], boundary_result[i]);
            boundary_pass = 0;
        }
    }
    
    if (boundary_pass) {
        printf("[PASS] Boundary values output check\n");
        passed_tests++;
    } else {
        printf("[FAIL] Boundary values output check\n");
    }
    
    // // Check MXCSR state
    // unsigned int mxcsr = 0;
    // __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    // printf("--- MXCSR State After Operation ---\n");
    // printf("MXCSR: 0x%08X\n", mxcsr);
    // printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
    //        (mxcsr >> 7) & 1, (mxcsr >> 8) & 1, (mxcsr >> 9) & 1,
    //        (mxcsr >> 10) & 1, (mxcsr >> 11) & 1, (mxcsr >> 12) & 1);
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vhsubps tests passed!\n");
    } else {
        printf("Some vhsubps tests failed\n");
    }
}

int main() {
    test_vhsubps();
    return 0;
}
