#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMULPD - Packed Double-FP Multiply
// Multiplies packed double-precision floating-point values

static void test_vmulpd() {
    printf("Testing VMULPD\n");
    
    // 128-bit test cases
    {
        printf("Testing VMULPD (128-bit)\n");
        struct TestCase128 {
            double a[2];
            double b[2];
            double expected[2];
        } test_cases[] = {
            {{1.5, 2.5}, {2.0, 3.0}, {3.0, 7.5}},  // 基本测试
            {{0.0, -0.0}, {1.0, -1.0}, {0.0, 0.0}}, // 零值测试
            {{INFINITY, -INFINITY}, {1.0, 1.0}, {INFINITY, -INFINITY}}, // 无穷大测试
            {{NAN, 1.0}, {1.0, NAN}, {NAN, NAN}},   // NaN处理
            {{1.7976931348623157e+308, -1.7976931348623157e+308}, 
             {2.0, 2.0}, 
             {INFINITY, -INFINITY}}  // 溢出测试
        };

        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            double result[2] = {0};
            
            __asm__ __volatile__(
                "vmovupd %1, %%xmm0\n\t"
                "vmovupd %2, %%xmm1\n\t"
                "vmulpd %%xmm1, %%xmm0, %%xmm2\n\t"
                "vmovupd %%xmm2, %0\n\t"
                : "=m"(result)
                : "m"(test_cases[i].a), "m"(test_cases[i].b)
                : "xmm0", "xmm1", "xmm2"
            );

            printf("Test case %zu (128-bit):\n", i+1);
            printf("Input A: [%.17g, %.17g]\n", 
                  test_cases[i].a[0], test_cases[i].a[1]);
            printf("Input B: [%.17g, %.17g]\n", 
                  test_cases[i].b[0], test_cases[i].b[1]);
            printf("Result: [%.17g, %.17g]\n", 
                  result[0], result[1]);
            printf("Expected: [%.17g, %.17g]\n", 
                  test_cases[i].expected[0], test_cases[i].expected[1]);

            for (int j = 0; j < 2; j++) {
                if (isnan(test_cases[i].expected[j])) {
                    if (!isnan(result[j])) {
                        printf("Mismatch at element %d: expected NaN\n", j);
                    }
                } else if (isinf(test_cases[i].expected[j]) && isinf(result[j])) {
                    if (signbit(test_cases[i].expected[j]) != signbit(result[j])) {
                        printf("Mismatch at element %d: sign differs for infinity\n", j);
                    }
                } else if (!double_equal(result[j], test_cases[i].expected[j], 0.0001)) {
                    printf("Mismatch at element %d: got %.17g, expected %.17g\n", 
                          j, result[j], test_cases[i].expected[j]);
                }
            }
            printf("\n");
        }
    }

    // 256-bit test cases
    {
        printf("Testing VMULPD (256-bit)\n");
        struct TestCase256 {
            double a[4];
            double b[4];
            double expected[4];
        } test_cases[] = {
            {{1.5, 2.5, 3.5, 4.5}, 
             {2.0, 3.0, 4.0, 5.0}, 
             {3.0, 7.5, 14.0, 22.5}},
            {{0.0, -0.0, INFINITY, -INFINITY}, 
             {1.0, -1.0, 1.0, 1.0}, 
             {0.0, 0.0, INFINITY, -INFINITY}},
            {{NAN, 1.0, 1.7976931348623157e+308, -1.7976931348623157e+308}, 
             {1.0, NAN, 2.0, 2.0}, 
             {NAN, NAN, INFINITY, -INFINITY}}
        };

        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            double result[4] = {0};
            
            __asm__ __volatile__(
                "vmovupd %1, %%ymm0\n\t"
                "vmovupd %2, %%ymm1\n\t"
                "vmulpd %%ymm1, %%ymm0, %%ymm2\n\t"
                "vmovupd %%ymm2, %0\n\t"
                : "=m"(result)
                : "m"(test_cases[i].a), "m"(test_cases[i].b)
                : "ymm0", "ymm1", "ymm2"
            );

            printf("Test case %zu (256-bit):\n", i+1);
            printf("Input A: [%.17g, %.17g, %.17g, %.17g]\n", 
                  test_cases[i].a[0], test_cases[i].a[1],
                  test_cases[i].a[2], test_cases[i].a[3]);
            printf("Input B: [%.17g, %.17g, %.17g, %.17g]\n", 
                  test_cases[i].b[0], test_cases[i].b[1],
                  test_cases[i].b[2], test_cases[i].b[3]);
            printf("Result: [%.17g, %.17g, %.17g, %.17g]\n", 
                  result[0], result[1], result[2], result[3]);
            printf("Expected: [%.17g, %.17g, %.17g, %.17g]\n", 
                  test_cases[i].expected[0], test_cases[i].expected[1],
                  test_cases[i].expected[2], test_cases[i].expected[3]);

            for (int j = 0; j < 4; j++) {
                if (isnan(test_cases[i].expected[j])) {
                    if (!isnan(result[j])) {
                        printf("Mismatch at element %d: expected NaN\n", j);
                    }
                } else if (isinf(test_cases[i].expected[j]) && isinf(result[j])) {
                    if (signbit(test_cases[i].expected[j]) != signbit(result[j])) {
                        printf("Mismatch at element %d: sign differs for infinity\n", j);
                    }
                } else if (!double_equal(result[j], test_cases[i].expected[j], 0.0001)) {
                    printf("Mismatch at element %d: got %.17g, expected %.17g\n", 
                          j, result[j], test_cases[i].expected[j]);
                }
            }
            printf("\n");
        }
    }
}

int main() {
    test_vmulpd();
    printf("VMULPD tests completed\n");
    return 0;
}
