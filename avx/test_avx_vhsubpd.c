#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vhsubpd() {
    printf("--- Testing vhsubpd (horizontal subtract packed double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    double src1[4] ALIGNED(32) = {
        10.0, 3.0, 8.0, 2.0
    };
    double src2[4] ALIGNED(32) = {
        15.0, 5.0, 20.0, 4.0
    };
    double expected[4] ALIGNED(32) = {
        7.0, 10.0,   // src1[1]-src1[0] = 3.0-10.0, src2[1]-src2[0] = 5.0-15.0
        6.0, 16.0    // src1[3]-src1[2] = 2.0-8.0, src2[3]-src2[2] = 4.0-20.0
    };
    double result[4] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vhsubpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", expected, 4);
    printf("Result:   ");
    print_double_vec("Result", result, 4);
    
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (fabs(result[i] - expected[i]) > 1e-6) {
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
    double boundary_src1[4] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 1.0
    };
    double boundary_src2[4] ALIGNED(32) = {
        5.0, 6.0, 7.0, 8.0
    };
    double boundary_expected[4] ALIGNED(32) = {
        INFINITY, -1.0,   // INFINITY - (-INFINITY) = INFINITY, 5.0-6.0 = -1.0
        NAN, -1.0         // 1.0 - NAN = NAN, 7.0-8.0 = -1.0
    };
    double boundary_result[4] ALIGNED(32) = {0};
    total_tests++;
    
    // Set MXCSR to default before boundary test
    unsigned int mxcsr_default = 0x1F80;
    __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_default));
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vhsubpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", boundary_expected, 4);
    printf("Result:   ");
    print_double_vec("Result", boundary_result, 4);
    
    // Check results
    int boundary_pass = 1;
    for (int i = 0; i < 4; i++) {
        int is_nan_expected = isnan(boundary_expected[i]);
        int is_nan_result = isnan(boundary_result[i]);
        
        if (is_nan_expected && is_nan_result) continue;
        
        if (fabs(boundary_result[i] - boundary_expected[i]) > 1e-6) {
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
        printf("All vhsubpd tests passed!\n");
    } else {
        printf("Some vhsubpd tests failed\n");
    }
}

int main() {
    test_vhsubpd();
    return 0;
}
