#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VSUBPS - Subtract Packed Single-Precision Floating-Point Values
// Performs subtraction on packed single-precision floating-point values

static void test_vsubps() {
    printf("Testing VSUBPS\n");
    
    // 测试128位和256位版本
    float input1_128[4] = {1.0f, 4.0f, 16.0f, 64.0f};
    float input2_128[4] = {0.5f, 2.0f, 4.0f, 8.0f};
    float expected_128[4] = {0.5f, 2.0f, 12.0f, 56.0f};
    float result_128[4] = {0};
    
    float input1_256[8] = {1.0f, 4.0f, 16.0f, 64.0f, 256.0f, 1024.0f, 4096.0f, 16384.0f};
    float input2_256[8] = {0.5f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f};
    float expected_256[8] = {0.5f, 2.0f, 12.0f, 56.0f, 240.0f, 992.0f, 4032.0f, 16256.0f};
    float result_256[8] = {0};

    // 测试128位版本
    printf("Testing VSUBPS (128-bit)\n");
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"
        "vmovups %2, %%xmm1\n\t"
        "vsubps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovups %%xmm2, %0\n\t"
        : "=m"(result_128)
        : "m"(input1_128), "m"(input2_128)
        : "xmm0", "xmm1", "xmm2"
    );

    for (int i = 0; i < 4; i++) {
        printf("Test case %d (128-bit):\n", i+1);
        printf("Input1: %.9g\n", input1_128[i]);
        printf("Input2: %.9g\n", input2_128[i]);
        printf("Result: %.9g\n", result_128[i]);
        printf("Expected: %.9g\n", expected_128[i]);
        
        if (fabsf(result_128[i] - expected_128[i]) > 0.0001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result_128[i], expected_128[i]);
        }
        printf("\n");
    }

    // 测试256位版本
    printf("Testing VSUBPS (256-bit)\n");
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vsubps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovups %%ymm2, %0\n\t"
        : "=m"(result_256)
        : "m"(input1_256), "m"(input2_256)
        : "ymm0", "ymm1", "ymm2"
    );

    for (int i = 0; i < 8; i++) {
        printf("Test case %d (256-bit):\n", i+1);
        printf("Input1: %.9g\n", input1_256[i]);
        printf("Input2: %.9g\n", input2_256[i]);
        printf("Result: %.9g\n", result_256[i]);
        printf("Expected: %.9g\n", expected_256[i]);
        
        if (fabsf(result_256[i] - expected_256[i]) > 0.0001f) {
            printf("Mismatch: got %.9g, expected %.9g\n", 
                  result_256[i], expected_256[i]);
        }
        printf("\n");
    }
}

int main() {
    test_vsubps();
    printf("VSUBPS tests completed\n");
    return 0;
}
