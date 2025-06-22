#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSUBPD - Subtract Packed Double-Precision Floating-Point Values
// Performs subtraction on packed double-precision floating-point values

static void test_vsubpd() {
    printf("Testing VSUBPD\n");
    
    // 测试128位和256位版本
    double input1_128[2] = {1.0, 4.0};
    double input2_128[2] = {0.5, 2.0};
    double expected_128[2] = {0.5, 2.0};
    double result_128[2] = {0};
    
    double input1_256[4] = {1.0, 4.0, 16.0, 64.0};
    double input2_256[4] = {0.5, 2.0, 4.0, 8.0};
    double expected_256[4] = {0.5, 2.0, 12.0, 56.0};
    double result_256[4] = {0};

    // 测试128位版本
    printf("Testing VSUBPD (128-bit)\n");
    __asm__ __volatile__(
        "vmovupd %1, %%xmm0\n\t"
        "vmovupd %2, %%xmm1\n\t"
        "vsubpd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovupd %%xmm2, %0\n\t"
        : "=m"(result_128)
        : "m"(input1_128), "m"(input2_128)
        : "xmm0", "xmm1", "xmm2"
    );

    for (int i = 0; i < 2; i++) {
        printf("Test case %d (128-bit):\n", i+1);
        printf("Input1: %.17g\n", input1_128[i]);
        printf("Input2: %.17g\n", input2_128[i]);
        printf("Result: %.17g\n", result_128[i]);
        printf("Expected: %.17g\n", expected_128[i]);
        
        if (fabs(result_128[i] - expected_128[i]) > 0.0000001) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result_128[i], expected_128[i]);
        }
        printf("\n");
    }

    // 测试256位版本
    printf("Testing VSUBPD (256-bit)\n");
    __asm__ __volatile__(
        "vmovupd %1, %%ymm0\n\t"
        "vmovupd %2, %%ymm1\n\t"
        "vsubpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovupd %%ymm2, %0\n\t"
        : "=m"(result_256)
        : "m"(input1_256), "m"(input2_256)
        : "ymm0", "ymm1", "ymm2"
    );

    for (int i = 0; i < 4; i++) {
        printf("Test case %d (256-bit):\n", i+1);
        printf("Input1: %.17g\n", input1_256[i]);
        printf("Input2: %.17g\n", input2_256[i]);
        printf("Result: %.17g\n", result_256[i]);
        printf("Expected: %.17g\n", expected_256[i]);
        
        if (fabs(result_256[i] - expected_256[i]) > 0.0000001) {
            printf("Mismatch: got %.17g, expected %.17g\n", 
                  result_256[i], expected_256[i]);
        }
        printf("\n");
    }
}

int main() {
    test_vsubpd();
    printf("VSUBPD tests completed\n");
    return 0;
}
