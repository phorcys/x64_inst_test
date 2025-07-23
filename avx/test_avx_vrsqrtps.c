#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

// 定义测试用例结构
typedef struct {
    const char* desc;
    int width;
    float input[8];
    float expected[8];
} TestCase;

// VRSQRTPS测试函数
static void run_vrsqrtps_test(const TestCase* test) {
    printf("\n--- Testing %s (%d-bit) ---\n", test->desc, test->width);
    
    float result[8] = {0};
    int elem_count = (test->width == 128) ? 4 : 8;

    // 寄存器操作数测试
    if (test->width == 128) {
        __asm__ __volatile__(
            "vmovups %1, %%xmm0\n\t"
            "vrsqrtps %%xmm0, %%xmm1\n\t"
            "vmovups %%xmm1, %0\n\t"
            : "=m"(result)
            : "m"(test->input)
            : "xmm0", "xmm1"
        );
    } else {
        __asm__ __volatile__(
            "vmovups %1, %%ymm0\n\t"
            "vrsqrtps %%ymm0, %%ymm1\n\t"
            "vmovups %%ymm1, %0\n\t"
            : "=m"(result)
            : "m"(test->input)
            : "ymm0", "ymm1"
        );
    }
    
    // 内存操作数测试
    float mem_result[8] = {0};
    if (test->width == 128) {
        __asm__ __volatile__(
            "vrsqrtps %1, %%xmm0\n\t"
            "vmovups %%xmm0, %0\n\t"
            : "=m"(mem_result)
            : "m"(test->input)
            : "xmm0"
        );
    } else {
        __asm__ __volatile__(
            "vrsqrtps %1, %%ymm0\n\t"
            "vmovups %%ymm0, %0\n\t"
            : "=m"(mem_result)
            : "m"(test->input)
            : "ymm0"
        );
    }

    // 结果验证
    for (int i = 0; i < elem_count; i++) {
        printf("\nTest case %d: %s\n", i+1, test->desc);
        printf("Input: %.9g\n", test->input[i]);
        
        // 寄存器结果验证
        printf("Register result: %.9g\n", result[i]);
        printf("Expected: %.9g\n", test->expected[i]);
        // 验证寄存器结果
        if (isnan(test->expected[i])) {
            if (!isnan(result[i])) {
                printf("[ERROR] Register: expected NaN\n");
            } else {
                printf("[OK] Register: NaN as expected\n");
            }
        } else if (isinf(test->expected[i])) {
            if (!isinf(result[i]) || signbit(test->expected[i]) != signbit(result[i])) {
                printf("[ERROR] Register: sign mismatch\n");
            } else {
                printf("[OK] Register: infinity with correct sign\n");
            }
        } else {
            // 处理预期值为0的特殊情况
            if (test->expected[i] == 0.0f) {
                float abs_error = fabsf(result[i] - test->expected[i]);
                if (abs_error > 1e-6f) {
                    printf("[WARNING] Register: abs error %.9g, got %.9g, expected 0\n", 
                          abs_error, result[i]);
                } else {
                    printf("[OK] Register: within tolerance (abs error %.9g)\n", abs_error);
                }
            } else {
                // 计算相对误差
                float rel_error = fabsf((result[i] - test->expected[i]) / test->expected[i]);
                if (rel_error > 0.0015f) { // 容忍0.15%误差
                    printf("[WARNING] Register: rel error %.4f%%, got %.9g, expected %.9g\n", 
                          rel_error*100, result[i], test->expected[i]);
                } else {
                    printf("[OK] Register: within tolerance (rel error %.4f%%)\n", rel_error*100);
                }
            }
        }
        
        // 额外检查：负数输入应返回NaN
        if (signbit(test->input[i]) && test->input[i] < 0.0f && !isnan(result[i])) {
            printf("[WARNING] Register: negative input should produce NaN\n");
        }
        
        // 验证内存结果
        printf("Memory result: %.9g\n", mem_result[i]);
        if (isnan(test->expected[i])) {
            if (!isnan(mem_result[i])) {
                printf("[ERROR] Memory: expected NaN\n");
            } else {
                printf("[OK] Memory: NaN as expected\n");
            }
        } else if (isinf(test->expected[i])) {
            if (!isinf(mem_result[i]) || signbit(test->expected[i]) != signbit(mem_result[i])) {
                printf("[ERROR] Memory: sign mismatch\n");
            } else {
                printf("[OK] Memory: infinity with correct sign\n");
            }
        } else {
            // 处理预期值为0的特殊情况
            if (test->expected[i] == 0.0f) {
                float abs_error = fabsf(mem_result[i] - test->expected[i]);
                if (abs_error > 1e-6f) {
                    printf("[WARNING] Memory: abs error %.9g, got %.9g, expected 0\n", 
                          abs_error, mem_result[i]);
                } else {
                    printf("[OK] Memory: within tolerance (abs error %.9g)\n", abs_error);
                }
            } else {
                float rel_error = fabsf((mem_result[i] - test->expected[i]) / test->expected[i]);
                if (rel_error > 0.0015f) {
                    printf("[WARNING] Memory: rel error %.4f%%, got %.9g, expected %.9g\n", 
                          rel_error*100, mem_result[i], test->expected[i]);
                } else {
                    printf("[OK] Memory: within tolerance (rel error %.4f%%)\n", rel_error*100);
                }
            }
        }
        
        // 额外检查：负数输入应返回NaN
        if (signbit(test->input[i]) && test->input[i] < 0.0f && !isnan(mem_result[i])) {
            printf("[WARNING] Memory: negative input should produce NaN\n");
        }
    }
}

int main() {
    printf("Starting VRSQRTPS tests...\n");
    
    // 预定义测试用例数组
    TestCase tests[] = {
        // 128位测试
        {
            "Basic values",
            128,
            {1.0f, 4.0f, 16.0f, 64.0f},
            {1.0f/sqrtf(1.0f), 1.0f/sqrtf(4.0f), 1.0f/sqrtf(16.0f), 1.0f/sqrtf(64.0f)}
        },
        {
            "Zero values",
            128,
            {+0.0f, -0.0f, FLT_MIN, -FLT_MIN},
            {INFINITY, -INFINITY, 1.0f/sqrtf(FLT_MIN), NAN}
        },
        {
            "Special values",
            128,
            {INFINITY, -INFINITY, NAN, -NAN},
            {0.0f, NAN, NAN, NAN}
        },
        
        // 256位测试
        {
            "Mixed values",
            256,
            {1.0f, 4.0f, 0.25f, 100.0f, +0.0f, -1.0f, INFINITY, NAN},
            {1.0f/sqrtf(1.0f), 1.0f/sqrtf(4.0f), 1.0f/sqrtf(0.25f), 
             1.0f/sqrtf(100.0f), 1.0f/sqrtf(+0.0f), NAN, 0.0f, NAN}
        },
        {
            "Boundary values",
            256,
            {FLT_MAX, -FLT_MAX, FLT_TRUE_MIN, 1e-30f, -1e-30f, 1e30f, -1e30f, 0.0f},  // 移除问题输入
            {1.0f/sqrtf(FLT_MAX), NAN, INFINITY, 
             1.0f/sqrtf(1e-30f), NAN, 1.0f/sqrtf(1e30f), NAN, INFINITY}
        },
        {
            "Known precision issues",
            128,
            {1e-30f, 1e-20f, 1e-10f, 1e10f},
            {1.0f/sqrtf(1e-30f), 1.0f/sqrtf(1e-20f), 1.0f/sqrtf(1e-10f), 1.0f/sqrtf(1e10f)}
        }
    };
    
    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        run_vrsqrtps_test(&tests[i]);
    }
    
    printf("\nVRSQRTPS tests completed\n");
    return 0;
}
