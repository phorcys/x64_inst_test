#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vdppd() {
    printf("--- Testing vdppd (dot product of double-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 128-bit register-register test
    printf("Test 1: 128-bit register-register\n");
    double src1[2] ALIGNED(16) = {1.0, 2.0};
    double src2[2] ALIGNED(16) = {0.5, 0.5};
    double expected[2] ALIGNED(16) = {1.5, 0.0}; // Dot product: 1.0*0.5 + 2.0*0.5 = 1.5
    double result[2] ALIGNED(16) = {0};
    total_tests++;
    
    // Immediate value control mask (0x31: use both elements)
    
    unsigned int mxcsr_after = 0;
    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vmovapd %[src2], %%xmm1\n\t"
        "vdppd $0x31, %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*src1), [src2] "m"(*src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", expected, 2);
    printf("Result:   ");
    print_double_vec("Result", result, 2);
    
    int pass = 1;
    for (int i = 0; i < 2; i++) {
        if (fabs(result[i] - expected[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected[i], result[i]);
            pass = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass) {
        printf("[PASS] Test 1: 128-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: 128-bit register-register\n\n");
    }
    
    // Test with different mask (0x11: use only first element)
    printf("Test 2: Different mask (0x11)\n");
    double src1_mask[2] ALIGNED(16) = {3.0, 4.0};
    double src2_mask[2] ALIGNED(16) = {0.5, 0.5};
    double expected_mask[2] ALIGNED(16) = {1.5, 0.0}; // Only first element: 3.0*0.5 = 1.5
    double result_mask[2] ALIGNED(16) = {0};
    total_tests++;
    
    // Immediate value control mask (0x11: use only first element)
    

    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vmovapd %[src2], %%xmm1\n\t"
        "vdppd $0x11, %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*result_mask), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*src1_mask), [src2] "m"(*src2_mask)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", expected_mask, 2);
    printf("Result:   ");
    print_double_vec("Result", result_mask, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (fabs(result_mask[i] - expected_mask[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_mask[i], result_mask[i]);
            pass = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass) {
        printf("[PASS] Test 2: Different mask (0x11)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Different mask (0x11)\n\n");
    }
    
    // Boundary values test
    printf("Test 3: Boundary values\n");
    double boundary_src1[2] ALIGNED(16) = {INFINITY, NAN};
    double boundary_src2[2] ALIGNED(16) = {1.0, 1.0};
    double boundary_expected[2] ALIGNED(16) = {INFINITY, 0.0}; // INF*1.0 + NAN*1.0 = INF + NAN = NAN
    double boundary_result[2] ALIGNED(16) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vmovapd %[src2], %%xmm1\n\t"
        "vdppd $0x31, %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*boundary_result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*boundary_src1), [src2] "m"(*boundary_src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    // Check for NaN in result[0]
    if (isnan(boundary_result[0])) {
        boundary_expected[0] = NAN;
    }
    
    printf("Expected: ");
    print_double_vec("Expected", boundary_expected, 2);
    printf("Result:   ");
    print_double_vec("Result", boundary_result, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (isnan(boundary_expected[i]) && isnan(boundary_result[i])) continue;
        if (fabs(boundary_result[i] - boundary_expected[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, boundary_expected[i], boundary_result[i]);
            pass = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass) {
        printf("[PASS] Test 3: Boundary values\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 3: Boundary values\n\n");
    }
    
    // Test with high elements only (0x22)
    printf("Test 4: High elements only (0x22)\n");
    double src1_high[2] ALIGNED(16) = {1.0, 2.0};
    double src2_high[2] ALIGNED(16) = {0.5, 0.5};
    double expected_high[2] ALIGNED(16) = {0.0, 1.0}; // Only high element: 2.0*0.5 = 1.0 (stored in high element)
    double result_high[2] ALIGNED(16) = {0};
    total_tests++;
    

    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vmovapd %[src2], %%xmm1\n\t"
        "vdppd $0x22, %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*result_high), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*src1_high), [src2] "m"(*src2_high)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", expected_high, 2);
    printf("Result:   ");
    print_double_vec("Result", result_high, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (fabs(result_high[i] - expected_high[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_high[i], result_high[i]);
            pass = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass) {
        printf("[PASS] Test 4: High elements only (0x22)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 4: High elements only (0x22)\n\n");
    }

    // Check MXCSR state
    unsigned int mxcsr = 0;
    __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    printf("--- MXCSR State After Operations ---\n");
    printf("MXCSR: 0x%08X\n", mxcsr);
    printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
           (mxcsr >> 7) & 1, (mxcsr >> 8) & 1, (mxcsr >> 9) & 1,
           (mxcsr >> 10) & 1, (mxcsr >> 11) & 1, (mxcsr >> 12) & 1);

    // Memory operand test
    printf("Test 5: Memory operand\n");
    double mem_src1[2] ALIGNED(16) = {2.0, 3.0};
    double mem_src2[2] ALIGNED(16) = {1.0, 1.0};
    double mem_expected[2] ALIGNED(16) = {5.0, 0.0}; // 2.0*1.0 + 3.0*1.0 = 5.0
    double mem_result[2] ALIGNED(16) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vdppd $0x31, %[src2], %%xmm0, %%xmm1\n\t"
        "vmovapd %%xmm1, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*mem_result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*mem_src1), [src2] "m"(*mem_src2)
        : "xmm0", "xmm1"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", mem_expected, 2);
    printf("Result:   ");
    print_double_vec("Result", mem_result, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (fabs(mem_result[i] - mem_expected[i]) > 1e-6) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, mem_expected[i], mem_result[i]);
            pass = 0;
        }
    }
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass) {
        printf("[PASS] Test 5: Memory operand\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 5: Memory operand\n\n");
    }

    // NaN propagation test
    printf("Test 6: NaN propagation\n");
    double nan_src1[2] ALIGNED(16) = {NAN, 1.0};
    double nan_src2[2] ALIGNED(16) = {1.0, 1.0};
    double nan_expected[2] ALIGNED(16) = {NAN, 0.0}; // NAN*1.0 + 1.0*1.0 = NAN
    double nan_result[2] ALIGNED(16) = {0};
    total_tests++;
    
    
    __asm__ __volatile__(
        "vmovapd %[src1], %%xmm0\n\t"
        "vmovapd %[src2], %%xmm1\n\t"
        "vdppd $0x31, %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovapd %%xmm2, %[result]\n\t"
        "stmxcsr %[mxcsr]"
        : [result] "=m"(*nan_result), [mxcsr] "=m"(mxcsr_after)
        : [src1] "m"(*nan_src1), [src2] "m"(*nan_src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Expected: ");
    print_double_vec("Expected", nan_expected, 2);
    printf("Result:   ");
    print_double_vec("Result", nan_result, 2);
    
    pass = isnan(nan_result[0]) && (fabs(nan_result[1] - nan_expected[1]) < 1e-6);
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    if (pass) {
        printf("[PASS] Test 6: NaN propagation\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 6: NaN propagation\n\n");
    }

    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vdppd tests passed!\n");
    } else {
        printf("Some vdppd tests failed\n");
    }
}

int main() {
    test_vdppd();
    return 0;
}
