#include "avx.h"
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define TOLERANCE 1e-12

static void test_vaddsd() {
    printf("--- Testing vaddsd (scalar double-precision addition) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 保存原始MXCSR状态
    uint32_t original_mxcsr = get_mxcsr();
    
    // 测试1: 寄存器到寄存器
    printf("Test 1: Register-register\n");
    double src1 = 1.234567890123456;
    double src2 = 9.876543210987654;
    double expected = src1 + src2;
    double result = 0;
    total_tests++;
    
    __asm__ __volatile__(
        "vmovsd %1, %%xmm0\n\t"
        "vmovsd %2, %%xmm1\n\t"
        "vaddsd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovsd %%xmm2, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Input1:   %.15f\n", src1);
    printf("Input2:   %.15f\n", src2);
    printf("Expected: %.15f\n", expected);
    printf("Result:   %.15f\n", result);
    
    int pass = fabs(result - expected) < TOLERANCE;
    if (pass) {
        printf("[PASS] Test 1: Register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: Register-register\n\n");
    }
    
    // 测试2: 寄存器到内存
    printf("Test 2: Register-memory\n");
    double mem_operand = 1.0e-15;
    double mem_expected = src1 + mem_operand;
    double mem_result = 0;
    total_tests++;
    
    __asm__ __volatile__(
        "vmovsd %1, %%xmm0\n\t"
        "vaddsd %2, %%xmm0, %%xmm1\n\t"
        "vmovsd %%xmm1, %0\n\t"
        : "=m"(mem_result)
        : "m"(src1), "m"(mem_operand)
        : "xmm0", "xmm1"
    );
    
    printf("Input1:   %.15e\n", src1);
    printf("Input2:   %.15e\n", mem_operand);
    printf("Expected: %.15e\n", mem_expected);
    printf("Result:   %.15e\n", mem_result);
    
    pass = fabs(mem_result - mem_expected) < TOLERANCE;
    if (pass) {
        printf("[PASS] Test 2: Register-memory\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Register-memory\n\n");
    }
    
    // 测试3: 边界值
    printf("Test 3: Boundary values\n");
    struct {
        double a, b, expected;
    } tests[] = {
        {0.0, 0.0, 0.0},
        {INFINITY, 1.0, INFINITY},
        {-INFINITY, 1.0, -INFINITY},
        {NAN, 1.0, NAN},
        {DBL_MAX, DBL_MAX, INFINITY}, // 溢出测试
        {DBL_MIN, DBL_MIN, 2*DBL_MIN},
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        total_tests++;
        double boundary_result = 0;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vaddsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(boundary_result)
            : "m"(tests[i].a), "m"(tests[i].b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test %zu: %.15e + %.15e\n", i+1, tests[i].a, tests[i].b);
        printf("Expected: %.15e\n", tests[i].expected);
        printf("Result:   %.15e\n", boundary_result);
        
        if (isnan(tests[i].expected)) {
            pass = isnan(boundary_result);
        } else if (isinf(tests[i].expected)) {
            pass = isinf(boundary_result) && 
                  (signbit(tests[i].expected) == signbit(boundary_result));
        } else {
            pass = fabs(boundary_result - tests[i].expected) < TOLERANCE;
        }
        
        if (pass) {
            printf("[PASS] Test %zu: Boundary value\n\n", i+1);
            passed_tests++;
        } else {
            printf("[FAIL] Test %zu: Boundary value\n\n", i+1);
        }
    }
    
    // 恢复原始MXCSR状态
    set_mxcsr(original_mxcsr);
    
    // 测试总结
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vaddsd tests passed!\n");
    } else {
        printf("Some vaddsd tests failed\n");
    }
}

int main() {
    test_vaddsd();
    return 0;
}
