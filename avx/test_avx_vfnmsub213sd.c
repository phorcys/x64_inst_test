#include "avx.h"
#include <stdio.h>

// vfnmsub213sd - Fused Negative Multiply-Subtract of Scalar Double-Precision Floating-Point Values
// Performs: -(a*b) - c
// Forms: VFNMSUB213SD xmm1, xmm2, xmm3/m64

static void test_vfnmsub213sd() {
    printf("Testing vfnmsub213sd\n");
    
    // Test case 1: Basic operation
    {
        double a = 1.0;
        double b = 2.0;
        double c = 3.0;
        double res;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfnmsub213sd %%xmm2, %%xmm1, %%xmm0\n\t"
            "vmovsd %%xmm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test 1: -(a*b) - c\n");
        printf("a: %f\n", a);
        printf("b: %f\n", b);
        printf("c: %f\n", c);
        printf("result: %f\n", res);
        print_double_vec_hex("result (hex)", &res, 1);
        
        double expected = -(1.0 * 2.0) - 3.0;
        if(!double_equal(res, expected, 1e-10)) {
            printf("Mismatch: got %f, expected %f\n", res, expected);
        }
    }
    
    // Test case 2: Special values (NaN, Inf)
    {
        double a = INFINITY;
        double b = 2.0;
        double c = 1.0;
        double res;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfnmsub213sd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("\nTest 2: Special values\n");
        printf("a: %f\n", a);
        printf("b: %f\n", b);
        printf("c: %f\n", c);
        printf("result: %f\n", res);
        print_double_vec_hex("result (hex)", &res, 1);
    }
}

int main() {
    printf("Starting vfnmsub213sd tests\n");
    
    // Save MXCSR
    uint32_t old_mxcsr = get_mxcsr();
    
    // Test with default MXCSR
    test_vfnmsub213sd();
    
    // Test with FTZ/DAZ enabled
    set_mxcsr(old_mxcsr | 0x8040);
    printf("\nTesting with FTZ/DAZ enabled\n");
    test_vfnmsub213sd();
    
    // Restore MXCSR
    set_mxcsr(old_mxcsr);
    
    printf("vfnmsub213sd tests completed\n");
    return 0;
}
