#include "avx.h"
#include <stdio.h>
#include <float.h>

void test_vfnmsub132ps_128() {
    __m128 dst = _mm_set_ps(2.0f, 3.0f, 4.0f, 5.0f);
    __m128 src1 = _mm_set_ps(1.5f, 2.5f, 3.5f, 4.5f);
    __m128 src2 = _mm_set_ps(0.5f, 1.5f, 2.5f, 3.5f);
    
    __m128 result;
    asm volatile (
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vmovaps %3, %%xmm2\n\t"
        "vfnmsub132ps %%xmm1, %%xmm2, %%xmm0\n\t"
        "vmovaps %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src1), "m"(src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    float res[4];
    _mm_store_ps(res, result);
    printf("128-bit result: [%.7f, %.7f, %.7f, %.7f]\n", 
           res[0], res[1], res[2], res[3]);
}

void test_vfnmsub132ps_256() {
    __m256 dst = _mm256_set_ps(2.0f, 3.0f, 4.0f, 5.0f, 
                              6.0f, 7.0f, 8.0f, 9.0f);
    __m256 src1 = _mm256_set_ps(1.5f, 2.5f, 3.5f, 4.5f,
                              5.5f, 6.5f, 7.5f, 8.5f);
    __m256 src2 = _mm256_set_ps(0.5f, 1.5f, 2.5f, 3.5f,
                              4.5f, 5.5f, 6.5f, 7.5f);
    
    __m256 result;
    asm volatile (
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vmovaps %3, %%ymm2\n\t"
        "vfnmsub132ps %%ymm1, %%ymm2, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    float res[8];
    _mm256_store_ps(res, result);
    printf("256-bit result: [%.7f, %.7f, %.7f, %.7f, %.7f, %.7f, %.7f, %.7f]\n", 
           res[0], res[1], res[2], res[3], res[4], res[5], res[6], res[7]);
}

void test_vfnmsub132ps_special() {
    __m128 dst = _mm_set_ps(INFINITY, NAN, -INFINITY, 0.0f);
    __m128 src1 = _mm_set_ps(-1.0f, 0.0f, 1.0f, -0.0f);
    __m128 src2 = _mm_set_ps(INFINITY, INFINITY, -INFINITY, NAN);
    
    __m128 result;
    asm volatile (
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vmovaps %3, %%xmm2\n\t"
        "vfnmsub132ps %%xmm1, %%xmm2, %%xmm0\n\t"
        "vmovaps %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src1), "m"(src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    float res[4];
    _mm_store_ps(res, result);
    printf("Special values result: [%.7f, %.7f, %.7f, %.7f]\n", 
           res[0], res[1], res[2], res[3]);
}

int main() {
    printf("=== Testing vfnmsub132ps ===\n");
    test_vfnmsub132ps_128();
    test_vfnmsub132ps_256();
    test_vfnmsub132ps_special();
    return 0;
}
