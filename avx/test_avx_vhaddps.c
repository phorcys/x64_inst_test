#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vhaddps() {
    printf("--- Testing vhaddps (horizontal add packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
    };
    float src2[8] ALIGNED(32) = {
        9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f
    };
    float expected[8] ALIGNED(32) = {
        3.0f, 7.0f,   // src1[0]+src1[1], src1[2]+src1[3]
        19.0f, 23.0f, // src2[0]+src2[1], src2[2]+src2[3]
        11.0f, 15.0f, // src1[4]+src1[5], src1[6]+src1[7]
        27.0f, 31.0f  // src2[4]+src2[5], src2[6]+src2[7]
    };
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vhaddps %%ymm1, %%ymm0, %%ymm2\n\t"
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
    
    // 128-bit register-register test
    printf("Test 2: 128-bit register-register\n");
    float src1_128[4] ALIGNED(16) = {
        10.0f, 20.0f, 30.0f, 40.0f
    };
    float src2_128[4] ALIGNED(16) = {
        50.0f, 60.0f, 70.0f, 80.0f
    };
    float expected_128[4] ALIGNED(16) = {
        30.0f, 70.0f,  // src1_128[0]+src1_128[1], src1_128[2]+src1_128[3]
        110.0f, 150.0f // src2_128[0]+src2_128[1], src2_128[2]+src2_128[3]
    };
    float result_128[4] ALIGNED(16) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vhaddps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m"(*result_128)
        : "m"(*src1_128), "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected_128, 4);
    printf("Result:   ");
    print_float_vec("Result", result_128, 4);
    
    pass = 1;
    for (int i = 0; i < 4; i++) {
        if (fabsf(result_128[i] - expected_128[i]) > 1e-6f) {
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
    
    // Boundary values test (infinity, NaN)
    printf("Test 3: Boundary values\n");
    float boundary_src1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 1.0f, 0.0f, -0.0f, 5.0f, 10.0f
    };
    float boundary_src2[8] ALIGNED(32) = {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
    };
    float boundary_expected[8] ALIGNED(32) = {
        -NAN, NAN,   // INFINITY + (-INFINITY) = -NaN, NaN + 1.0f = NaN
        3.0f, 7.0f,  // src2[0]+src2[1] = 1.0f+2.0f, src2[2]+src2[3] = 3.0f+4.0f
        -0.0f, 15.0f, // src1[4]+src1[5] = 0.0f + (-0.0f) = -0.0f, src1[6]+src1[7] = 5.0f+10.0f
        11.0f, 15.0f  // src2[4]+src2[5] = 5.0f+6.0f, src2[6]+src2[7] = 7.0f+8.0f
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    // Set MXCSR to default before boundary test
    unsigned int mxcsr_default = 0x1F80;
    __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_default));
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vhaddps %%ymm1, %%ymm0, %%ymm2\n\t"
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
        printf("All vhaddps tests passed!\n");
    } else {
        printf("Some vhaddps tests failed\n");
    }
}

int main() {
    test_vhaddps();
    return 0;
}
