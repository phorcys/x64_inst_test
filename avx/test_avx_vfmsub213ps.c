#include "avx.h"
#include <stdio.h>

// vfmsub213ps - Fused Multiply-Subtract of Packed Single-Precision Floating-Point Values
// Performs: a*b - c
// Forms: VFMSUB213PS xmm1, xmm2, xmm3/m128
//        VFMSUB213PS ymm1, ymm2, ymm3/m256

static void test_vfmsub213ps_128() {
    printf("Testing vfmsub213ps (128-bit)\n");
    
    // Test case 1: Basic operation
    {
        float a[4] ALIGNED(16) = {1.0f, 2.0f, 3.0f, 4.0f};
        float b[4] ALIGNED(16) = {5.0f, 6.0f, 7.0f, 8.0f};
        float c[4] ALIGNED(16) = {9.0f, 10.0f, 11.0f, 12.0f};
        float res[4] ALIGNED(16);
        
        __asm__ __volatile__(
            "vmovaps %1, %%xmm0\n\t"
            "vmovaps %2, %%xmm1\n\t"
            "vmovaps %3, %%xmm2\n\t"
            "vfmsub213ps %%xmm2, %%xmm1, %%xmm0\n\t"
            "vmovaps %%xmm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test 1: a*b - c\n");
        print_float_vec("a", a, 4);
        print_float_vec("b", b, 4);
        print_float_vec("c", c, 4);
        print_float_vec("result", res, 4);
        print_hex_float_vec("result (hex)", res, 4);
        
        float expected[4] = {
            1.0f * 5.0f - 9.0f,
            2.0f * 6.0f - 10.0f,
            3.0f * 7.0f - 11.0f,
            4.0f * 8.0f - 12.0f
        };
        
        for(int i=0; i<4; i++) {
            if(!float_equal(res[i], expected[i], 1e-5f)) {
                printf("Mismatch at index %d: got %f, expected %f\n", 
                      i, res[i], expected[i]);
            }
        }
    }
    
    // Test case 2: Special values (NaN, Inf)
    {
        float a[4] ALIGNED(16) = {INFINITY, NAN, -INFINITY, 0.0f};
        float b[4] ALIGNED(16) = {2.0f, 3.0f, -1.0f, 1.0f};
        float c[4] ALIGNED(16) = {1.0f, 1.0f, 1.0f, 1.0f};
        float res[4] ALIGNED(16);
        
        __asm__ __volatile__(
            "vmovaps %1, %%xmm0\n\t"
            "vmovaps %2, %%xmm1\n\t"
            "vmovaps %3, %%xmm2\n\t"
            "vfmsub213ps %%xmm2, %%xmm1, %%xmm0\n\t"
            "vmovaps %%xmm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("\nTest 2: Special values\n");
        print_float_vec("a", a, 4);
        print_float_vec("b", b, 4);
        print_float_vec("c", c, 4);
        print_float_vec("result", res, 4);
        print_hex_float_vec("result (hex)", res, 4);
    }
}

static void test_vfmsub213ps_256() {
    printf("\nTesting vfmsub213ps (256-bit)\n");
    
    // Test case 1: Basic operation
    {
        float a[8] ALIGNED(32) = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        float b[8] ALIGNED(32) = {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
        float c[8] ALIGNED(32) = {17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f};
        float res[8] ALIGNED(32);
        
        __asm__ __volatile__(
            "vmovaps %1, %%ymm0\n\t"
            "vmovaps %2, %%ymm1\n\t"
            "vmovaps %3, %%ymm2\n\t"
            "vfmsub213ps %%ymm2, %%ymm1, %%ymm0\n\t"
            "vmovaps %%ymm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Test 1: a*b - c\n");
        print_float_vec("a", a, 8);
        print_float_vec("b", b, 8);
        print_float_vec("c", c, 8);
        print_float_vec("result", res, 8);
        print_hex_float_vec("result (hex)", res, 8);
        
        float expected[8] = {
            1.0f * 9.0f - 17.0f,
            2.0f * 10.0f - 18.0f,
            3.0f * 11.0f - 19.0f,
            4.0f * 12.0f - 20.0f,
            5.0f * 13.0f - 21.0f,
            6.0f * 14.0f - 22.0f,
            7.0f * 15.0f - 23.0f,
            8.0f * 16.0f - 24.0f
        };
        
        for(int i=0; i<8; i++) {
            if(!float_equal(res[i], expected[i], 1e-5f)) {
                printf("Mismatch at index %d: got %f, expected %f\n", 
                      i, res[i], expected[i]);
            }
        }
    }
    
    // Test case 2: Memory operand
    {
        float a[8] ALIGNED(32) = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
        float b[8] ALIGNED(32) = {0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f};
        float c[8] ALIGNED(32) = {1.7f, 1.8f, 1.9f, 2.0f, 2.1f, 2.2f, 2.3f, 2.4f};
        float res[8] ALIGNED(32);
        
        __asm__ __volatile__(
            "vmovaps %1, %%ymm0\n\t"
            "vmovaps %2, %%ymm1\n\t"
            "vfmsub213ps %3, %%ymm1, %%ymm0\n\t"
            "vmovaps %%ymm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b), "m"(c)
            : "ymm0", "ymm1"
        );
        
        printf("\nTest 2: Memory operand\n");
        print_float_vec("a", a, 8);
        print_float_vec("b", b, 8);
        print_float_vec("c", c, 8);
        print_float_vec("result", res, 8);
        print_hex_float_vec("result (hex)", res, 8);
    }
}

int main() {
    printf("Starting vfmsub213ps tests\n");
    
    // Save MXCSR
    uint32_t old_mxcsr = get_mxcsr();
    
    // Test with default MXCSR
    test_vfmsub213ps_128();
    test_vfmsub213ps_256();
    
    // Test with FTZ/DAZ enabled
    set_mxcsr(old_mxcsr | 0x8040);
    printf("\nTesting with FTZ/DAZ enabled\n");
    test_vfmsub213ps_128();
    test_vfmsub213ps_256();
    
    // Restore MXCSR
    set_mxcsr(old_mxcsr);
    
    printf("vfmsub213ps tests completed\n");
    return 0;
}
