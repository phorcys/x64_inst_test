#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

// 更精确的双精度浮点比较
static int double_equal_ulp(double a, double b, double tolerance, int max_ulps) {
    if (isnan(a) && isnan(b)) return 1;
    if (fabs(a - b) <= tolerance) return 1;
    
    int64_t ai = *(int64_t*)&a;
    int64_t bi = *(int64_t*)&b;
    if ((ai < 0) != (bi < 0)) return 0; // 符号不同
    return llabs(ai - bi) <= max_ulps;
}

static void test_vaddpd() {
    printf("--- Testing vaddpd (packed double-precision addition) ---\n");
    int total_tests = 0;
    int passed_tests = 0;

    // 测试1: 256位寄存器到寄存器加法
    printf("Test 1: 256-bit register-register addition\n");
    double src1[4] ALIGNED(32) = {1.1, 2.2, 3.3, 4.4};
    double src2[4] ALIGNED(32) = {0.9, 1.8, 2.7, 3.6};
    double result[4] ALIGNED(32) = {0};
    double expected[4] ALIGNED(32) = {2.0, 4.0, 6.0, 8.0};
    total_tests++;

    // 测试256位寄存器到寄存器加法
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vaddpd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    
    printf("256-bit register-register test:\n");
    print_double_vec("Expected", expected, 4);
    print_double_vec("Result", result, 4);
    
    // 验证结果
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (!double_equal(result[i], expected[i], 1e-9)) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", i, expected[i], result[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 256-bit register-register addition\n\n");
        passed_tests++;
    }
    
    // 测试2: 128位寄存器到寄存器加法
    printf("Test 2: 128-bit register-register addition\n");
    double src1_128[2] ALIGNED(16) = {1.5, 2.5};
    double src2_128[2] ALIGNED(16) = {0.5, 1.5};
    double result_128[2] ALIGNED(16) = {0};
    double expected_128[2] = {2.0, 4.0};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vaddpd %%xmm1, %%xmm0, %%xmm0\n\t"
        "vmovapd %%xmm0, %0\n\t"
        : "=m"(result_128)
        : "m"(src1_128), "m"(src2_128)
        : "xmm0", "xmm1"
    );
    
    printf("128-bit register-register test:\n");
    print_double_vec("Expected", expected_128, 2);
    print_double_vec("Result", result_128, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (!double_equal(result_128[i], expected_128[i], 1e-9)) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", i, expected_128[i], result_128[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 2: 128-bit register-register addition\n\n");
        passed_tests++;
    }
    
    // 测试3: 寄存器到内存加法
    printf("Test 3: 256-bit register-memory addition\n");
    double mem_operand[4] ALIGNED(32) = {0.1, 0.2, 0.3, 0.4};
    double expected_mem[4] = {1.2, 2.4, 3.6, 4.8};
    total_tests++;
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vaddpd %2, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(mem_operand)
        : "ymm0"
    );
    
    printf("256-bit register-memory test:\n");
    print_double_vec("Expected", expected_mem, 4);
    print_double_vec("Result", result, 4);
    
    pass = 1;
    for (int i = 0; i < 4; i++) {
        if (!double_equal(result[i], expected_mem[i], 1e-9)) {
            printf("Mismatch at position %d: expected %.6f, got %.6f\n", i, expected_mem[i], result[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 3: 256-bit register-memory addition\n\n");
        passed_tests++;
    }
    
    // 测试4: 边界值测试
    printf("Test 4: Boundary values\n");
    double boundary1[4] ALIGNED(32) = {INFINITY, -INFINITY, NAN, 0.0};
    double boundary2[4] ALIGNED(32) = {1.0, 1.0, 1.0, 1.0};
    total_tests++;
    
    // 保存原始MXCSR状态
    uint32_t original_mxcsr = get_mxcsr();
    uint32_t mxcsr_after = 0;
    
    __asm__ __volatile__(
        "vmovapd %2, %%ymm0\n\t"
        "vmovapd %3, %%ymm1\n\t"
        "vaddpd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        "stmxcsr %1\n\t"
        : "=m"(result), "=m"(mxcsr_after)
        : "m"(boundary1), "m"(boundary2)
        : "ymm0", "ymm1"
    );
    
    printf("--- Boundary Test Results ---\n");
    print_double_vec("Result", result, 4);
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after);
    
    // 恢复原始MXCSR状态
    set_mxcsr(original_mxcsr);
    
    pass = 1;
    if (!isinf(result[0]) || result[0] < 0) {
        printf("Mismatch at position 0: expected INF, got %f\n", result[0]);
        pass = 0;
    }
    if (!isinf(result[1]) || result[1] > 0) {
        printf("Mismatch at position 1: expected -INF, got %f\n", result[1]);
        pass = 0;
    }
    if (!isnan(result[2])) {
        printf("Mismatch at position 2: expected NAN, got %f\n", result[2]);
        pass = 0;
    }
    if (result[3] != 1.0) {
        printf("Mismatch at position 3: expected 1.0, got %f\n", result[3]);
        pass = 0;
    }
    
    if (pass) {
        printf("[PASS] Test 4: Boundary values\n\n");
        passed_tests++;
    }
    
    // 测试5: qNaN和sNaN测试
    printf("Test 5: qNaN and sNaN handling\n");
    double nan1[4] ALIGNED(32) = {NAN, 0.0, 1.0, 2.0};
    // 创建sNaN (signaling NaN)
    uint64_t snan_value = 0x7FF0000000000001;
    double snan = *(double*)&snan_value;
    double nan2[4] ALIGNED(32) = {snan, 1.0, snan, 3.0};
    double nan_expected[4] = {NAN, 1.0, NAN, 5.0};
    total_tests++;
    
    uint32_t mxcsr_after_nan = 0;
    __asm__ __volatile__(
        "vmovapd %2, %%ymm0\n\t"
        "vmovapd %3, %%ymm1\n\t"
        "vaddpd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        "stmxcsr %1\n\t"
        : "=m"(result), "=m"(mxcsr_after_nan)
        : "m"(nan1), "m"(nan2)
        : "ymm0", "ymm1"
    );
    
    printf("--- NaN Test Results ---\n");
    print_double_vec("Result", result, 4);
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after_nan);
    
    pass = 1;
    for (int i = 0; i < 4; i++) {
        if (i == 0 || i == 2) {
            if (!isnan(result[i])) {
                printf("Mismatch at position %d: expected NaN, got %f\n", i, result[i]);
                pass = 0;
            }
        } else {
            if (!double_equal_ulp(result[i], nan_expected[i], 1e-9, 4)) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", i, nan_expected[i], result[i]);
                pass = 0;
            }
        }
    }
    
    if (pass) {
        printf("[PASS] Test 5: qNaN and sNaN handling\n\n");
        passed_tests++;
    }
    
    // 测试6: 非规格化数测试
    printf("Test 6: Denormal values\n");
    double denorm1[4] ALIGNED(32) = {0x1p-1022, 0x1p-1023, 0x0p0, 1.0};
    double denorm2[4] ALIGNED(32) = {0x1p-1022, 0x1p-1023, 0x1p-1074, 2.0};
    double denorm_expected[4] = {0x1p-1021, 0x1p-1022, 0x1p-1074, 3.0};
    total_tests++;
    
    // 设置FTZ和DAZ标志
    uint32_t mxcsr = get_mxcsr();
    mxcsr |= (1 << 15) | (1 << 6);  // 设置FTZ和DAZ
    set_mxcsr(mxcsr);
    
    uint32_t mxcsr_after_denorm = 0;
    __asm__ __volatile__(
        "vmovapd %2, %%ymm0\n\t"
        "vmovapd %3, %%ymm1\n\t"
        "vaddpd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        "stmxcsr %1\n\t"
        : "=m"(result), "=m"(mxcsr_after_denorm)
        : "m"(denorm1), "m"(denorm2)
        : "ymm0", "ymm1"
    );
    
    printf("--- Denormal Test Results ---\n");
    print_double_vec("Result", result, 4);
    printf("--- MXCSR State After Operation ---\n");
    print_mxcsr(mxcsr_after_denorm);
    
    // 恢复原始MXCSR状态
    set_mxcsr(original_mxcsr);
    
    pass = 1;
    for (int i = 0; i < 4; i++) {
        if (!double_equal_ulp(result[i], denorm_expected[i], 1e-9, 4)) {
            printf("Mismatch at position %d: expected %.16e, got %.16e\n", i, denorm_expected[i], result[i]);
            pass = 0;
        }
    }
    
    if (pass) {
        printf("[PASS] Test 6: Denormal values\n\n");
        passed_tests++;
    }
    
    // 汇总测试结果
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vaddpd tests passed!\n");
    } else {
        printf("Some vaddpd tests failed\n");
    }
}

int main() {
    test_vaddpd();
    return 0;
}
