#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

static void test_vrcpps() {
    printf("--- Testing vrcpps (reciprocal of packed single-precision) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: Basic values\n");
    float src[8] ALIGNED(32) = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f};
    float expected[8] ALIGNED(32) = {
        1.0f/1.0f, 1.0f/2.0f, 1.0f/4.0f, 1.0f/8.0f,
        1.0f/16.0f, 1.0f/32.0f, 1.0f/64.0f, 1.0f/128.0f
    };
    
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vrcpps %%ymm0, %%ymm1\n\t"
        "vmovaps %%ymm1, %0\n\t"
        : "=m"(*result)
        : "m"(*src)
        : "ymm0", "ymm1"
    );
    
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass = 1;
    float tolerance = 1e-3f; // vrcpps 有大约 1.5*2^-12 的精度
    for (int i = 0; i < 8; i++) {
        float diff = fabsf(result[i] - expected[i]);
        if (diff > tolerance) {
            printf("Mismatch at position %d: expected %.6f, got %.6f (diff: %.6f)\n", 
                   i, expected[i], result[i], diff);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: Basic values\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: Basic values\n\n");
    }
    
    // Boundary values test
    printf("Test 2: Boundary values\n");
    float boundary_src[8] ALIGNED(32) = {
        0.0f, -0.0f, INFINITY, -INFINITY, 
        NAN, 1e-10f, 1e10f, -1e10f
    };
    float boundary_expected[8] ALIGNED(32) = {
        INFINITY, -INFINITY, 0.0f, -0.0f,
        NAN, 1e10f, 1e-10f, -1e-10f
    };
    
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vrcpps %%ymm0, %%ymm1\n\t"
        "vmovaps %%ymm1, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src)
        : "ymm0", "ymm1"
    );
    
    printf("Expected: ");
    print_hex_float_vec("Expected", boundary_expected, 8);
    printf("Result:   ");
    print_hex_float_vec("Result", boundary_result, 8);
    
    // Check results
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i < 4) { // 前4个有明确定义的行为
            if (memcmp(&boundary_result[i], &boundary_expected[i], sizeof(float))) {
                printf("Mismatch at position %d: expected 0x%08X, got 0x%08X\n", 
                       i, *(uint32_t*)&boundary_expected[i], *(uint32_t*)&boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (i == 4) { // NaN
            if (!isnan(boundary_result[i])) {
                printf("Expected NaN at position %d, got 0x%08X\n", 
                       i, *(uint32_t*)&boundary_result[i]);
                boundary_pass = 0;
            }
        } else { // 大/小值，使用相对误差检查
            float expected_val = boundary_expected[i];
            float result_val = boundary_result[i];
            float rel_error = fabsf((result_val - expected_val) / expected_val);
            if (rel_error > 0.01f) { // 1% 相对误差
                printf("Mismatch at position %d: expected %.6e, got %.6e (rel error: %.2f%%)\n", 
                       i, expected_val, result_val, rel_error*100);
                boundary_pass = 0;
            }
        }
    }
    
    if (boundary_pass) {
        printf("[PASS] Boundary values\n");
        passed_tests++;
    } else {
        printf("[FAIL] Boundary values\n");
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vrcpps tests passed!\n");
    } else {
        printf("Some vrcpps tests failed\n");
    }
}

int main() {
    test_vrcpps();
    return 0;
}
