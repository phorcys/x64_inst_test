#include "avx.h"
#include <stdio.h>

// vfmsub213pd - Fused Multiply-Subtract of Packed Double-Precision Floating-Point Values
// Performs: a*b - c
// Forms: VFMSUB213PD xmm1, xmm2, xmm3/m128
//        VFMSUB213PD ymm1, ymm2, ymm3/m256

static void test_vfmsub213pd_128() {
    printf("Testing vfmsub213pd (128-bit)\n");
    
    // Test case 1: Basic operation
    {
        double a[2] ALIGNED(16) = {1.0, 2.0};
        double b[2] ALIGNED(16) = {3.0, 4.0}; 
        double c[2] ALIGNED(16) = {5.0, 6.0};
        double res[2] ALIGNED(16);
        
        __asm__ __volatile__(
            "vmovapd %1, %%xmm0\n\t"
            "vmovapd %2, %%xmm1\n\t"
            "vmovapd %3, %%xmm2\n\t"
            "vfmsub213pd %%xmm2, %%xmm1, %%xmm0\n\t"
            "vmovapd %%xmm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test 1: a*b - c\n");
        print_double_vec("a", a, 2);
        print_double_vec("b", b, 2);
        print_double_vec("c", c, 2);
        print_double_vec("result", res, 2);
        print_double_vec_hex("result (hex)", res, 2);
        
        double expected[2] = {
            1.0 * 3.0 - 5.0,
            2.0 * 4.0 - 6.0
        };
        
        for(int i=0; i<2; i++) {
            if(!double_equal(res[i], expected[i], 1e-10)) {
                printf("Mismatch at index %d: got %f, expected %f\n", 
                      i, res[i], expected[i]);
            }
        }
    }
    
    // Test case 2: Special values (NaN, Inf)
    {
        double a[2] ALIGNED(16) = {INFINITY, NAN};
        double b[2] ALIGNED(16) = {2.0, 3.0};
        double c[2] ALIGNED(16) = {1.0, 1.0};
        double res[2] ALIGNED(16);
        
        __asm__ __volatile__(
            "vmovapd %1, %%xmm0\n\t"
            "vmovapd %2, %%xmm1\n\t"
            "vmovapd %3, %%xmm2\n\t"
            "vfmsub213pd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovapd %%xmm2, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("\nTest 2: Special values\n");
        print_double_vec("a", a, 2);
        print_double_vec("b", b, 2);
        print_double_vec("c", c, 2);
        print_double_vec("result", res, 2);
        print_double_vec_hex("result (hex)", res, 2);
    }
}

static void test_vfmsub213pd_256() {
    printf("\nTesting vfmsub213pd (256-bit)\n");
    
    // Test case 1: Basic operation
    {
        double a[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
        double b[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0};
        double c[4] ALIGNED(32) = {9.0, 10.0, 11.0, 12.0};
        double res[4] ALIGNED(32);
        
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vmovapd %3, %%ymm2\n\t"
            "vfmsub213pd %%ymm2, %%ymm1, %%ymm0\n\t"
            "vmovapd %%ymm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Test 1: a*b - c\n");
        print_double_vec("a", a, 4);
        print_double_vec("b", b, 4);
        print_double_vec("c", c, 4);
        print_double_vec("result", res, 4);
        print_double_vec_hex("result (hex)", res, 4);
        
        double expected[4] = {
            1.0 * 5.0 - 9.0,
            2.0 * 6.0 - 10.0,
            3.0 * 7.0 - 11.0,
            4.0 * 8.0 - 12.0
        };
        
        for(int i=0; i<4; i++) {
            if(!double_equal(res[i], expected[i], 1e-10)) {
                printf("Mismatch at index %d: got %f, expected %f\n", 
                      i, res[i], expected[i]);
            }
        }
    }
    
    // Test case 2: Memory operand
    {
        double a[4] ALIGNED(32) = {0.1, 0.2, 0.3, 0.4};
        double b[4] ALIGNED(32) = {0.5, 0.6, 0.7, 0.8};
        double c[4] ALIGNED(32) = {0.9, 1.0, 1.1, 1.2};
        double res[4] ALIGNED(32);
        
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vfmsub213pd %3, %%ymm1, %%ymm0\n\t"
            "vmovapd %%ymm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "ymm0", "ymm1"
        );
        
        printf("\nTest 2: Memory operand\n");
        print_double_vec("a", a, 4);
        print_double_vec("b", b, 4);
        print_double_vec("c", c, 4);
        print_double_vec("result", res, 4);
        print_double_vec_hex("result (hex)", res, 4);
    }
}

int main() {
    printf("Starting vfmsub213pd tests\n");
    
    // Save MXCSR
    uint32_t old_mxcsr = get_mxcsr();
    
    // Test with default MXCSR
    test_vfmsub213pd_128();
    test_vfmsub213pd_256();
    
    // Test with FTZ/DAZ enabled
    set_mxcsr(old_mxcsr | 0x8040);
    printf("\nTesting with FTZ/DAZ enabled\n");
    test_vfmsub213pd_128();
    test_vfmsub213pd_256();
    
    // Restore MXCSR
    set_mxcsr(old_mxcsr);
    
    printf("vfmsub213pd tests completed\n");
    return 0;
}
