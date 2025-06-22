#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// VMAXPD - Packed Double-FP Maximum
// Compares packed double-precision floating-point values and returns the maximum values

static void test_vmaxpd_128() {
    printf("Testing VMAXPD (128-bit)\n");
    
    // Test cases
    double a[2] = {1.5, -2.5};
    double b[2] = {-1.0, 3.0};
    double r[2];
    
    // Expected results: max(a0,b0), max(a1,b1)
    double expected[2] = {1.5, 3.0};
    
    __asm__ __volatile__(
        "vmovupd %1, %%xmm0\n\t"
        "vmovupd %2, %%xmm1\n\t"
        "vmaxpd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovupd %%xmm2, %0\n\t"
        : "=m"(r)
        : "m"(a), "m"(b)
        : "xmm0", "xmm1", "xmm2"
    );
    
    print_double_vec("Input A", a, 2);
    print_double_vec("Input B", b, 2);
    print_double_vec("Result", r, 2);
    print_double_vec("Expected", expected, 2);
    
    for (int i = 0; i < 2; i++) {
        if (!double_equal(r[i], expected[i], 0.0001)) {
            printf("Mismatch at index %d: got %f, expected %f\n", 
                   i, r[i], expected[i]);
        }
    }
    printf("\n");
}

static void test_vmaxpd_256() {
    printf("Testing VMAXPD (256-bit)\n");
    
    // Test cases
    double a[4] = {1.5, -2.5, 0.0, -INFINITY};
    double b[4] = {-1.0, 3.0, -0.0, INFINITY};
    double r[4];
    
    // Expected results: when comparing 0.0 and -0.0, returns second operand (-0.0)
    double expected[4] = {1.5, 3.0, -0.0, INFINITY};
    
    __asm__ __volatile__(
        "vmovupd %1, %%ymm0\n\t"
        "vmovupd %2, %%ymm1\n\t"
        "vmaxpd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovupd %%ymm2, %0\n\t"
        : "=m"(r)
        : "m"(a), "m"(b)
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_double_vec("Input A", a, 4);
    print_double_vec("Input B", b, 4);
    print_double_vec("Result", r, 4);
    print_double_vec("Expected", expected, 4);
    
    for (int i = 0; i < 4; i++) {
        if (isnan(expected[i])) {
            if (!isnan(r[i])) {
                printf("Mismatch at index %d: expected NaN, got %f\n", i, r[i]);
            }
        } else if (isinf(expected[i])) {
            if (!isinf(r[i]) || (signbit(expected[i]) != signbit(r[i]))) {
                printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
            }
        } else {
            if (!double_equal(r[i], expected[i], 0.0001)) {
                printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
            }
        }
    }
    printf("\n");
}

static void test_vmaxpd_edge_cases() {
    printf("Testing VMAXPD (Edge Cases)\n");
    
    // Function to create SNaN (signaling NaN)
    double create_snan() {
        uint64_t snan = 0x7FF0000000000001ULL;
        return *(double*)&snan;
    }
    
    struct {
        double a[2];
        double b[2];
        double expected[2];
    } tests[] = {
        // Zeros - VMAXPD returns second operand when equal
        {{0.0, 0.0}, {0.0, -0.0}, {0.0, -0.0}},
        {{-0.0, -0.0}, {0.0, -0.0}, {0.0, -0.0}},
        
        // NaN cases - VMAXPD returns the second operand if first is NaN
        {{NAN, NAN}, {1.0, -1.0}, {1.0, -1.0}},
        
        // NaN cases - VMAXPD returns second operand if it's NaN
        {{1.0, -1.0}, {NAN, NAN}, {NAN, NAN}},
        {{INFINITY, -INFINITY}, {NAN, NAN}, {NAN, NAN}},
        
        // SNaN cases
        {{1.0, -1.0}, {create_snan(), create_snan()}, {create_snan(), create_snan()}},
        {{NAN, NAN}, {create_snan(), create_snan()}, {create_snan(), create_snan()}},
        
        // Infinity cases
        {{INFINITY, -INFINITY}, {1.0, -1.0}, {INFINITY, -1.0}},
        {{1.0, -1.0}, {INFINITY, -INFINITY}, {INFINITY, -1.0}}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        double r[2];
        
        __asm__ __volatile__(
            "vmovupd %1, %%xmm0\n\t"
            "vmovupd %2, %%xmm1\n\t"
            "vmaxpd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovupd %%xmm2, %0\n\t"
            : "=m"(r)
            : "m"(tests[i].a), "m"(tests[i].b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test case %zu\n", i);
        print_double_vec("Input A", tests[i].a, 2);
        print_double_vec("Input B", tests[i].b, 2);
        print_double_vec("Result", r, 2);
        print_double_vec("Expected", tests[i].expected, 2);
        
        for (int j = 0; j < 2; j++) {
            if (isnan(tests[i].expected[j])) {
                if (!isnan(r[j])) {
                    printf("Mismatch at index %d: expected NaN, got %f\n", j, r[j]);
                }
            } else if (isinf(tests[i].expected[j]) && isinf(r[j])) {
                continue; // Skip inf comparison
            } else if (!double_equal(r[j], tests[i].expected[j], 0.0001)) {
                printf("Mismatch at index %d: got %f, expected %f\n", 
                       j, r[j], tests[i].expected[j]);
            }
        }
        printf("\n");
    }
}

int main() {
    test_vmaxpd_128();
    test_vmaxpd_256();
    test_vmaxpd_edge_cases();
    
    printf("VMAXPD tests completed\n");
    return 0;
}
