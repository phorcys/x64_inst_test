#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

static void test_reg_reg_operand() {
    float a = 1.0f;
    float b = 5.0f;
    float c = 9.0f;
    float expected = 1.0f*5.0f - 9.0f;

    __m128 va = _mm_load_ss(&a);
    __m128 vb = _mm_load_ss(&b);
    __m128 vc = _mm_load_ss(&c);
    
    __asm__ __volatile__(
        "vfmsub132ss %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "x" (vb), [c] "x" (vc)
    );

    float res;
    _mm_store_ss(&res, va);

    printf("VFMSUB132SS Test (Register-Register Operand):\n");
    printf("  a=%.1f, b=%.1f, c=%.1f\n", a, b, c);
    printf("  Expected: %.1f, Result: %.1f - %s\n", 
           expected, res,
           (fabsf(expected - res) < 0.0001f) ? "PASS" : "FAIL");
    printf("\n");
}

static void test_reg_mem_operand() {
    float a = -1.0f;
    float b = 0.5f;
    float c = 1.0f;
    float expected = -1.0f*0.5f - 1.0f;

    __m128 va = _mm_load_ss(&a);
    __m128 vc = _mm_load_ss(&c);
    
    __asm__ __volatile__(
        "vfmsub132ss %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "m" (b), [c] "x" (vc)
    );

    float res;
    _mm_store_ss(&res, va);

    printf("VFMSUB132SS Test (Register-Memory Operand):\n");
    printf("  a=%.1f, b=%.1f, c=%.1f\n", a, b, c);
    printf("  Expected: %.1f, Result: %.1f - %s\n", 
           expected, res,
           (fabsf(expected - res) < 0.0001f) ? "PASS" : "FAIL");
    printf("\n");
}

int main() {
    printf("=============================\n");
    printf("VFMSUB132SS Instruction Tests\n");
    printf("=============================\n\n");

    test_reg_reg_operand();
    test_reg_mem_operand();

    return 0;
}
