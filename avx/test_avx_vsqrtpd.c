#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSQRTPD - Compute Square Roots of Packed Double-Precision Floating-Point Values
// Computes square roots of packed double-precision floating-point values

static void test_vsqrtpd() {
    printf("Testing VSQRTPD\n");
    
    // 测试128位和256位版本
    double input128[2] = {1.0, 4.0};
    double expected128[2] = {sqrt(1.0), sqrt(4.0)};
    double result128[2] = {0};
    
    double input256[4] = {1.0, 4.0, 16.0, 64.0};
    double expected256[4] = {sqrt(1.0), sqrt(4.0), sqrt(16.0), sqrt(64.0)};
    double result256[4] = {0};

    // 测试128位版本
    printf("Testing VSQRTPD (128-bit)\n");
    __asm__ __volatile__(
        "vmovupd %1, %%xmm0\n\t"
        "vsqrtpd %%xmm0, %%xmm1\n\t"
        "vmovupd %%xmm1, %0\n\t"
        : "=m"(result128)
        : "m"(input128)
        : "xmm0", "xmm1"
    );

    for (int i = 0; i < 2; i++) {
        printf("Test case %d (128-bit):\n", i+1);
        printf("Input: %.17g\n", input128[i]);
        printf("Result: %.17g\n", result128[i]);
        printf("Expected: %.17g\n", expected128[i]);
        
        if (fabs(result128[i] - expected128[i]) > 0.0000001) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result128[i], expected128[i]);
        }
        printf("\n");
    }

    // 测试256位版本
    printf("Testing VSQRTPD (256-bit)\n");
    __asm__ __volatile__(
        "vmovupd %1, %%ymm0\n\t"
        "vsqrtpd %%ymm0, %%ymm1\n\t"
        "vmovupd %%ymm1, %0\n\t"
        : "=m"(result256)
        : "m"(input256)
        : "ymm0", "ymm1"
    );

    for (int i = 0; i < 4; i++) {
        printf("Test case %d (256-bit):\n", i+1);
        printf("Input: %.17g\n", input256[i]);
        printf("Result: %.17g\n", result256[i]);
        printf("Expected: %.17g\n", expected256[i]);
        
        if (fabs(result256[i] - expected256[i]) > 0.0000001) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result256[i], expected256[i]);
        }
        printf("\n");
    }
}

int main() {
    test_vsqrtpd();
    printf("VSQRTPD tests completed\n");
    return 0;
}
