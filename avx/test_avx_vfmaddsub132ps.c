#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

static void test_reg_reg_operand() {
    float a[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float b[8] = {0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f};
    float c[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float expected[8] = {
        1.0f*0.5f - 1.0f,  // 奇数元素减
        2.0f*1.5f + 2.0f,  // 偶数元素加
        3.0f*2.5f - 3.0f,
        4.0f*3.5f + 4.0f,
        5.0f*4.5f - 5.0f,
        6.0f*5.5f + 6.0f,
        7.0f*6.5f - 7.0f,
        8.0f*7.5f + 8.0f
    };

    __m256 va = _mm256_loadu_ps(a);
    __m256 vb = _mm256_loadu_ps(b);
    __m256 vc = _mm256_loadu_ps(c);
    
    __asm__ __volatile__(
        "vfmaddsub132ps %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "x" (vb), [c] "x" (vc)
    );

    float res[8];
    _mm256_storeu_ps(res, va);

    printf("VFMADDSUB132PS Test (Register-Register Operand):\n");
    for (int i = 0; i < 8; i++) {
        printf("  a=%.1f, b=%.1f, c=%.1f, Expected: %.1f, Result: %.1f - %s\n",
               a[i], b[i], c[i], expected[i], res[i],
               (fabsf(expected[i] - res[i]) < 0.0001f) ? "PASS" : "FAIL");
    }
    printf("\n");
}

static void test_reg_mem_operand() {
    float a[8] = {-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f};
    float b[8] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    float c[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float expected[8] = {
        -1.0f*0.1f - 1.0f,
        -2.0f*0.2f + 2.0f,
        -3.0f*0.3f - 3.0f,
        -4.0f*0.4f + 4.0f,
        -5.0f*0.5f - 5.0f,
        -6.0f*0.6f + 6.0f,
        -7.0f*0.7f - 7.0f,
        -8.0f*0.8f + 8.0f
    };

    __m256 va = _mm256_loadu_ps(a);
    __m256 vc = _mm256_loadu_ps(c);
    
    __asm__ __volatile__(
        "vfmaddsub132ps %[b], %[c], %[a]"
        : [a] "+x" (va)
        : [b] "m" (b), [c] "x" (vc)
    );

    float res[8];
    _mm256_storeu_ps(res, va);

    printf("VFMADDSUB132PS Test (Register-Memory Operand):\n");
    for (int i = 0; i < 8; i++) {
        printf("  a=%.1f, b=%.1f, c=%.1f, Expected: %.1f, Result: %.1f - %s\n",
               a[i], b[i], c[i], expected[i], res[i],
               (fabsf(expected[i] - res[i]) < 0.0001f) ? "PASS" : "FAIL");
    }
    printf("\n");
}

int main() {
    printf("=============================\n");
    printf("VFMADDSUB132PS Instruction Tests\n");
    printf("=============================\n\n");

    test_reg_reg_operand();
    test_reg_mem_operand();

    return 0;
}
