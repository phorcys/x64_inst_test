#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// VMAXPS - Packed Single-FP Maximum
// Compares packed single-precision floating-point values and returns the maximum values

static void test_vmaxps_128() {
    printf("Testing VMAXPS (128-bit)\n");
    
    float a[4] = {1.5f, -2.5f, 3.0f, -4.0f};
    float b[4] = {-1.0f, 3.0f, 2.0f, -3.0f};
    float r[4];
    float expected[4] = {1.5f, 3.0f, 3.0f, -3.0f};
    
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"
        "vmovups %2, %%xmm1\n\t"
        "vmaxps %%xmm1, %%xmm0, %%xmm2\n\t"
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
        if (!float_equal(r[i], expected[i], 0.0001f)) {
            printf("Mismatch at index %d: got %f, expected %f\n", 
                   i, r[i], expected[i]);
        }
    }
    printf("\n");
}

static void test_vmaxps_256() {
    printf("Testing VMAXPS (256-bit)\n");
    
    float a[8] = {1.5f, -2.5f, 3.0f, -4.0f, 0.0f, -0.0f, INFINITY, -INFINITY};
    float b[8] = {-1.0f, 3.0f, 2.0f, -3.0f, -0.0f, 0.0f, 1.0f, -1.0f};
    float r[8];
    float expected[8] = {1.5f, 3.0f, 3.0f, -3.0f, -0.0f, 0.0f, INFINITY, -1.0f};
    
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vmaxps %%ymm1, %%ymm0, %%ymm2\n\t"
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
        } else if (isinf(expected[i])) {
            if (!isinf(r[i]) || (signbit(expected[i]) != signbit(r[i]))) {
                printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
            }
        } else {
            if (!float_equal(r[i], expected[i], 0.0001f)) {
                printf("Mismatch at index %d: got %f, expected %f\n", i, r[i], expected[i]);
            }
        }
    }
    printf("\n");
}

static void test_vmaxps_edge_cases() {
    printf("Testing VMAXPS (Edge Cases)\n");
    
    // Function to create SNaN (signaling NaN)
    float create_snan() {
        uint32_t snan = 0x7F800001;
        return *(float*)&snan;
    }
    
    struct {
        float a[4];
        float b[4];
        float expected[4];
    } tests[] = {
        // Zeros - VMAXPS returns second operand when equal
        {{0.0f, 0.0f, 0.0f, 0.0f}, 
         {0.0f, -0.0f, -0.0f, 0.0f}, 
         {0.0f, -0.0f, -0.0f, 0.0f}},
         
        {{-0.0f, -0.0f, -0.0f, -0.0f}, 
         {0.0f, -0.0f, 0.0f, -0.0f}, 
         {0.0f, -0.0f, 0.0f, -0.0f}},
        
        // NaN cases - VMAXPS returns the second operand if first is NaN
        {{NAN, NAN, NAN, NAN}, 
         {1.0f, -1.0f, 2.0f, -2.0f}, 
         {1.0f, -1.0f, 2.0f, -2.0f}},
        
        // NaN cases - VMAXPS returns second operand if it's NaN
        {{1.0f, -1.0f, 2.0f, -2.0f}, 
         {NAN, NAN, NAN, NAN}, 
         {NAN, NAN, NAN, NAN}},
        
        {{INFINITY, -INFINITY, 3.0f, -3.0f}, 
         {NAN, NAN, NAN, NAN}, 
         {NAN, NAN, NAN, NAN}},
        
        // SNaN cases
        {{1.0f, -1.0f, 2.0f, -2.0f}, 
         {create_snan(), create_snan(), create_snan(), create_snan()}, 
         {create_snan(), create_snan(), create_snan(), create_snan()}},
        
        {{NAN, NAN, NAN, NAN}, 
         {create_snan(), create_snan(), create_snan(), create_snan()}, 
         {create_snan(), create_snan(), create_snan(), create_snan()}},
        
        // Infinity cases
        {{INFINITY, -INFINITY, INFINITY, -INFINITY}, 
         {1.0f, -1.0f, -2.0f, 2.0f}, 
         {INFINITY, -1.0f, INFINITY, 2.0f}},
         
        {{1.0f, -1.0f, -2.0f, 2.0f}, 
         {INFINITY, -INFINITY, INFINITY, -INFINITY}, 
         {INFINITY, -1.0f, INFINITY, 2.0f}}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        float r[4];
        
        __asm__ __volatile__(
            "vmovups %1, %%xmm0\n\t"
            "vmovups %2, %%xmm1\n\t"
            "vmaxps %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovups %%xmm2, %0\n\t"
            : "=m"(r)
            : "m"(tests[i].a), "m"(tests[i].b)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test case %zu\n", i);
        print_float_vec("Input A", tests[i].a, 4);
        print_float_vec("Input B", tests[i].b, 4);
        print_float_vec("Result", r, 4);
        print_float_vec("Expected", tests[i].expected, 4);
        
        for (int j = 0; j < 4; j++) {
            if (isnan(tests[i].expected[j])) {
                if (!isnan(r[j])) {
                    printf("Mismatch at index %d: expected NaN, got %f\n", j, r[j]);
                }
            } else if (isinf(tests[i].expected[j])) {
                if (!isinf(r[j]) || (signbit(tests[i].expected[j]) != signbit(r[j]))) {
                    printf("Mismatch at index %d: got %f, expected %f\n", j, r[j], tests[i].expected[j]);
                }
            } else {
                if (!float_equal(r[j], tests[i].expected[j], 0.0001f)) {
                    printf("Mismatch at index %d: got %f, expected %f\n", 
                           j, r[j], tests[i].expected[j]);
                }
            }
        }
        printf("\n");
    }
}

int main() {
    test_vmaxps_128();
    test_vmaxps_256();
    test_vmaxps_edge_cases();
    printf("VMAXPS tests completed\n");
    return 0;
}
