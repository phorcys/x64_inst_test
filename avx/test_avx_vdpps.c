#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vdpps() {
    printf("--- Testing vdpps (dot product of packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };
    float src2[8] ALIGNED(32) = {
        0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f
    };
    float expected[8] ALIGNED(32) = {
        5.0f, 5.0f, 5.0f, 5.0f,   // First lane: (1*0.5 + 2*0.5 + 3*0.5 + 4*0.5) = 5.0
        13.0f, 13.0f, 13.0f, 13.0f // Second lane: (5*0.5 + 6*0.5 + 7*0.5 + 8*0.5) = 13.0
    };
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    // Immediate value control mask
    const int imm8 = 0xFF; // Use all components
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vdpps %3, %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2), "i"(imm8)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        // For NaN values, use isnan to check
        if (isnan(expected[i]) && isnan(result[i])) continue;
        
        if (fabsf(result[i] - expected[i]) > 1e-6) {
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
    
    // Check MXCSR state
    unsigned int mxcsr = 0;
    __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    printf("--- MXCSR State After Operation ---\n");
    printf("MXCSR: 0x%08X\n", mxcsr);
    printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
           (mxcsr >> 7) & 1, (mxcsr >> 8) & 1, (mxcsr >> 9) & 1,
           (mxcsr >> 10) & 1, (mxcsr >> 11) & 1, (mxcsr >> 12) & 1);
    
    // Boundary values test
    printf("Test 2: Boundary values\n");
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
        "vdpps %3, %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2), "i"(imm8)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Result: ");
    print_float_vec("Result", boundary_result, 8);
    
    // Check MXCSR state for boundary test
    mxcsr = 0;
    __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    printf("--- MXCSR State After Boundary Test ---\n");
    printf("MXCSR: 0x%08X\n", mxcsr);
    printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
           (mxcsr >> 7) & 1, (mxcsr >> 8) & 1, (mxcsr >> 9) & 1,
           (mxcsr >> 10) & 1, (mxcsr >> 11) & 1, (mxcsr >> 12) & 1);
    
    // We expect invalid operation flag (bit 0) to be set due to NaN operations
    if (mxcsr & 1) {
        printf("[PASS] Test 2: Boundary values (invalid operation detected)\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Boundary values (no invalid operation detected)\n\n");
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
