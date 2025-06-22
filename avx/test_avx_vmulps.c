#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMULPS - Packed Single-FP Multiply
// Multiplies packed single-precision floating-point values

static void test_vmulps() {
    printf("Testing VMULPS\n");
    
    // 128-bit test cases
    {
        printf("Testing VMULPS (128-bit)\n");
        struct TestCase128 {
            float a[4];
            float b[4];
            float expected[4];
        } test_cases[] = {
            {{1.5f, 2.5f, 3.5f, 4.5f}, 
             {2.0f, 3.0f, 4.0f, 5.0f}, 
             {3.0f, 7.5f, 14.0f, 22.5f}},  // 基本测试
            {{0.0f, -0.0f, INFINITY, -INFINITY}, 
             {1.0f, -1.0f, 1.0f, 1.0f}, 
             {0.0f, 0.0f, INFINITY, -INFINITY}}, // 边界测试
            {{NAN, 1.0f, 3.402823466e+38f, -3.402823466e+38f},
             {1.0f, NAN, 2.0f, 2.0f},
             {NAN, NAN, INFINITY, -INFINITY}}  // 特殊值测试
        };

        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            float result[4] = {0};
            
            __asm__ __volatile__(
                "vmovups %1, %%xmm0\n\t"
                "vmovups %2, %%xmm1\n\t"
                "vmulps %%xmm1, %%xmm0, %%xmm2\n\t"
                "vmovups %%xmm2, %0\n\t"
                : "=m"(result)
                : "m"(test_cases[i].a), "m"(test_cases[i].b)
                : "xmm0", "xmm1", "xmm2"
            );

            printf("Test case %zu (128-bit):\n", i+1);
            printf("Input A: [%.9g, %.9g, %.9g, %.9g]\n", 
                  test_cases[i].a[0], test_cases[i].a[1],
                  test_cases[i].a[2], test_cases[i].a[3]);
            printf("Input B: [%.9g, %.9g, %.9g, %.9g]\n", 
                  test_cases[i].b[0], test_cases[i].b[1],
                  test_cases[i].b[2], test_cases[i].b[3]);
            printf("Result: [%.9g, %.9g, %.9g, %.9g]\n", 
                  result[0], result[1], result[2], result[3]);
            printf("Expected: [%.9g, %.9g, %.9g, %.9g]\n", 
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
                } else if (!float_equal(result[j], test_cases[i].expected[j], 0.0001f)) {
                    printf("Mismatch at element %d: got %.9g, expected %.9g\n", 
                          j, result[j], test_cases[i].expected[j]);
                }
            }
            printf("\n");
        }
    }

    // 256-bit test cases
    {
        printf("Testing VMULPS (256-bit)\n");
        struct TestCase256 {
            float a[8];
            float b[8];
            float expected[8];
        } test_cases[] = {
            {{1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f, 8.5f},
             {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f},
             {3.0f, 7.5f, 14.0f, 22.5f, 33.0f, 45.5f, 60.0f, 76.5f}},
            {{0.0f, -0.0f, INFINITY, -INFINITY, NAN, 1.0f, 3.402823466e+38f, -3.402823466e+38f},
             {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, NAN, 2.0f, 2.0f},
             {0.0f, 0.0f, INFINITY, -INFINITY, NAN, NAN, INFINITY, -INFINITY}}
        };

        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            float result[8] = {0};
            
            __asm__ __volatile__(
                "vmovups %1, %%ymm0\n\t"
                "vmovups %2, %%ymm1\n\t"
                "vmulps %%ymm1, %%ymm0, %%ymm2\n\t"
                "vmovups %%ymm2, %0\n\t"
                : "=m"(result)
                : "m"(test_cases[i].a), "m"(test_cases[i].b)
                : "ymm0", "ymm1", "ymm2"
            );

            printf("Test case %zu (256-bit):\n", i+1);
            printf("Input A: [%.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g]\n", 
                  test_cases[i].a[0], test_cases[i].a[1],
                  test_cases[i].a[2], test_cases[i].a[3],
                  test_cases[i].a[4], test_cases[i].a[5],
                  test_cases[i].a[6], test_cases[i].a[7]);
            printf("Input B: [%.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g]\n", 
                  test_cases[i].b[0], test_cases[i].b[1],
                  test_cases[i].b[2], test_cases[i].b[3],
                  test_cases[i].b[4], test_cases[i].b[5],
                  test_cases[i].b[6], test_cases[i].b[7]);
            printf("Result: [%.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g]\n", 
                  result[0], result[1], result[2], result[3],
                  result[4], result[5], result[6], result[7]);
            printf("Expected: [%.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g, %.9g]\n", 
                  test_cases[i].expected[0], test_cases[i].expected[1],
                  test_cases[i].expected[2], test_cases[i].expected[3],
                  test_cases[i].expected[4], test_cases[i].expected[5],
                  test_cases[i].expected[6], test_cases[i].expected[7]);

            for (int j = 0; j < 8; j++) {
                if (isnan(test_cases[i].expected[j])) {
                    if (!isnan(result[j])) {
                        printf("Mismatch at element %d: expected NaN\n", j);
                    }
                } else if (isinf(test_cases[i].expected[j]) && isinf(result[j])) {
                    if (signbit(test_cases[i].expected[j]) != signbit(result[j])) {
                        printf("Mismatch at element %d: sign differs for infinity\n", j);
                    }
                } else if (!float_equal(result[j], test_cases[i].expected[j], 0.0001f)) {
                    printf("Mismatch at element %d: got %.9g, expected %.9g\n", 
                          j, result[j], test_cases[i].expected[j]);
                }
            }
            printf("\n");
        }
    }
}

int main() {
    test_vmulps();
    printf("VMULPS tests completed\n");
    return 0;
}
