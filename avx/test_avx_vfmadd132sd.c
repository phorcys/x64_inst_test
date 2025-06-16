#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

static void test_reg_reg_operand() {
    double a = 1.0;
    double b = 5.0;
    double c = 9.0;
    double expected = 1.0*5.0 + 9.0;

    __m128d va = _mm_load_sd(&a);
    __m128d vb = _mm_load_sd(&b);
    __m128d vc = _mm_load_sd(&c);
    
    __asm__ __volatile__(
        "vfmadd132sd %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "x" (vb), [c] "x" (vc)
    );

    double res;
    _mm_store_sd(&res, va);

    printf("VFMADD132SD Test (Register-Register Operand):\n");
    printf("  a=%.1f, b=%.1f, c=%.1f, Expected: %.1f, Result: %.1f - %s\n",
           a, b, c, expected, res,
           (fabs(expected - res) < 0.0001) ? "PASS" : "FAIL");
    printf("\n");
}

static void test_reg_mem_operand() {
    double a = -1.0;
    double b = 0.5;
    double c = 1.0;
    double expected = -1.0*0.5 + 1.0;

    __m128d va = _mm_load_sd(&a);
    __m128d vc = _mm_load_sd(&c);
    
    __asm__ __volatile__(
        "vfmadd132sd %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "m" (b), [c] "x" (vc)
    );

    double res;
    _mm_store_sd(&res, va);

    printf("VFMADD132SD Test (Register-Memory Operand):\n");
    printf("  a=%.1f, b=%.1f, c=%.1f, Expected: %.1f, Result: %.1f - %s\n",
           a, b, c, expected, res,
           (fabs(expected - res) < 0.0001) ? "PASS" : "FAIL");
    printf("\n");
}

int main() {
    printf("=============================\n");
    printf("VFMADD132SD Instruction Tests\n");
    printf("=============================\n\n");

    test_reg_reg_operand();
    test_reg_mem_operand();

    return 0;
}
