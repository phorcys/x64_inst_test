#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

static void test_reg_operand() {
    float input[8] = {1.5f, -1.5f, 2.5f, -2.5f, 3.5f, -3.5f, 4.5f, -4.5f};
    float expected[4][8] = {
        {2.0f, -2.0f, 2.0f, -2.0f, 4.0f, -4.0f, 4.0f, -4.0f},  // Round to nearest (even)
        {1.0f, -2.0f, 2.0f, -3.0f, 3.0f, -4.0f, 4.0f, -5.0f},  // Round down (toward -∞)
        {2.0f, -1.0f, 3.0f, -2.0f, 4.0f, -3.0f, 5.0f, -4.0f},  // Round up (toward +∞)
        {1.0f, -1.0f, 2.0f, -2.0f, 3.0f, -3.0f, 4.0f, -4.0f}   // Round toward zero (truncate)
    };

    for (int mode = 0; mode < 4; mode++) {
        __m256 src = _mm256_loadu_ps(input);
        __m256 result;
        
        switch(mode) {
            case 0: __asm__ __volatile__("vroundps $0, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
            case 1: __asm__ __volatile__("vroundps $1, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
            case 2: __asm__ __volatile__("vroundps $2, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
            case 3: __asm__ __volatile__("vroundps $3, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
        }

        float res[8];
        _mm256_storeu_ps(res, result);

        printf("VROUNDPS Test (Register Operand, Mode %d):\n", mode);
        for (int i = 0; i < 8; i++) {
            printf("  Input: %.1f, Expected: %.1f, Result: %.1f - %s\n",
                   input[i], expected[mode][i], res[i],
                   (expected[mode][i] == res[i]) ? "PASS" : "FAIL");
        }
        printf("\n");
    }
}

static void test_mem_operand() {
    float input[8] = {10.1f, -10.1f, 20.2f, -20.2f, 30.3f, -30.3f, 40.4f, -40.4f};
    float expected[4][8] = {
        {10.0f, -10.0f, 20.0f, -20.0f, 30.0f, -30.0f, 40.0f, -40.0f},  // Round to nearest (even)
        {10.0f, -11.0f, 20.0f, -21.0f, 30.0f, -31.0f, 40.0f, -41.0f},  // Round down (toward -∞)
        {11.0f, -10.0f, 21.0f, -20.0f, 31.0f, -30.0f, 41.0f, -40.0f},  // Round up (toward +∞)
        {10.0f, -10.0f, 20.0f, -20.0f, 30.0f, -30.0f, 40.0f, -40.0f}   // Round toward zero (truncate)
    };

    for (int mode = 0; mode < 4; mode++) {
        __m256 result;
        
        switch(mode) {
            case 0: __asm__ __volatile__("vroundps $0, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
            case 1: __asm__ __volatile__("vroundps $1, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
            case 2: __asm__ __volatile__("vroundps $2, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
            case 3: __asm__ __volatile__("vroundps $3, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
        }

        float res[8];
        _mm256_storeu_ps(res, result);

        printf("VROUNDPS Test (Memory Operand, Mode %d):\n", mode);
        for (int i = 0; i < 8; i++) {
            printf("  Input: %.1f, Expected: %.1f, Result: %.1f - %s\n",
                   input[i], expected[mode][i], res[i],
                   (expected[mode][i] == res[i]) ? "PASS" : "FAIL");
        }
        printf("\n");
    }
}

int main() {
    printf("===========================\n");
    printf("VROUNDPS Instruction Tests\n");
    printf("===========================\n\n");

    test_reg_operand();
    test_mem_operand();

    return 0;
}
