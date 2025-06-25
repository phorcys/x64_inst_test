#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <fenv.h>

static void test_vdivpd() {
    printf("=== Testing vdivpd (packed double-precision division) ===\n");
    int total_tests = 0;
    int passed_tests = 0;
    int errors = 0;
    
    // 测试256位寄存器-寄存器操作
    printf("\n[Test 1: 256-bit reg-reg]\n");
    {
        ALIGNED(32) double a[4] = {10.0, 20.0, 30.0, 40.0};
        ALIGNED(32) double b[4] = {2.0, 4.0, 5.0, 8.0};
        ALIGNED(32) double c[4] = {0};
        ALIGNED(32) double expected[4] = {5.0, 5.0, 6.0, 5.0};
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%ymm0\n\t"
            "vmovapd %3, %%ymm1\n\t"
            "vdivpd %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovapd %%ymm2, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "ymm0", "ymm1", "ymm2"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 4);
        print_double_vec("Input B", b, 4);
        print_double_vec("Result", c, 4);
        print_double_vec("Expected", expected, 4);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        // 检查标志位（不应有异常）
        if (mxcsr_after_div & 0x3F) {
            printf("Unexpected floating-point exceptions detected!\n");
            pass = 0;
            errors++;
        }
        
        if (pass) {
            printf("[PASS] Test 1: 256-bit reg-reg\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 1: 256-bit reg-reg\n");
        }
        total_tests++;
    }
    
    // 测试256位寄存器-内存操作
    printf("\n[Test 2: 256-bit reg-mem]\n");
    {
        ALIGNED(32) double a[4] = {100.0, 200.0, 300.0, 400.0};
        ALIGNED(32) double b[4] = {10.0, 20.0, 30.0, 40.0};
        ALIGNED(32) double c[4] = {0};
        ALIGNED(32) double expected[4] = {10.0, 10.0, 10.0, 10.0};
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%ymm0\n\t"
            "vdivpd %3, %%ymm0, %%ymm1\n\t"
            "vmovapd %%ymm1, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "ymm0", "ymm1"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 4);
        print_double_vec("Memory Input", b, 4);
        print_double_vec("Result", c, 4);
        print_double_vec("Expected", expected, 4);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        if (pass) {
            printf("[PASS] Test 2: 256-bit reg-mem\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 2: 256-bit reg-mem\n");
        }
        total_tests++;
    }
    
    // 测试128位寄存器-寄存器操作
    printf("\n[Test 3: 128-bit reg-reg]\n");
    {
        ALIGNED(16) double a[2] = {1.23456e200, 2.34567e-200};
        ALIGNED(16) double b[2] = {1.0e100, 1.0e-100};
        ALIGNED(16) double c[2] = {0};
        ALIGNED(16) double expected[2] = {1.23456e100, 2.34567e-100};
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%xmm0\n\t"
            "vmovapd %3, %%xmm1\n\t"
            "vdivpd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovapd %%xmm2, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 2);
        print_double_vec("Input B", b, 2);
        print_double_vec("Result", c, 2);
        print_double_vec("Expected", expected, 2);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.6e, got %.6e\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        if (pass) {
            printf("[PASS] Test 3: 128-bit reg-reg\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 3: 128-bit reg-reg\n");
        }
        total_tests++;
    }
    
    // 测试128位寄存器-内存操作
    printf("\n[Test 4: 128-bit reg-mem]\n");
    {
        ALIGNED(16) double a[2] = {100.0, 200.0};
        ALIGNED(16) double b[2] = {25.0, 50.0};
        ALIGNED(16) double c[2] = {0};
        ALIGNED(16) double expected[2] = {4.0, 4.0};
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%xmm0\n\t"
            "vdivpd %3, %%xmm0, %%xmm1\n\t"
            "vmovapd %%xmm1, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "xmm0", "xmm1"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 2);
        print_double_vec("Memory Input", b, 2);
        print_double_vec("Result", c, 2);
        print_double_vec("Expected", expected, 2);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        if (pass) {
            printf("[PASS] Test 4: 128-bit reg-mem\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 4: 128-bit reg-mem\n");
        }
        total_tests++;
    }
    
    // 测试边界值（256位）
    printf("\n[Test 5: 256-bit boundary values]\n");
    {
        ALIGNED(32) double a[4] = {
            INFINITY, -INFINITY, NAN, 1.0,
        };
        ALIGNED(32) double b[4] = {
            1.0, 0.0, 1.0, 0.0
        };
        ALIGNED(32) double c[4] = {0};
        ALIGNED(32) double expected[4] = {
            INFINITY, -INFINITY, NAN, INFINITY
        };
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%ymm0\n\t"
            "vmovapd %3, %%ymm1\n\t"
            "vdivpd %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovapd %%ymm2, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "ymm0", "ymm1", "ymm2"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 4);
        print_double_vec("Input B", b, 4);
        print_double_vec("Result", c, 4);
        print_double_vec("Expected", expected, 4);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            int is_nan_expected = isnan(expected[i]);
            int is_nan_result = isnan(c[i]);
            
            if (is_nan_expected && is_nan_result) continue;
            
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        // 检查预期的异常标志
        int flags_pass = 1;
        // 期望除零标志 (ZE)
        if (!(mxcsr_after_div & (1 << 2))) {
            printf("[FAIL] Division by zero flag not set\n");
            flags_pass = 0;
            errors++;
        }
        
        if (pass && flags_pass) {
            printf("[PASS] Test 5: 256-bit boundary values\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 5: 256-bit boundary values\n");
        }
        total_tests++;
    }
    
    // 测试边界值（128位）
    printf("\n[Test 6: 128-bit boundary values]\n");
    {
        ALIGNED(16) double a[2] = {
            0.0, INFINITY
        };
        ALIGNED(16) double b[2] = {
            0.0, INFINITY
        };
        ALIGNED(16) double c[2] = {0};
        ALIGNED(16) double expected[2] = {
            NAN, NAN
        };
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%xmm0\n\t"
            "vmovapd %3, %%xmm1\n\t"
            "vdivpd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovapd %%xmm2, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 2);
        print_double_vec("Input B", b, 2);
        print_double_vec("Result", c, 2);
        print_double_vec("Expected", expected, 2);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            int is_nan_expected = isnan(expected[i]);
            int is_nan_result = isnan(c[i]);
            
            if (is_nan_expected && is_nan_result) continue;
            
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        // 检查预期的异常标志
        int flags_pass = 1;
        // 期望无效操作标志 (IE) 用于0/0和INF/INF
        if (!(mxcsr_after_div & (1 << 0))) {
            printf("[FAIL] Invalid operation flag not set\n");
            flags_pass = 0;
            errors++;
        }
        
        if (pass && flags_pass) {
            printf("[PASS] Test 6: 128-bit boundary values\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 6: 128-bit boundary values\n");
        }
        total_tests++;
    }
    
    // 测试舍入模式（默认舍入）
    printf("\n[Test 7: Rounding mode (nearest)]\n");
    {
        ALIGNED(16) double a[2] = {10.0, 20.0};
        ALIGNED(16) double b[2] = {3.0, 6.0};
        ALIGNED(16) double c[2] = {0};
        ALIGNED(16) double expected[2] = {10.0/3.0, 20.0/6.0};
        
        CLEAR_FLAGS;
        unsigned int mxcsr_before = 0x1F80; // Default MXCSR
        __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr_before));
        
        // 在除法后立即读取 MXCSR
        unsigned int mxcsr_after_div = 0;
        
        __asm__ __volatile__(
            "vmovapd %2, %%xmm0\n\t"
            "vmovapd %3, %%xmm1\n\t"
            "vdivpd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovapd %%xmm2, %0\n\t"
            "stmxcsr %1"
            : "=m"(*c), "=m"(mxcsr_after_div)
            : "m"(*a), "m"(*b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        // 打印输入和结果
        print_double_vec("Input A", a, 2);
        print_double_vec("Input B", b, 2);
        print_double_vec("Result", c, 2);
        print_double_vec("Expected", expected, 2);
        
        // 检查结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (fabs(c[i] - expected[i]) > 1e-6) {
                printf("Mismatch at position %d: expected %.16f, got %.16f\n", 
                       i, expected[i], c[i]);
                pass = 0;
                errors++;
            }
        }
        
        // 使用除法后立即读取的 MXCSR 状态
        print_mxcsr(mxcsr_after_div);
        
        if (pass) {
            printf("[PASS] Test 7: Rounding mode (nearest)\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 7: Rounding mode (nearest)\n");
        }
        total_tests++;
    }
    
    // 测试总结
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    printf("Total errors: %d\n", errors);
    
    if (errors == 0) {
        printf("All vdivpd tests passed successfully!\n");
    } else {
        printf("Some vdivpd tests encountered errors\n");
    }
}

int main() {
    test_vdivpd();
    return 0;
}
