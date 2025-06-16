#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <float.h>
#include "avx.h"

static void test_reg_operand() {
    float input[8] = {
        1.5f, -1.5f, 2.5f, -2.5f,
        3.5f, -3.5f, 4.5f, -4.5f
    };
    int32_t expected[8] = {
        1, -1, 2, -2,
        3, -3, 4, -4
    };
    
    uint32_t old_mxcsr = get_mxcsr();
    
    // Test with default MXCSR
    __m256 src1 = _mm256_loadu_ps(input);
    __m256 src2 = _mm256_loadu_ps(input+8);
    __m256i result1, result2;
    
    __asm__ __volatile__(
        "vcvttps2dq %[src1], %[dst1]\n\t"
        "vcvttps2dq %[src2], %[dst2]"
        : [dst1] "=x" (result1), [dst2] "=x" (result2)
        : [src1] "x" (src1), [src2] "x" (src2)
    );
    
    int32_t res[16];
    _mm256_storeu_si256((__m256i*)res, result1);
    _mm256_storeu_si256((__m256i*)res+8, result2);
    
    printf("VCVTTPS2DQ Test (Register Operand):\n");
   // printf("MXCSR before: 0x%08X\n", old_mxcsr);
    print_mxcsr(old_mxcsr);
    
    for (int i = 0; i < 8; i++) {
        printf("  Input: ");
        if (isnan(input[i])) printf("NaN");
        else if (isinf(input[i])) printf(input[i] > 0 ? "+INF" : "-INF");
        else printf("%.1f", input[i]);
        
        printf(", Expected: 0x%08X, Result: 0x%08X - %s\n",
               expected[i], res[i],
               (expected[i] == res[i]) ? "PASS" : "FAIL");
    }
    printf("\n");
}

static void test_mem_operand() {
    float input[8] = {
        10.1f, -10.1f, 20.2f, -20.2f,
        30.3f, -30.3f, 40.4f, -40.4f
    };
    int32_t expected[8] = {
        10, -10, 20, -20,
        30, -30, 40, -40
    };
    
    __m256i result1, result2;
    
    __asm__ __volatile__(
        "vcvttps2dq %[src1], %[dst1]\n\t"
        "vcvttps2dq %[src2], %[dst2]"
        : [dst1] "=x" (result1), [dst2] "=x" (result2)
        : [src1] "m" (input), [src2] "m" (input[8])
    );
    
    int32_t res[16];
    _mm256_storeu_si256((__m256i*)res, result1);
    _mm256_storeu_si256((__m256i*)res+8, result2);
    
    printf("VCVTTPS2DQ Test (Memory Operand):\n");
   // printf("MXCSR: 0x%08X\n", get_mxcsr());
    
    for (int i = 0; i < 8; i++) {
        printf("  Input: ");
        if (isnan(input[i])) printf("NaN");
        else if (isinf(input[i])) printf(input[i] > 0 ? "+INF" : "-INF");
        else printf("%.1f", input[i]);
        
        printf(", Expected: 0x%08X, Result: 0x%08X - %s\n",
               expected[i], res[i],
               (expected[i] == res[i]) ? "PASS" : "FAIL");
    }
    printf("\n");
}

int main() {
    printf("============================\n");
    printf("VCVTTPS2DQ Instruction Tests\n");
    printf("============================\n\n");
    
    // Test with default MXCSR
    test_reg_operand();
    test_mem_operand();
    
    // Test with different rounding modes
    for (int i=0; i<4; i++) {
        uint32_t mxcsr = (get_mxcsr() & ~0x6000) | (i << 13);
        set_mxcsr(mxcsr);
        printf("\nTesting with MXCSR rounding mode %d\n", i);
        test_reg_operand();
        test_mem_operand();
    }
    
    return 0;
}
