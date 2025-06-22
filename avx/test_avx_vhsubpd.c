#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Helper function to create SNaN (signaling NaN) for double
static double create_snan_d() {
    uint64_t snan = 0x7FF0000000000001ULL;
    return *(double*)&snan;
}

// Compare two doubles including NaN bit patterns
static int double_equal_bits(double a, double b) {
    uint64_t a_bits = *(uint64_t*)&a;
    uint64_t b_bits = *(uint64_t*)&b;
    return a_bits == b_bits;
}

// VHSUBPD - Packed Double-FP Horizontal Subtract
// Performs horizontal subtraction on adjacent pairs of double-precision floating-point values

static void test_vhsubpd(int width) {
    const char* bit_str = width == 128 ? "128-bit" : "256-bit";
    int elements = width == 128 ? 2 : 4;
    printf("Testing VHSUBPD (%s)\n", bit_str);
    
    // Define test scenarios
    struct {
        const char* desc;
        double a[4];
        double expected[4];
        int check_bits[4]; // 1 = check bit pattern, 0 = check value
    } tests[] = {
        // Basic cases
        {
            "Basic values",
            {1.0, 2.0, 3.0, 4.0},
            {0}, // Will be calculated dynamically
            {0, 0, 0, 0}
        },
        // Zero cases
        {
            "Mixed zeros",
            {0.0, -0.0, 0.0, -0.0},
            {0.0 - (-0.0), 0.0 - (-0.0), 0.0, 0.0},
            {1, 1, 0, 0}  // Check bit patterns for zeros
        },
        // NaN cases
        {
            "NaN values",
            {NAN, 1.0, NAN, 2.0},
            {NAN, NAN, 0.0, 0.0},
            {0, 0, 0, 0}
        },
        // SNaN cases
        {
            "SNaN values",
            {1.0, create_snan_d(), 2.0, 3.0},
            {0}, // Will be calculated dynamically
            {0, 0, 0, 0}  // Results will be QNaN or SNaN? (implementation specific)
        },
        // Infinity cases
        {
            "Infinity values",
            {INFINITY, 1.0, -INFINITY, 2.0},
            {INFINITY - 1.0, INFINITY - 1.0, -INFINITY - 2.0, -INFINITY - 2.0},
            {0, 0, 0, 0}
        },
        // Denormal cases
        {
            "Denormal values",
            {1.0e-320, 2.0e-320, 3.0e-320, 4.0e-320},
            {1.0e-320 - 2.0e-320, 3.0e-320 - 4.0e-320, 0.0, 0.0},
            {0, 0, 0, 0}
        },
        // Overflow cases
        {
            "Overflow values",
            {1.0e308, -1.0e308, 1.0e308, -1.0e308},
            {1.0e308 - (-1.0e308), 1.0e308 - (-1.0e308), 1.0e308 - (-1.0e308), 1.0e308 - (-1.0e308)},
            {0, 0, 0, 0}
        }
    };

    int num_tests = sizeof(tests)/sizeof(tests[0]);
    
    for (int t = 0; t < num_tests; t++) {
        printf("\nTest %d: %s\n", t+1, tests[t].desc);
        
        double a[4] = {0};
        double r[4] = {0};
        double expected[4] = {0};
        
        // Copy test data
        for (int i = 0; i < elements; i++) {
            a[i] = tests[t].a[i];
        }
        
        // Calculate expected results based on instruction behavior
        if (width == 128) {
            // 128-bit: horizontal subtraction on first two elements
            expected[0] = a[0] - a[1];
            expected[1] = a[0] - a[1];  // repeated for both positions
            expected[2] = 0.0;
            expected[3] = 0.0;
        } else {
            // 256-bit: horizontal subtraction on each 128-bit segment
            expected[0] = a[0] - a[1];
            expected[1] = a[0] - a[1];  // repeated in first segment
            expected[2] = a[2] - a[3];
            expected[3] = a[2] - a[3];  // repeated in second segment
        }
        
        // Execute instruction
        if (width == 128) {
            __asm__ __volatile__(
                "vmovupd %1, %%xmm0\n\t"
                "vhsubpd %%xmm0, %%xmm0, %%xmm1\n\t"  // Horizontal subtract: xmm1[0] = xmm0[0] - xmm0[1], xmm1[1] = xmm0[0] - xmm0[1]
                "vmovupd %%xmm1, %0\n\t"
                : "=m"(r)
                : "m"(a)
                : "xmm0", "xmm1"
            );
        } else {
            __asm__ __volatile__(
                "vmovupd %1, %%ymm0\n\t"
                "vhsubpd %%ymm0, %%ymm0, %%ymm1\n\t"  // Horizontal subtract: 
                // ymm1[0] = ymm0[0] - ymm0[1]
                // ymm1[1] = ymm0[0] - ymm0[1]
                // ymm1[2] = ymm0[2] - ymm0[3]
                // ymm1[3] = ymm0[2] - ymm0[3]
                "vmovupd %%ymm1, %0\n\t"
                : "=m"(r)
                : "m"(a)
                : "ymm0", "ymm1"
            );
        }
        
        print_double_vec("Input", a, elements);
        print_double_vec("Result", r, elements);
        print_double_vec("Expected", expected, elements);
        
        int all_pass = 1;
        for (int i = 0; i < elements; i++) {
            if (tests[t].check_bits[i]) {
                if (!double_equal_bits(r[i], expected[i])) {
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
                if (fabs(r[i] - expected[i]) > fabs(expected[i] * 0.0001)) {
                    printf("Mismatch at index %d: got %f, expected %f (error: %e)\n", 
                          i, r[i], expected[i], fabs(r[i] - expected[i]));
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
    test_vhsubpd(128);  // Test 128-bit version
    test_vhsubpd(256);  // Test 256-bit version
    printf("VHSUBPD tests completed\n");
    return 0;
}
