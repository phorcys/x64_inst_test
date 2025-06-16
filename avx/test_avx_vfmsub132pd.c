#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

static void test_reg_reg_operand() {
    double a[4] = {1.0, 2.0, 3.0, 4.0};
    double b[4] = {5.0, 6.0, 7.0, 8.0};
    double c[4] = {9.0, 10.0, 11.0, 12.0};
    double expected[4] = {
        1.0*5.0 - 9.0,
        2.0*6.0 - 10.0,
        3.0*7.0 - 11.0,
        4.0*8.0 - 12.0
    };

    __m256d va = _mm256_loadu_pd(a);
    __m256d vb = _mm256_loadu_pd(b);
    __m256d vc = _mm256_loadu_pd(c);
    
    __asm__ __volatile__(
        "vfmsub132pd %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "x" (vb), [c] "x" (vc)
    );

    double res[4];
    _mm256_storeu_pd(res, va);

    printf("VFMSUB132PD Test (Register-Register Operand):\n");
    for (int i = 0; i < 4; i++) {
        printf("  a=%.1f, b=%.1f, c=%.1f, Expected: %.1f, Result: %.1f - %s\n",
               a[i], b[i], c[i], expected[i], res[i],
               (fabs(expected[i] - res[i]) < 0.0001) ? "PASS" : "FAIL");
    }
    printf("\n");
}

static void test_reg_mem_operand() {
    double a[4] = {-1.0, -2.0, -3.0, -4.0};
    double b[4] = {0.5, 1.5, 2.5, 3.5};
    double c[4] = {1.0, 2.0, 3.0, 4.0};
    double expected[4] = {
        -1.0*0.5 - 1.0,
        -2.0*1.5 - 2.0,
        -3.0*2.5 - 3.0,
        -4.0*3.5 - 4.0
    };

    __m256d va = _mm256_loadu_pd(a);
    __m256d vc = _mm256_loadu_pd(c);
    
    __asm__ __volatile__(
        "vfmsub132pd %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "m" (b), [c] "x" (vc)
    );

    double res[4];
    _mm256_storeu_pd(res, va);

    printf("VFMSUB132PD Test (Register-Memory Operand):\n");
    for (int i = 0; i < 4; i++) {
        printf("  a=%.1f, b=%.1f, c=%.1f, Expected: %.1f, Result: %.1f - %s\n",
               a[i], b[i], c[i], expected[i], res[i],
               (fabs(expected[i] - res[i]) < 0.0001) ? "PASS" : "FAIL");
    }
    printf("\n");
}

int main() {
    printf("=============================\n");
    printf("VFMSUB132PD Instruction Tests\n");
    printf("=============================\n\n");

    test_reg_reg_operand();
    test_reg_mem_operand();

    return 0;
}
