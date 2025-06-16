#include "avx.h"
#include <stdio.h>

// vfnmsub213ss - Fused Negative Multiply-Subtract of Scalar Single-Precision Floating-Point Values
// Performs: -(a*b) - c
// Forms: VFNMSUB213SS xmm1, xmm2, xmm3/m32

static void test_vfnmsub213ss() {
    printf("Testing vfnmsub213ss\n");
    
    // Test case 1: Basic operation
    {
        float a = 1.0f;
        float b = 2.0f;
        float c = 3.0f;
        float res;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfnmsub213ss %%xmm2, %%xmm1, %%xmm0\n\t"
            "vmovss %%xmm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test 1: -(a*b) - c\n");
        printf("a: %f\n", a);
        printf("b: %f\n", b);
        printf("c: %f\n", c);
        printf("result: %f\n", res);
        print_hex_float_vec("result (hex)", &res, 1);
        
        float expected = -(1.0f * 2.0f) - 3.0f;
        if(!float_equal(res, expected, 1e-5f)) {
            printf("Mismatch: got %f, expected %f\n", res, expected);
        }
    }
    
    // Test case 2: Special values (NaN, Inf)
    {
        float a = INFINITY;
        float b = 2.0f;
        float c = 1.0f;
        float res;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfnmsub213ss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("\nTest 2: Special values\n");
        printf("a: %f\n", a);
        printf("b: %f\n", b);
        printf("c: %f\n", c);
        printf("result: %f\n", res);
        print_hex_float_vec("result (hex)", &res, 1);
    }
}

int main() {
    printf("Starting vfnmsub213ss tests\n");
    
    // Save MXCSR
    uint32_t old_mxcsr = get_mxcsr();
    
    // Test with default MXCSR
    test_vfnmsub213ss();
    
    // Test with FTZ/DAZ enabled
    set_mxcsr(old_mxcsr | 0x8040);
    printf("\nTesting with FTZ/DAZ enabled\n");
    test_vfnmsub213ss();
    
    // Restore MXCSR
    set_mxcsr(old_mxcsr);
    
    printf("vfnmsub213ss tests completed\n");
    return 0;
}
