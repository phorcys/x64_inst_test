#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vdpps() {
    printf("--- Testing vdpps (dot product of packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 128-bit register-register test
    printf("Test 1: 128-bit register-register\n");
    float src1_128[4] ALIGNED(16) = {1.0f, 2.0f, 3.0f, 4.0f};
    float src2_128[4] ALIGNED(16) = {0.5f, 0.5f, 0.5f, 0.5f};
    float expected_128[4] ALIGNED(16) = {5.0f, 5.0f, 5.0f, 5.0f};
    float result_128[4] ALIGNED(16) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vdpps $0xFF, %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m"(*result_128)
        : "m"(*src1_128), "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected_128, 4);
    printf("Result:   ");
    print_float_vec("Result", result_128, 4);
    
    int pass1 = 1;
    for (int i = 0; i < 4; i++) {
        if (fabsf(result_128[i] - expected_128[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_128[i], result_128[i]);
            pass1 = 0;
        }
    }
    if (pass1) {
        printf("[PASS] Test 1: 128-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: 128-bit register-register\n\n");
    }

    // 256-bit register-register test
    printf("Test 2: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };
    float src2[8] ALIGNED(32) = {
        0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f
    };
    float expected[8] ALIGNED(32) = {
        5.0f, 5.0f, 5.0f, 5.0f,
        13.0f, 13.0f, 13.0f, 13.0f
    };
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vdpps $0xFF, %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass2 = 1;
    for (int i = 0; i < 8; i++) {
        if (isnan(expected[i]) && isnan(result[i])) continue;
        if (fabsf(result[i] - expected[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected[i], result[i]);
            pass2 = 0;
        }
    }
    if (pass2) {
        printf("[PASS] Test 2: 256-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: 256-bit register-register\n\n");
    }

    // Test with partial mask (0x55)
    printf("Test 3: 256-bit with partial mask (0x55)\n");
    float src1_partial[8] ALIGNED(32) = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };
    float src2_partial[8] ALIGNED(32) = {
        0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f
    };
    float expected_partial[8] ALIGNED(32) = {
        2.0f, 0.0f, 2.0f, 0.0f,
        6.0f, 0.0f, 6.0f, 0.0f
    };
    float result_partial[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vdpps $0x55, %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result_partial)
        : "m"(*src1_partial), "m"(*src2_partial)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected_partial, 8);
    printf("Result:   ");
    print_float_vec("Result", result_partial, 8);
    
    int pass3 = 1;
    for (int i = 0; i < 8; i++) {
        if (fabsf(result_partial[i] - expected_partial[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_partial[i], result_partial[i]);
            pass3 = 0;
        }
    }
    if (pass3) {
        printf("[PASS] Test 3: 256-bit with partial mask (0x55)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 3: 256-bit with partial mask (0x55)\n\n");
    }

    // Boundary values test
    printf("Test 4: Boundary values\n");
    float boundary_src1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 0.0f,
        1.0e30f, -1.0e30f, 0.0f, -0.0f
    };
    float boundary_src2[8] ALIGNED(32) = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0e30f, 1.0e30f, INFINITY, -INFINITY
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vdpps $0xFF, %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Result: ");
    print_float_vec("Result", boundary_result, 8);
    
    unsigned int mxcsr = 0;
    __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    printf("MXCSR: 0x%08X\n", mxcsr);
    
    int pass4 = (mxcsr & 1) ? 1 : 0; // Check invalid operation flag
    if (pass4) {
        printf("[PASS] Test 4: Boundary values (invalid operation detected)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 4: Boundary values (no invalid operation detected)\n\n");
    }

    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vdpps tests passed!\n");
    } else {
        printf("Some vdpps tests failed\n");
    }
}

int main() {
    test_vdpps();
    return 0;
}
