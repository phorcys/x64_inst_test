#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fenv.h>

static void test_vhaddpd() {
    printf("--- Testing vhaddpd (horizontal add packed double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
        unsigned int mxcsr_after = 0;
    // 128-bit register-register test
    printf("Test 1: 128-bit register-register\n");
    double src1_128[2] ALIGNED(16) = {1.0, 2.0};
    double src2_128[2] ALIGNED(16) = {3.0, 4.0};
    double expected_128[2] ALIGNED(16) = {3.0, 7.0}; // [1.0+2.0, 3.0+4.0]
    double result_128[2] ALIGNED(16) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vmovapd %[src2], %%xmm1\n\t"
        "vhaddpd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*result_128), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*src1_128), [src2] "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", expected_128, 2);
    printf("Result:   ");
    print_double_vec("Result", result_128, 2);
    
    int pass1 = 1;
    for (int i = 0; i < 2; i++) {
        if (fabs(result_128[i] - expected_128[i]) > 1e-12) {
            printf("Mismatch at position %d: expected %.12f, got %.12f\n", 
                   i, expected_128[i], result_128[i]);
            pass1 = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass1) {
        printf("[PASS] Test 1: 128-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: 128-bit register-register\n\n");
    }

    // 256-bit register-register test
    printf("Test 2: 256-bit register-register\n");
    double src1[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
    double src2[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0};
    double expected[4] ALIGNED(32) = {3.0, 11.0, 7.0, 15.0}; // [1+2,5+6,3+4,7+8]
    double result[4] ALIGNED(32) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%ymm0\n\t"
        "vmovapd %[src2], %%ymm1\n\t"
        "vhaddpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*src1), [src2] "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", expected, 4);
    printf("Result:   ");
    print_double_vec("Result", result, 4);
    
    int pass2 = 1;
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i]) && isnan(result[i])) continue;
        if (fabs(result[i] - expected[i]) > 1e-12) {
            printf("Mismatch at position %d: expected %.12f, got %.12f\n", 
                   i, expected[i], result[i]);
            pass2 = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass2) {
        printf("[PASS] Test 2: 256-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: 256-bit register-register\n\n");
    }

    // Memory operand test
    printf("Test 3: Memory operand\n");
    double mem_src1[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
    double mem_src2[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0};
    double mem_expected[4] ALIGNED(32) = {3.0, 11.0, 7.0, 15.0};
    double mem_result[4] ALIGNED(32) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%ymm0\n\t"
        "vhaddpd %[src2], %%ymm0, %%ymm1\n\t"
        "vmovapd %%ymm1, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*mem_result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*mem_src1), [src2] "m"(*mem_src2)
        : "ymm0", "ymm1"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", mem_expected, 4);
    printf("Result:   ");
    print_double_vec("Result", mem_result, 4);
    
    int pass3 = 1;
    for (int i = 0; i < 4; i++) {
        if (fabs(mem_result[i] - mem_expected[i]) > 1e-12) {
            printf("Mismatch at position %d: expected %.12f, got %.12f\n", 
                   i, mem_expected[i], mem_result[i]);
            pass3 = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass3) {
        printf("[PASS] Test 3: Memory operand\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 3: Memory operand\n\n");
    }

    // Boundary values test
    printf("Test 4: Boundary values\n");
    double boundary_src1[4] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 0.0
    };
    double boundary_src2[4] ALIGNED(32) = {
        1.0, 1.0, 1.0, 1.0
    };
    double boundary_expected[4] ALIGNED(32) = {
        NAN, NAN, NAN, 2.0  // 0.0+1.0 + 1.0+1.0 (due to horizontal add behavior)
    };
    double boundary_result[4] ALIGNED(32) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%ymm0\n\t"
        "vmovapd %[src2], %%ymm1\n\t"
        "vhaddpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*boundary_result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*boundary_src1), [src2] "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", boundary_expected, 4);
    printf("Result:   ");
    print_double_vec("Result", boundary_result, 4);
    
    int pass4 = 1;
    for (int i = 0; i < 4; i++) {
        if (isnan(boundary_expected[i]) && isnan(boundary_result[i])) continue;
        if (fabs(boundary_result[i] - boundary_expected[i]) > 1e-12) {
            printf("Mismatch at position %d: expected %.12f, got %.12f\n", 
                   i, boundary_expected[i], boundary_result[i]);
            pass4 = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    // Check if any exception flags are set
    int pass4_flag = (mxcsr_after & (FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_DIVBYZERO)) ? 1 : 0;
    
    if (pass4 && pass4_flag) {
        printf("[PASS] Test 4: Boundary values (exception detected)\n\n");
        passed_tests++;
    } else if (pass4) {
        printf("[PASS] Test 4: Boundary values (no exception)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 4: Boundary values\n\n");
    }
    
    // Check MXCSR state
    unsigned int mxcsr = 0;
    __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    printf("--- MXCSR State After Operations ---\n");
    printf("MXCSR: 0x%08X\n", mxcsr);
    printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
           (mxcsr >> 0) & 1,  // Invalid
           (mxcsr >> 1) & 1,  // Denormal
           (mxcsr >> 2) & 1,  // Divide-by-zero
           (mxcsr >> 3) & 1,  // Overflow
           (mxcsr >> 4) & 1,  // Underflow
           (mxcsr >> 5) & 1); // Precision

    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vhaddpd tests passed!\n");
    } else {
        printf("Some vhaddpd tests failed\n");
    }
}

int main() {
    test_vhaddpd();
    return 0;
}
