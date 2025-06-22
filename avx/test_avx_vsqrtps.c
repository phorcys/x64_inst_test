#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSQRTPS - Compute Square Roots of Packed Single-Precision Floating-Point Values
// Computes square roots of packed single-precision floating-point values

static void test_vsqrtps() {
    printf("Testing VSQRTPS\n");
    
    // 测试128位和256位版本
    float input128[4] = {1.0f, 4.0f, 16.0f, 64.0f};
    float expected128[4] = {sqrtf(1.0f), sqrtf(4.0f), sqrtf(16.0f), sqrtf(64.0f)};
    float result128[4] = {0};
    
    float input256[8] = {1.0f, 4.0f, 16.0f, 64.0f, 0.25f, -0.25f, INFINITY, NAN};
    float expected256[8] = {sqrtf(1.0f), sqrtf(4.0f), sqrtf(16.0f), sqrtf(64.0f),
                          sqrtf(0.25f), NAN, INFINITY, NAN};
    float result256[8] = {0};

    // 测试128位版本
    printf("Testing VSQRTPS (128-bit)\n");
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"
        "vsqrtps %%xmm0, %%xmm1\n\t"
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
        } else if (isinf(expected128[i])) {
            if (!isinf(result128[i]) || signbit(expected128[i]) != signbit(result128[i])) {
                printf("Mismatch: expected %s\n", expected128[i] > 0 ? "+INF" : "-INF");
            }
        } else if (fabsf(result128[i] - expected128[i]) > 0.0001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result128[i], expected128[i]);
        }
        printf("\n");
    }

    // 测试256位版本
    printf("Testing VSQRTPS (256-bit)\n");
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vsqrtps %%ymm0, %%ymm1\n\t"
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
        } else if (isinf(expected256[i])) {
            if (!isinf(result256[i]) || signbit(expected256[i]) != signbit(result256[i])) {
                printf("Mismatch: expected %s\n", expected256[i] > 0 ? "+INF" : "-INF");
            }
        } else if (fabsf(result256[i] - expected256[i]) > 0.0001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result256[i], expected256[i]);
        }
        printf("\n");
    }
}

int main() {
    test_vsqrtps();
    printf("VSQRTPS tests completed\n");
    return 0;
}
