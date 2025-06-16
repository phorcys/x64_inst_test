#include "avx.h"
#include <stdio.h>
#include <float.h>

// vfnmadd132ss指令测试
void test_vfnmadd132ss() {
    printf("=== Testing vfnmadd132ss ===\n");
    
    // 测试正常值
    {
        float dst = 2.0f;
        float src1 = 3.0f;
        float src2 = 1.5f;
        
        float result;
        asm volatile (
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfnmadd132ss %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovss %%xmm0, %0\n\t"
            : "=m"(result)
            : "m"(dst), "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Result: %.15f\n", result);
        printf("Expected: %.15f\n", -(dst * src1) + src2);
    }
    
    // 测试边界值
    {
        float dst = FLT_MAX;
        float src1 = 2.0f;
        float src2 = FLT_MIN;
        
        float result;
        asm volatile (
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfnmadd132ss %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovss %%xmm0, %0\n\t"
            : "=m"(result)
            : "m"(dst), "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Boundary result: %.15e\n", result);
        printf("Boundary expected: %.15e\n", -(dst * src1) + src2);
    }
    
    // 测试特殊值(NaN, Inf)
    {
        float dst = INFINITY;
        float src1 = -1.0f;
        float src2 = NAN;
        
        float result;
        asm volatile (
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfnmadd132ss %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovss %%xmm0, %0\n\t"
            : "=m"(result)
            : "m"(dst), "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Special result: %.15f\n", result);
    }
    
    printf("\n");
}

int main() {
    test_vfnmadd132ss();
    return 0;
}
