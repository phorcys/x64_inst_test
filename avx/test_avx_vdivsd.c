#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <fenv.h>

static void test_vdivsd() {
    printf("=== Testing vdivsd (scalar double-precision division) ===\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // Register-register test
    printf("\n[Test 1: Register-register operation]\n");
    {
        ALIGNED(16) double a[2] = {10.0, 20.0};  // Only low 64-bit used
        ALIGNED(16) double b[2] = {2.0, 3.0};    // Only low 64-bit used
        ALIGNED(16) double result[2] = {0};
        double expected = 5.0;  // 10.0 / 2.0
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        __asm__ __volatile__(
            "vmovapd %1, %%xmm0\n\t"
            "vmovapd %2, %%xmm1\n\t"
            "vdivsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovapd %%xmm2, %0\n\t"
            : "=m"(*result)
            : "m"(*a), "m"(*b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Input A: %.6f (low), %.6f (high)\n", a[0], a[1]);
        printf("Input B: %.6f (low), %.6f (high)\n", b[0], b[1]);
        printf("Result: %.15f (low), %.15f (high)\n", result[0], result[1]);
        printf("Expected: %.15f (low)\n", expected);
        
        int pass = 1;
        if (fabs(result[0] - expected) > 1e-12) {
            printf("Mismatch: expected %.15f, got %.15f\n", expected, result[0]);
            pass = 0;
        }
        if (result[1] != a[1]) {
            printf("High quadword changed unexpectedly: %.15f\n", result[1]);
            pass = 0;
        }
        
        if (pass) {
            printf("[PASS] Test 1: Register-register operation\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 1: Register-register operation\n");
        }
        total_tests++;
    }
    
    // Register-memory test
    printf("\n[Test 2: Register-memory operation]\n");
    {
        ALIGNED(16) double a[2] = {100.0, 200.0};
        ALIGNED(16) double b = 25.0;
        ALIGNED(16) double result[2] = {0};
        double expected = 4.0;  // 100.0 / 25.0
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80;
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        __asm__ __volatile__(
            "vmovapd %1, %%xmm0\n\t"
            "vdivsd %2, %%xmm0, %%xmm1\n\t"
            "vmovapd %%xmm1, %0\n\t"
            : "=m"(*result)
            : "m"(*a), "m"(b)
            : "xmm0", "xmm1"
        );
        
        printf("Input A: %.6f (low), %.6f (high)\n", a[0], a[1]);
        printf("Memory Input: %.6f\n", b);
        printf("Result: %.15f (low), %.15f (high)\n", result[0], result[1]);
        printf("Expected: %.15f (low)\n", expected);
        
        int pass = 1;
        if (fabs(result[0] - expected) > 1e-12) {
            printf("Mismatch: expected %.15f, got %.15f\n", expected, result[0]);
            pass = 0;
        }
        if (result[1] != a[1]) {
            printf("High quadword changed unexpectedly: %.15f\n", result[1]);
            pass = 0;
        }
        
        if (pass) {
            printf("[PASS] Test 2: Register-memory operation\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 2: Register-memory operation\n");
        }
        total_tests++;
    }
    
    // Boundary values test
    printf("\n[Test 3: Boundary values]\n");
    {
        ALIGNED(16) double test_cases[][3] = {
            {INFINITY, 1.0, INFINITY},      // INF / 1 = INF
            {-INFINITY, 1.0, -INFINITY},    // -INF / 1 = -INF
            {NAN, 1.0, NAN},               // NAN / 1 = NAN
            {1.0, 0.0, INFINITY},           // 1 / 0 = INF
            {0.0, 0.0, NAN},               // 0 / 0 = NAN
            {INFINITY, INFINITY, NAN},      // INF / INF = NAN
            {1.0e-100, 1.0e100, 1.0e-200}, // Very small / very large
            {1.0e100, 1.0e-100, 1.0e200}   // Very large / very small
        };
        
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            ALIGNED(16) double a[2] = {test_cases[i][0], 1234.5678};
            ALIGNED(16) double b = test_cases[i][1];
            ALIGNED(16) double result[2] = {0};
            double expected = test_cases[i][2];
            
            CLEAR_FLAGS;
            unsigned int mxcsr_before = 0x1F80;
            __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
            
            __asm__ __volatile__(
                "vmovapd %1, %%xmm0\n\t"
                "vdivsd %2, %%xmm0, %%xmm1\n\t"
                "vmovapd %%xmm1, %0\n\t"
                : "=m"(*result)
                : "m"(*a), "m"(b)
                : "xmm0", "xmm1"
            );
            
            printf("\nTest case %zu:\n", i+1);
            printf("Input A: %.6e (low), %.6f (high)\n", a[0], a[1]);
            printf("Input B: %.6e\n", b);
            printf("Result: %.15e (low), %.15f (high)\n", result[0], result[1]);
            printf("Expected: %.15e (low)\n", expected);
            
            int pass = 1;
            if (isnan(expected)) {
                if (!isnan(result[0])) {
                    printf("Expected NaN but got %.15e\n", result[0]);
                    pass = 0;
                }
            } else if (isinf(expected)) {
                if (!isinf(result[0]) || (result[0] * expected < 0)) {
                    printf("Expected %.15e but got %.15e\n", expected, result[0]);
                    pass = 0;
                }
            } else if (fabs(result[0] - expected) > 1e-12 * fabs(expected)) {
                printf("Mismatch: expected %.15e, got %.15e\n", expected, result[0]);
                pass = 0;
            }
            
            if (result[1] != a[1]) {
                printf("High quadword changed unexpectedly: %.15f\n", result[1]);
                pass = 0;
            }
            
            // Check MXCSR flags
            unsigned int mxcsr_after = 0;
            __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr_after));
            print_mxcsr(mxcsr_after);
            
            if (pass) {
                printf("[PASS] Boundary case %zu\n", i+1);
                passed_tests++;
            } else {
                printf("[FAIL] Boundary case %zu\n", i+1);
            }
            total_tests++;
        }
    }
    
    // Test summary
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vdivsd tests passed!\n");
    } else {
        printf("Some vdivsd tests failed\n");
    }
}

int main() {
    test_vdivsd();
    return 0;
}
