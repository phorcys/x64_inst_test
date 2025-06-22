#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Helper function to create SNaN (signaling NaN) for float
static float create_snan_f() {
    uint32_t snan = 0x7F800001;
    return *(float*)&snan;
}

// Compare two floats including NaN bit patterns
static int float_equal_bits(float a, float b) {
    uint32_t a_bits = *(uint32_t*)&a;
    uint32_t b_bits = *(uint32_t*)&b;
    return a_bits == b_bits;
}

// VHADDPS - Horizontal Add Packed Single-Precision Floating-Point Values
// Performs horizontal addition on adjacent pairs of single-precision floating-point values

static void test_vhaddps(int width) {
    const char* bit_str = width == 128 ? "128-bit" : "256-bit";
    int elements = width == 128 ? 4 : 8;
    printf("Testing VHADDPS (%s)\n", bit_str);
    
    // Test cases
    struct {
        const char* desc;
        float a[8];
        float b[8];
        float expected[8];
        int check_bits[8]; // 1 = check bit pattern, 0 = check value
    } tests[] = {
        // Basic cases
        {
            "Basic values",
            {1.0f, 2.0f, 3.0f, 4.0f},
            {5.0f, 6.0f, 7.0f, 8.0f},
            {1.0f+2.0f, 3.0f+4.0f, 5.0f+6.0f, 7.0f+8.0f},
            {0, 0, 0, 0, 0, 0, 0, 0}
        },
        // Zero cases
        {
            "Mixed zeros",
            {0.0f, -0.0f, 0.0f, -0.0f},
            {0.0f, 0.0f, -0.0f, -0.0f},
            {0.0f, 0.0f, 0.0f, -0.0f},
            {1, 1, 1, 1, 0, 0, 0, 0} // Check bit patterns for zeros
        },
        // NaN cases
        {
            "NaN values",
            {NAN, 1.0f, NAN, 2.0f},
            {3.0f, NAN, 4.0f, NAN},
            {NAN, NAN, NAN, NAN},
            {0, 0, 0, 0, 0, 0, 0, 0}
        },
        // SNaN cases
        {
            "SNaN values",
            {1.0f, create_snan_f(), 2.0f, 3.0f},
            {4.0f, 5.0f, create_snan_f(), 6.0f},
            {1.0f+create_snan_f(), 2.0f+3.0f, 4.0f+5.0f, create_snan_f()+6.0f},
            {0, 0, 0, 0, 0, 0, 0, 0} // Results will be QNaN or SNaN? (implementation specific)
        },
        // Infinity cases
        {
            "Infinity values",
            {INFINITY, 1.0f, -INFINITY, 2.0f},
            {3.0f, INFINITY, 4.0f, -INFINITY},
            {INFINITY, -INFINITY, INFINITY, -INFINITY},
            {0, 0, 0, 0, 0, 0, 0, 0}
        },
        // Denormal cases
        {
            "Denormal values",
            {1.0e-40f, 2.0e-40f, 3.0e-40f, 4.0e-40f},
            {5.0e-40f, 6.0e-40f, 7.0e-40f, 8.0e-40f},
            {3.0e-40f, 7.0e-40f, 11.0e-40f, 15.0e-40f},
            {0, 0, 0, 0, 0, 0, 0, 0}
        },
        // Overflow cases
        {
            "Overflow values",
            {1.0e38f, 1.0e38f, -1.0e38f, -1.0e38f},
            {1.0e38f, 1.0e38f, -1.0e38f, -1.0e38f},
            {2.0e38f, -2.0e38f, 2.0e38f, -2.0e38f},
            {0, 0, 0, 0, 0, 0, 0, 0}
        }
    };

    int num_tests = sizeof(tests)/sizeof(tests[0]);
    
    for (int t = 0; t < num_tests; t++) {
        printf("\nTest %d: %s\n", t+1, tests[t].desc);
        
        float a[8] = {0};
        float b[8] = {0};
        float r[8] = {0};
        float expected[8] = {0};
        
        // Copy test data
        for (int i = 0; i < elements; i++) {
            a[i] = tests[t].a[i];
            b[i] = tests[t].b[i];
            expected[i] = tests[t].expected[i];
        }
        
        // Execute instruction
        if (width == 128) {
            __asm__ __volatile__(
                "vmovups %1, %%xmm0\n\t"
                "vmovups %2, %%xmm1\n\t"
                "vhaddps %%xmm1, %%xmm0, %%xmm2\n\t"
                "vmovups %%xmm2, %0\n\t"
                : "=m"(r)
                : "m"(a), "m"(b)
                : "xmm0", "xmm1", "xmm2"
            );
        } else {
            __asm__ __volatile__(
                "vmovups %1, %%ymm0\n\t"
                "vmovups %2, %%ymm1\n\t"
                "vhaddps %%ymm1, %%ymm0, %%ymm2\n\t"
                "vmovups %%ymm2, %0\n\t"
                : "=m"(r)
                : "m"(a), "m"(b)
                : "ymm0", "ymm1", "ymm2"
            );
        }
        
        print_float_vec("Input A", a, elements);
        print_float_vec("Input B", b, elements);
        print_float_vec("Result", r, elements);
        print_float_vec("Expected", expected, elements);
        
        int all_pass = 1;
        for (int i = 0; i < elements; i++) {
            if (tests[t].check_bits[i]) {
                if (!float_equal_bits(r[i], expected[i])) {
                    printf("Mismatch at index %d: bit patterns differ\n", i);
                    all_pass = 0;
                }
            } else if (isnan(expected[i])) {
                if (!isnan(r[i])) {
                    printf("Mismatch at index %d: expected NaN, got %f\n", i, r[i]);
                    all_pass = 0;
                }
            } else if (isinf(expected[i])) {
                if (!isinf(r[i]) || (signbit(expected[i]) != signbit(r[i]))) {
                    printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
                    all_pass = 0;
                }
            } else {
                if (fabsf(r[i] - expected[i]) > fabsf(expected[i] * 0.0001f)) {
                    printf("Mismatch at index %d: got %f, expected %f (error: %e)\n", 
                          i, r[i], expected[i], fabsf(r[i] - expected[i]));
                    all_pass = 0;
                }
            }
        }
        
        if (all_pass) {
            printf("All elements passed\n");
        }
    }
    printf("\n");
}

int main() {
    test_vhaddps(128);  // Test 128-bit version
    test_vhaddps(256);  // Test 256-bit version
    printf("VHADDPS tests completed\n");
    return 0;
}
