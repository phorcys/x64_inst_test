#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VRSQRTPS - Compute Reciprocals of Square Roots of Packed Single-Precision Floating-Point Values
// Computes approximate reciprocals of square roots (1/√x) of packed single-precision floating-point values

static void test_vrsqrtps() {
    printf("Testing VRSQRTPS\n");
    
    // 测试128位和256位版本
    float input128[4] = {1.0f, 4.0f, 16.0f, 64.0f};
    float expected128[4] = {1.0f/sqrtf(1.0f), 1.0f/sqrtf(4.0f), 1.0f/sqrtf(16.0f), 1.0f/sqrtf(64.0f)};
    float result128[4] = {0};
    
    float input256[8] = {1.0f, 4.0f, 16.0f, 64.0f, 0.25f, -0.25f, INFINITY, NAN};
    float expected256[8] = {1.0f/sqrtf(1.0f), 1.0f/sqrtf(4.0f), 1.0f/sqrtf(16.0f), 1.0f/sqrtf(64.0f),
                          1.0f/sqrtf(0.25f), NAN, 0.0f, NAN};
    float result256[8] = {0};

    // 测试128位版本
    printf("Testing VRSQRTPS (128-bit)\n");
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"
        "vrsqrtps %%xmm0, %%xmm1\n\t"
        "vmovups %%xmm1, %0\n\t"
        : "=m"(result128)
        : "m"(input128)
        : "xmm0", "xmm1"
    );

    for (int i = 0; i < 4; i++) {
        printf("Test case %d (128-bit):\n", i+1);
        printf("Input: %.9g\n", input128[i]);
        printf("Result: %.9g\n", result128[i]);
        printf("Expected: %.9g\n", expected128[i]);
        
        if (isnan(expected128[i])) {
            if (!isnan(result128[i])) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (fabsf(result128[i] - expected128[i]) > 0.001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result128[i], expected128[i]);
        }
        printf("\n");
    }

    // 测试256位版本
    printf("Testing VRSQRTPS (256-bit)\n");
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vrsqrtps %%ymm0, %%ymm1\n\t"
        "vmovups %%ymm1, %0\n\t"
        : "=m"(result256)
        : "m"(input256)
        : "ymm0", "ymm1"
    );

    for (int i = 0; i < 8; i++) {
        printf("Test case %d (256-bit):\n", i+1);
        printf("Input: %.9g\n", input256[i]);
        printf("Result: %.9g\n", result256[i]);
        printf("Expected: %.9g\n", expected256[i]);
        
        if (isnan(expected256[i])) {
            if (!isnan(result256[i])) {
                printf("Mismatch: expected NaN\n");
            }
        } else if (isinf(expected256[i]) && isinf(result256[i])) {
            if (signbit(expected256[i]) != signbit(result256[i])) {
                printf("Mismatch: sign differs for infinity\n");
            }
        } else if (fabsf(result256[i] - expected256[i]) > 0.001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result256[i], expected256[i]);
        }
        printf("\n");
    }
}

int main() {
    test_vrsqrtps();
    printf("VRSQRTPS tests completed\n");
    return 0;
}
