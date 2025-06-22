#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// VMINPS - Packed Single-FP Minimum
// Compares packed single-precision floating-point values and returns the minimum values

static void test_vminps_128() {
    printf("Testing VMINPS (128-bit)\n");
    
    float a[4] = {1.5f, -2.5f, 0.0f, NAN};
    float b[4] = {-1.0f, 3.0f, -0.0f, 1.0f};
    float r[4];
    float expected[4] = {-1.0f, -2.5f, -0.0f, 1.0f};
    
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"
        "vmovups %2, %%xmm1\n\t"
        "vminps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovups %%xmm2, %0\n\t"
        : "=m"(r)
        : "m"(a), "m"(b)
        : "xmm0", "xmm1", "xmm2"
    );
    
    print_float_vec("Input A", a, 4);
    print_float_vec("Input B", b, 4);
    print_float_vec("Result", r, 4);
    print_float_vec("Expected", expected, 4);
    
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i])) {
            if (!isnan(r[i])) {
                printf("Mismatch at index %d: expected NaN, got %f\n", i, r[i]);
            }
        } else if (!float_equal(r[i], expected[i], 0.0001f)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
        }
    }
    printf("\n");
}

static void test_vminps_256() {
    printf("Testing VMINPS (256-bit)\n");
    
    float a[8] = {1.5f, -2.5f, 0.0f, NAN, INFINITY, -INFINITY, 3.0f, -4.0f};
    float b[8] = {-1.0f, 3.0f, -0.0f, 1.0f, 1.0f, -1.0f, 2.0f, -3.0f};
    float r[8];
    float expected[8] = {-1.0f, -2.5f, -0.0f, 1.0f, 1.0f, -INFINITY, 2.0f, -4.0f};
    
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vminps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovups %%ymm2, %0\n\t"
        : "=m"(r)
        : "m"(a), "m"(b)
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_float_vec("Input A", a, 8);
    print_float_vec("Input B", b, 8);
    print_float_vec("Result", r, 8);
    print_float_vec("Expected", expected, 8);
    
    for (int i = 0; i < 8; i++) {
        if (isnan(expected[i])) {
            if (!isnan(r[i])) {
                printf("Mismatch at index %d: expected NaN, got %f\n", i, r[i]);
            }
        } else if (isinf(expected[i]) && isinf(r[i])) {
            continue; // Skip inf comparison
        } else if (!float_equal(r[i], expected[i], 0.0001f)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
        }
    }
    printf("\n");
}

int main() {
    test_vminps_128();
    test_vminps_256();
    printf("VMINPS tests completed\n");
    return 0;
}
