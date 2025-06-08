#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
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
    
    __m256 src = _mm256_loadu_ps(input);
    __m256i result;
    
    __asm__ __volatile__(
        "vcvttps2dq %[src], %[dst]"
        : [dst] "=x" (result)
        : [src] "x" (src)
    );
    
    int32_t res[8];
    _mm256_storeu_si256((__m256i*)res, result);
    
    printf("VCVTTPS2DQ Test (Register Operand):\n");
    for (int i = 0; i < 8; i++) {
        printf("  Input: %.1f, Expected: %d, Result: %d - %s\n",
               input[i], expected[i], res[i],
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
    
    __m256i result;
    
    __asm__ __volatile__(
        "vcvttps2dq %[src], %[dst]"
        : [dst] "=x" (result)
        : [src] "m" (input)
    );
    
    int32_t res[8];
    _mm256_storeu_si256((__m256i*)res, result);
    
    printf("VCVTTPS2DQ Test (Memory Operand):\n");
    for (int i = 0; i < 8; i++) {
        printf("  Input: %.1f, Expected: %d, Result: %d - %s\n",
               input[i], expected[i], res[i],
               (expected[i] == res[i]) ? "PASS" : "FAIL");
    }
    printf("\n");
}

int main() {
    printf("============================\n");
    printf("VCVTTPS2DQ Instruction Tests\n");
    printf("============================\n\n");
    
    test_reg_operand();
    test_mem_operand();
    
    return 0;
}
