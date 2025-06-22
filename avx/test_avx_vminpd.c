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

// VMINPD - Packed Double-FP Minimum
// Compares packed double-precision floating-point values and returns the minimum values

static void test_vminpd(int width) {
    const char* bit_str = width == 128 ? "128-bit" : "256-bit";
    int elements = width == 128 ? 2 : 4;
    printf("Testing VMINPD (%s)\n", bit_str);
    
    // Define test scenarios
    struct {
        const char* desc;
        double a[4];
        double b[4];
        double expected[4];
        int check_bits[4]; // 1 = check bit pattern, 0 = check value
    } tests[] = {
        // Basic cases
        {
            "Basic values",
            {1.5, -2.5, 3.0, -4.0},
            {-1.0, 3.0, 2.0, -3.0},
            {-1.0, -2.5, 2.0, -4.0},
            {0, 0, 0, 0}
        },
        // Zero cases
        {
            "Zeros",
            {0.0, -0.0, 0.0, -0.0},
            {0.0, 0.0, -0.0, -0.0},
            {0.0, 0.0, -0.0, -0.0},
            {1, 1, 1, 1}  // Check bit patterns
        },
        // NaN cases
        {
            "First operand NaN",
            {NAN, NAN, NAN, NAN},
            {1.0, -1.0, INFINITY, -INFINITY},
            {1.0, -1.0, INFINITY, -INFINITY},
            {0, 0, 0, 0}
        },
        {
            "Second operand NaN",
            {1.0, -1.0, INFINITY, -INFINITY},
            {NAN, NAN, NAN, NAN},
            {NAN, NAN, NAN, NAN},
            {0, 0, 0, 0}
        },
        // SNaN cases
        {
            "Second operand SNaN",
            {1.0, -1.0, INFINITY, -INFINITY},
            {create_snan_d(), create_snan_d(), create_snan_d(), create_snan_d()},
            {create_snan_d(), create_snan_d(), create_snan_d(), create_snan_d()},
            {1, 1, 1, 1}  // Check bit patterns
        },
        // Infinity cases
        {
            "Infinity values",
            {INFINITY, -INFINITY, INFINITY, -INFINITY},
            {1.0, -1.0, -INFINITY, INFINITY},
            {1.0, -INFINITY, -INFINITY, -INFINITY},
            {0, 0, 0, 0}
        }
    };

    int num_tests = sizeof(tests)/sizeof(tests[0]);
    
    for (int t = 0; t < num_tests; t++) {
        printf("\nTest %d: %s\n", t+1, tests[t].desc);
        
        double a[4] = {0};
        double b[4] = {0};
        double r[4] = {0};
        double expected[4] = {0};
        
        // Copy test data
        for (int i = 0; i < elements; i++) {
            a[i] = tests[t].a[i];
            b[i] = tests[t].b[i];
            expected[i] = tests[t].expected[i];
        }
        
        // Execute instruction
        if (width == 128) {
            __asm__ __volatile__(
                "vmovupd %1, %%xmm0\n\t"
                "vmovupd %2, %%xmm1\n\t"
                "vminpd %%xmm1, %%xmm0, %%xmm2\n\t"
                "vmovupd %%xmm2, %0\n\t"
                : "=m"(r)
                : "m"(a), "m"(b)
                : "xmm0", "xmm1", "xmm2"
            );
        } else {
            __asm__ __volatile__(
                "vmovupd %1, %%ymm0\n\t"
                "vmovupd %2, %%ymm1\n\t"
                "vminpd %%ymm1, %%ymm0, %%ymm2\n\t"
                "vmovupd %%ymm2, %0\n\t"
                : "=m"(r)
                : "m"(a), "m"(b)
                : "ymm0", "ymm1", "ymm2"
            );
        }
        
        print_double_vec("Input A", a, elements);
        print_double_vec("Input B", b, elements);
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
                if (!double_equal(r[i], expected[i], 0.0001)) {
                    printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
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
    test_vminpd(128);  // Test 128-bit version
    test_vminpd(256);  // Test 256-bit version
    printf("VMINPD tests completed\n");
    return 0;
}
