#include "avx.h"
#include <stdio.h>
#include <float.h>

// vfnmadd132sd指令测试
void test_vfnmadd132sd() {
    printf("=== Testing vfnmadd132sd ===\n");
    
    // 测试正常值
    {
        double dst = 2.0;
        double src1 = 3.0;
        double src2 = 1.5;
        
        double result;
        asm volatile (
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfnmadd132sd %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovsd %%xmm0, %0\n\t"
            : "=m"(result)
            : "m"(dst), "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Result: %.15f\n", result);
        printf("Expected: %.15f\n", -(dst * src1) + src2);
    }
    
    // 测试边界值
    {
        double dst = DBL_MAX;
        double src1 = 2.0;
        double src2 = DBL_MIN;
        
        double result;
        asm volatile (
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfnmadd132sd %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovsd %%xmm0, %0\n\t"
            : "=m"(result)
            : "m"(dst), "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Boundary result: %.15e\n", result);
        printf("Boundary expected: %.15e\n", -(dst * src1) + src2);
    }
    
    // 测试特殊值(NaN, Inf)
    {
        double dst = INFINITY;
        double src1 = -1.0;
        double src2 = NAN;
        
        double result;
        asm volatile (
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfnmadd132sd %%xmm1, %%xmm2, %%xmm0\n\t"
            "vmovsd %%xmm0, %0\n\t"
            : "=m"(result)
            : "m"(dst), "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Special result: %.15f\n", result);
    }
    
    printf("\n");
}

int main() {
    test_vfnmadd132sd();
    return 0;
}
