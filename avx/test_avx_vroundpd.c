#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

static void test_reg_operand() {
    double input[4] = {1.5, -1.5, 2.5, -2.5};
    double expected[4][4] = {
        {2.0, -2.0, 2.0, -2.0},  // Round to nearest (even)
        {1.0, -2.0, 2.0, -3.0},  // Round down (toward -∞)
        {2.0, -1.0, 3.0, -2.0},  // Round up (toward +∞)
        {1.0, -1.0, 2.0, -2.0}   // Round toward zero (truncate)
    };

    for (int mode = 0; mode < 4; mode++) {
        __m256d src = _mm256_loadu_pd(input);
        __m256d result;
        
        switch(mode) {
            case 0: __asm__ __volatile__("vroundpd $0, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
            case 1: __asm__ __volatile__("vroundpd $1, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
            case 2: __asm__ __volatile__("vroundpd $2, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
            case 3: __asm__ __volatile__("vroundpd $3, %[src], %[dst]" : [dst] "=x" (result) : [src] "x" (src)); break;
        }

        double res[4];
        _mm256_storeu_pd(res, result);

        printf("VROUNDPD Test (Register Operand, Mode %d):\n", mode);
        for (int i = 0; i < 4; i++) {
            printf("  Input: %.1f, Expected: %.1f, Result: %.1f - %s\n",
                   input[i], expected[mode][i], res[i],
                   (expected[mode][i] == res[i]) ? "PASS" : "FAIL");
        }
        printf("\n");
    }
}

static void test_mem_operand() {
    double input[4] = {10.1, -10.1, 20.2, -20.2};
    double expected[4][4] = {
        {10.0, -10.0, 20.0, -20.0},  // Round to nearest (even)
        {10.0, -11.0, 20.0, -21.0},  // Round down (toward -∞)
        {11.0, -10.0, 21.0, -20.0},  // Round up (toward +∞)
        {10.0, -10.0, 20.0, -20.0}   // Round toward zero (truncate)
    };

    for (int mode = 0; mode < 4; mode++) {
        __m256d result;
        
        switch(mode) {
            case 0: __asm__ __volatile__("vroundpd $0, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
            case 1: __asm__ __volatile__("vroundpd $1, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
            case 2: __asm__ __volatile__("vroundpd $2, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
            case 3: __asm__ __volatile__("vroundpd $3, %[src], %[dst]" : [dst] "=x" (result) : [src] "m" (input)); break;
        }

        double res[4];
        _mm256_storeu_pd(res, result);

        printf("VROUNDPD Test (Memory Operand, Mode %d):\n", mode);
        for (int i = 0; i < 4; i++) {
            printf("  Input: %.1f, Expected: %.1f, Result: %.1f - %s\n",
                   input[i], expected[mode][i], res[i],
                   (expected[mode][i] == res[i]) ? "PASS" : "FAIL");
        }
        printf("\n");
    }
}

int main() {
    printf("===========================\n");
    printf("VROUNDPD Instruction Tests\n");
    printf("===========================\n\n");

    test_reg_operand();
    test_mem_operand();

    return 0;
}
