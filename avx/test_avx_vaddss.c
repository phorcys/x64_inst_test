#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

#define TOLERANCE 1e-6f

static void test_vaddss() {
    printf("--- Testing vaddss (scalar single-precision addition) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 保存原始MXCSR状态
    uint32_t original_mxcsr = get_mxcsr();
    
    // 测试1: 寄存器到寄存器
    printf("Test 1: Register-register\n");
    float src1 = 1.234567f;
    float src2 = 9.876543f;
    float expected = src1 + src2;
    float result = 0;
    total_tests++;
    
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"
        "vmovss %2, %%xmm1\n\t"
        "vaddss %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovss %%xmm2, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Input1:   %.7f\n", src1);
    printf("Input2:   %.7f\n", src2);
    printf("Expected: %.7f\n", expected);
    printf("Result:   %.7f\n", result);
    
    int pass = fabsf(result - expected) < TOLERANCE;
    if (pass) {
        printf("[PASS] Test 1: Register-register\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 1: Register-register\n\n");
    }
    
    // 测试2: 寄存器到内存
    printf("Test 2: Register-memory\n");
    float mem_operand = 1.0e-7f;
    float mem_expected = src1 + mem_operand;
    float mem_result = 0;
    total_tests++;
    
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"
        "vaddss %2, %%xmm0, %%xmm1\n\t"
        "vmovss %%xmm1, %0\n\t"
        : "=m"(mem_result)
        : "m"(src1), "m"(mem_operand)
        : "xmm0", "xmm1"
    );
    
    printf("Input1:   %.7e\n", src1);
    printf("Input2:   %.7e\n", mem_operand);
    printf("Expected: %.7e\n", mem_expected);
    printf("Result:   %.7e\n", mem_result);
    
    pass = fabsf(mem_result - mem_expected) < TOLERANCE;
    if (pass) {
        printf("[PASS] Test 2: Register-memory\n\n");
        passed_tests++;
    } else {
        printf("[FAIL] Test 2: Register-memory\n\n");
    }
    
    // 测试3: 边界值
    printf("Test 3: Boundary values\n");
    struct {
        float a, b, expected;
    } tests[] = {
        {0.0f, 0.0f, 0.0f},
        {INFINITY, 1.0f, INFINITY},
        {-INFINITY, 1.0f, -INFINITY},
        {NAN, 1.0f, NAN},
        {FLT_MAX, FLT_MAX, INFINITY}, // 溢出测试
        {FLT_MIN, FLT_MIN, 2*FLT_MIN},
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        total_tests++;
        float boundary_result = 0;
        uint32_t mxcsr_after = 0;
        
        __asm__ __volatile__(
            "vmovss %2, %%xmm0\n\t"
            "vmovss %3, %%xmm1\n\t"
            "vaddss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            "stmxcsr %1\n\t"
            : "=m"(boundary_result), "=m"(mxcsr_after)
            : "m"(tests[i].a), "m"(tests[i].b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test %zu: %.7e + %.7e\n", i+1, tests[i].a, tests[i].b);
        printf("Expected: %.7e\n", tests[i].expected);
        printf("Result:   %.7e\n", boundary_result);
        printf("--- MXCSR State After Operation ---\n");
        print_mxcsr(mxcsr_after);
        printf("\n");
        
        if (isnan(tests[i].expected)) {
            pass = isnan(boundary_result);
        } else if (isinf(tests[i].expected)) {
            pass = isinf(boundary_result) && 
                  (signbit(tests[i].expected) == signbit(boundary_result));
        } else {
            pass = fabsf(boundary_result - tests[i].expected) < TOLERANCE;
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
        printf("All vaddss tests passed!\n");
    } else {
        printf("Some vaddss tests failed\n");
    }
}

int main() {
    test_vaddss();
    return 0;
}
