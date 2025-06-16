#include "avx.h"
#include <stdio.h>
#include <float.h>

void test_vfnmsub132pd_128() {
    __m128d dst = _mm_set_pd(2.0, 3.0);
    __m128d src1 = _mm_set_pd(4.0, 5.0);
    __m128d src2 = _mm_set_pd(1.5, 2.5);
    
    __m128d result;
    asm volatile (
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vmovapd %3, %%xmm2\n\t"
        "vfnmsub132pd %%xmm1, %%xmm2, %%xmm0\n\t"
        "vmovapd %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src1), "m"(src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    double res[2];
    _mm_store_pd(res, result);
    printf("128-bit result: [%.15f, %.15f]\n", res[0], res[1]);
}

void test_vfnmsub132pd_256() {
    __m256d dst = _mm256_set_pd(2.0, 3.0, 4.0, 5.0);
    __m256d src1 = _mm256_set_pd(1.5, 2.5, 3.5, 4.5);
    __m256d src2 = _mm256_set_pd(0.5, 1.5, 2.5, 3.5);
    
    __m256d result;
    asm volatile (
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vmovapd %3, %%ymm2\n\t"
        "vfnmsub132pd %%ymm1, %%ymm2, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    double res[4];
    _mm256_store_pd(res, result);
    printf("256-bit result: [%.15f, %.15f, %.15f, %.15f]\n", 
           res[0], res[1], res[2], res[3]);
}

void test_vfnmsub132pd_special() {
    __m128d dst = _mm_set_pd(INFINITY, NAN);
    __m128d src1 = _mm_set_pd(-1.0, 0.0);
    __m128d src2 = _mm_set_pd(INFINITY, INFINITY);
    
    __m128d result;
    asm volatile (
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vmovapd %3, %%xmm2\n\t"
        "vfnmsub132pd %%xmm1, %%xmm2, %%xmm0\n\t"
        "vmovapd %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src1), "m"(src2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    double res[2];
    _mm_store_pd(res, result);
    printf("Special values result: [%.15f, %.15f]\n", res[0], res[1]);
}

int main() {
    printf("=== Testing vfnmsub132pd ===\n");
    test_vfnmsub132pd_128();
    test_vfnmsub132pd_256();
    test_vfnmsub132pd_special();
    return 0;
}
