#include "avx.h"
#include <stdio.h>
#include <math.h>

// VMOVLHPS 指令测试
void test_vmovlhps() {
    printf("=== Testing VMOVLHPS ===\n");
    
    // 测试1: 组合两个寄存器的低64位
    printf("\nTest 1: Combine low 64-bits of two registers\n");
    float a[4] = {1.1f, 2.2f, 3.3f, 4.4f};   // 第一个操作数
    float b[4] = {5.5f, 6.6f, 7.7f, 8.8f};   // 第二个操作数
    float dst[4] = {0};
    float expected[4] = {1.1f, 2.2f, 5.5f, 6.6f}; // 期望结果: [a低64位, b低64位]
    
    __asm__ __volatile__ (
        "vmovups %1, %%xmm0\n\t"      // 加载a到xmm0
        "vmovups %2, %%xmm1\n\t"      // 加载b到xmm1
        "vmovlhps %%xmm1, %%xmm0, %%xmm2\n\t" // xmm2 = [xmm1低64位, xmm0高64位]
        "vmovups %%xmm2, %0"          // 存储结果到dst
        : "=m" (*(float (*)[4])dst)
        : "m" (*(const float (*)[4])a), "m" (*(const float (*)[4])b)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("a: %.1f, %.1f, %.1f, %.1f\n", a[0], a[1], a[2], a[3]);
    printf("b: %.1f, %.1f, %.1f, %.1f\n", b[0], b[1], b[2], b[3]);
    printf("dst: %.1f, %.1f, %.1f, %.1f\n", dst[0], dst[1], dst[2], dst[3]);
    
    if(fabsf(dst[0] - expected[0]) < 0.001f && 
       fabsf(dst[1] - expected[1]) < 0.001f &&
       fabsf(dst[2] - expected[2]) < 0.001f && 
       fabsf(dst[3] - expected[3]) < 0.001f) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 特殊值测试 (NaN, Inf)
    printf("\nTest 2: Special values\n");
    float nan = 0.0f/0.0f;
    float inf = 1.0f/0.0f;
    float a2[4] = {1.0f, 2.0f, nan, inf}; 
    float b2[4] = {nan, inf, 3.0f, 4.0f};
    float dst2[4] = {0};
    
    __asm__ __volatile__ (
        "vmovups %1, %%xmm0\n\t"      // 加载a2到xmm0
        "vmovups %2, %%xmm1\n\t"      // 加载b2到xmm1
        "vmovlhps %%xmm1, %%xmm0, %%xmm2\n\t" 
        "vmovups %%xmm2, %0"          // 存储结果到dst2
        : "=m" (*(float (*)[4])dst2)
        : "m" (*(const float (*)[4])a2), "m" (*(const float (*)[4])b2)
        : "xmm0", "xmm1", "xmm2"
    );
    
    int nan_ok0 = isnan(dst2[2]);
    int inf_ok1 = isinf(dst2[3]) && dst2[3] > 0;
    
    printf("dst: %f, %f, %f, %f\n", dst2[0], dst2[1], dst2[2], dst2[3]);
    printf("Checks: [2] is nan:%d, [3] is +inf:%d\n", nan_ok0, inf_ok1);
    
    if(fabsf(dst2[0] - 1.0f) < 0.001f && 
       fabsf(dst2[1] - 2.0f) < 0.001f &&
       nan_ok0 && inf_ok1) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovlhps();
    return 0;
}
