#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define TOLERANCE 1e-6f

static void test_vaddps() {
    printf("--- Testing vaddps (packed single-precision addition) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 保存原始MXCSR状态
    uint32_t original_mxcsr = get_mxcsr();
    
    // 测试1: 128-bit register-register
    printf("Test 1: 128-bit register-register\n");
    float src1_128[4] ALIGNED(16) = {1.5f, 2.5f, 3.5f, 4.5f};
    float src2_128[4] ALIGNED(16) = {0.5f, 1.5f, 2.5f, 3.5f};
    float expected_128[4] = {2.0f, 4.0f, 6.0f, 8.0f};
    float result_128[4] ALIGNED(16) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vaddps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m"(*result_128)
        : "m"(*src1_128), "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", src1_128, 4);
    printf("Input2:   ");
    print_float_vec("Input2", src2_128, 4);
    printf("Expected: ");
    print_float_vec("Expected", expected_128, 4);
    printf("Result:   ");
    print_float_vec("Result", result_128, 4);
    
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (fabsf(result_128[i] - expected_128[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, expected_128[i], result_128[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 128-bit register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: 128-bit register-register\n\n");
    }
    
    // 测试2: 256-bit register-register test with normal values
    printf("Test 1: 256-bit register-register\n");
    float src1[8] ALIGNED(32) = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};
    float src2[8] ALIGNED(32) = {0.9f, 1.8f, 2.7f, 3.6f, 4.5f, 5.4f, 6.3f, 7.2f};
    float expected[8] ALIGNED(32) = {2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f};
    float result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vaddps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", src1, 8);
    printf("Input2:   ");
    print_float_vec("Input2", src2, 8);
    printf("Expected: ");
    print_float_vec("Expected", expected, 8);
    printf("Result:   ");
    print_float_vec("Result", result, 8);
    
    int pass2 = 1;
    for (int i = 0; i < 8; i++) {
        if (fabsf(result[i] - expected[i]) > TOLERANCE) {
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
    
    // Boundary values test (zeros, ones, NaN, infinity, negative)
    printf("Test 2: Boundary values\n");
    float boundary_src1[8] ALIGNED(32) = {
        0.0f, 1.0f, INFINITY, -1.0f, 
        NAN, -INFINITY, 100.0f, -100.0f
    };
    float boundary_src2[8] ALIGNED(32) = {
        0.0f, 0.0f, 1.0f, 1.0f, 
        1.0f, 1.0f, -50.0f, 50.0f
    };
    float boundary_expected[8] ALIGNED(32) = {
        0.0f, 1.0f, INFINITY, 0.0f,
        NAN, -INFINITY, 50.0f, -50.0f
    };
    float boundary_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vaddps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*boundary_result)
        : "m"(*boundary_src1), "m"(*boundary_src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", boundary_src1, 8);
    printf("Input2:   ");
    print_float_vec("Input2", boundary_src2, 8);
    printf("Expected: ");
    print_float_vec("Expected", boundary_expected, 8);
    printf("Result:   ");
    print_float_vec("Result", boundary_result, 8);
    
    // Check results with hex output for NaN
    printf("Expected (hex): ");
    print_hex_float_vec("Expected", boundary_expected, 8);
    printf("Result (hex):   ");
    print_hex_float_vec("Result", boundary_result, 8);
    
    int boundary_pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i == 4) { // NaN position
            if (!isnan(boundary_result[i])) {
                printf("Mismatch at position %d: expected NaN, got %.6f\n", 
                       i, boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (i == 2 || i == 5) { // Infinity positions
            if (!isinf(boundary_result[i]) || 
                (i == 2 && boundary_result[i] < 0) ||
                (i == 5 && boundary_result[i] > 0)) {
                printf("Mismatch at position %d: expected %s, got %.6f\n", 
                       i, (i == 2) ? "INF" : "-INF", boundary_result[i]);
                boundary_pass = 0;
            }
        } else if (fabsf(boundary_result[i] - boundary_expected[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, boundary_expected[i], boundary_result[i]);
            boundary_pass = 0;
        }
    }
    
    if (boundary_pass) {
        printf("[PASS] Test 2: Boundary values\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Boundary values\n");
    }
    
    // 测试3: 寄存器到内存加法
    printf("Test 3: 256-bit register-memory\n");
    float mem_operand[8] ALIGNED(32) = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    float mem_expected[8] = {1.2f, 2.4f, 3.6f, 4.8f, 6.0f, 7.2f, 8.4f, 9.6f};
    float mem_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vaddps %2, %%ymm0, %%ymm1\n\t"
        "vmovaps %%ymm1, %0\n\t"
        : "=m"(*mem_result)
        : "m"(*src1), "m"(*mem_operand)
        : "ymm0", "ymm1"
    );
    
    printf("Input1:   ");
    print_float_vec("Input1", src1, 8);
    printf("Input2:   ");
    print_float_vec("Input2", mem_operand, 8);
    printf("Expected: ");
    print_float_vec("Expected", mem_expected, 8);
    printf("Result:   ");
    print_float_vec("Result", mem_result, 8);
    
    int pass3 = 1;
    for (int i = 0; i < 8; i++) {
        if (fabsf(mem_result[i] - mem_expected[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                   i, mem_expected[i], mem_result[i]);
            pass3 = 0;
        }
    }
    if (pass3) {
        printf("[PASS] Test 3: 256-bit register-memory\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 3: 256-bit register-memory\n\n");
    }
    
    // 测试4: 非规格化数测试
    printf("Test 4: Denormal values\n");
    float denorm1[8] ALIGNED(32) = {0x1p-126f, 0x1p-127f, 0x0p0f, 1.0f, 0x1p-126f, 0x1p-127f, 0x0p0f, 1.0f};
    float denorm2[8] ALIGNED(32) = {0x1p-126f, 0x1p-127f, 0x1p-149f, 2.0f, 0x1p-126f, 0x1p-127f, 0x1p-149f, 2.0f};
    float denorm_expected[8] = {0x1p-125f, 0x1p-126f, 0x1p-149f, 3.0f, 0x1p-125f, 0x1p-126f, 0x1p-149f, 3.0f};
    float denorm_result[8] ALIGNED(32) = {0};
    total_tests++;
    
    // 设置FTZ和DAZ标志
    uint32_t mxcsr = get_mxcsr();
    mxcsr |= (1 << 15) | (1 << 6);  // 设置FTZ和DAZ
    set_mxcsr(mxcsr);
    
    uint32_t mxcsr_after_denorm = 0;
    __asm__ __volatile__(
        "vmovaps %2, %%ymm0\n\t"
        "vmovaps %3, %%ymm1\n\t"
        "vaddps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        "stmxcsr %1\n\t"
        : "=m"(*denorm_result), "=m"(mxcsr_after_denorm)
        : "m"(*denorm1), "m"(*denorm2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("--- Denormal Test Results ---\n");
    print_float_vec("Result", denorm_result, 8);
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after_denorm);
    
    // 恢复原始MXCSR状态
    set_mxcsr(original_mxcsr);
    
    int pass4 = 1;
    for (int i = 0; i < 8; i++) {
        if (fabsf(denorm_result[i] - denorm_expected[i]) > TOLERANCE) {
            printf("Mismatch at position %d: expected %.16e, got %.16e\n", 
                   i, denorm_expected[i], denorm_result[i]);
            pass4 = 0;
        }
    }
    if (pass4) {
        printf("[PASS] Test 4: Denormal values\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 4: Denormal values\n\n");
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vaddps tests passed!\n");
    } else {
        printf("Some vaddps tests failed\n");
    }
}

int main() {
    test_vaddps();
    return 0;
}
