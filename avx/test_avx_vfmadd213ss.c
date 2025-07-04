#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

// 比较两个float值是否相等（考虑NaN和浮点精度）
static int compare_float(float a, float b) {
    if (isnan(a) && isnan(b)) return 1;
    if (isinf(a) && isinf(b) && (signbit(a) == signbit(b))) return 1;
    
    float diff = fabsf(a - b);
    float max_val = fmaxf(fabsf(a), fabsf(b));
    return (diff < 1e-7f) || (diff < max_val * 1e-7f);
}

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void test_reg_reg() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_128_ps[t].a, 
               ((uintptr_t)fma_cases_128_ps[t].a % 16 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_128_ps[t].b, 
               ((uintptr_t)fma_cases_128_ps[t].b % 16 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_128_ps[t].c, 
               ((uintptr_t)fma_cases_128_ps[t].c % 16 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        __m128 va = _mm_loadu_ps(fma_cases_128_ps[t].a);
        __m128 vb = _mm_loadu_ps(fma_cases_128_ps[t].b);
        __m128 vc = _mm_loadu_ps(fma_cases_128_ps[t].c);
        
        // 保存原始高位值
        float original_high[3] = {vb[1], vb[2], vb[3]};
        
        // 内联汇编实现 VFMADD213SS (标量单精度)
        __asm__ __volatile__(
            "vfmadd213ss %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        float res = vb[0];
        float expected = fmaf(fma_cases_128_ps[t].b[0], fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].c[0]);
        
        // 添加自动验证
        int passed = compare_float(expected, res) && 
                    compare_float(vb[1], original_high[0]) &&
                    compare_float(vb[2], original_high[1]) &&
                    compare_float(vb[3], original_high[2]);
        
        printf("Test Case: %s (reg-reg) - %s\n", 
               fma_cases_128_ps[t].desc, passed ? "PASSED" : "FAILED");
        printf("A=%.9g, B=%.9g, C=%.9g\n",
               fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].b[0], fma_cases_128_ps[t].c[0]);
        printf("Expected: %.9g, Result: %.9g\n", expected, res);
        
        // 单独检查每个高位元素
        printf("High element 1: %.9g (original: %.9g) - %s\n", 
               vb[1], original_high[0], 
               compare_float(vb[1], original_high[0]) ? "unchanged" : "changed!");
        printf("High element 2: %.9g (original: %.9g) - %s\n", 
               vb[2], original_high[1], 
               compare_float(vb[2], original_high[1]) ? "unchanged" : "changed!");
        printf("High element 3: %.9g (original: %.9g) - %s\n", 
               vb[3], original_high[2], 
               compare_float(vb[3], original_high[2]) ? "unchanged" : "changed!");
        
        if (!passed) {
            printf("  Difference: %.9g\n", fabsf(expected - res));
        }
        printf("\n");
    }
}

static void test_reg_mem() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_128_ps[t].a, 
               ((uintptr_t)fma_cases_128_ps[t].a % 16 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_128_ps[t].b, 
               ((uintptr_t)fma_cases_128_ps[t].b % 16 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_128_ps[t].c, 
               ((uintptr_t)fma_cases_128_ps[t].c % 16 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        __m128 va = _mm_loadu_ps(fma_cases_128_ps[t].a);
        __m128 vb = _mm_loadu_ps(fma_cases_128_ps[t].b);
        float c_val = fma_cases_128_ps[t].c[0];  // 标量值
        
        // 保存原始高位值
        float original_high[3] = {vb[1], vb[2], vb[3]};
        
        // 内联汇编实现 VFMADD213SS (寄存器-内存)
        __asm__ __volatile__(
            "vfmadd213ss %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "m" (c_val)
        );
        
        float res = vb[0];
        float expected = fmaf(fma_cases_128_ps[t].b[0], fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].c[0]);
        
        // 添加自动验证
        int passed = compare_float(expected, res) && 
                    compare_float(vb[1], original_high[0]) &&
                    compare_float(vb[2], original_high[1]) &&
                    compare_float(vb[3], original_high[2]);
        
        printf("Test Case: %s (reg-mem) - %s\n", 
               fma_cases_128_ps[t].desc, passed ? "PASSED" : "FAILED");
        printf("A=%.9g, B=%.9g, C=%.9g\n",
               fma_cases_128_ps[t].a[0], fma_cases_128_ps[t].b[0], fma_cases_128_ps[t].c[0]);
        printf("Expected: %.9g, Result: %.9g\n", expected, res);
        
        // 单独检查每个高位元素
        printf("High element 1: %.9g (original: %.9g) - %s\n", 
               vb[1], original_high[0], 
               compare_float(vb[1], original_high[0]) ? "unchanged" : "changed!");
        printf("High element 2: %.9g (original: %.9g) - %s\n", 
               vb[2], original_high[1], 
               compare_float(vb[2], original_high[1]) ? "unchanged" : "changed!");
        printf("High element 3: %.9g (original: %.9g) - %s\n", 
               vb[3], original_high[2], 
               compare_float(vb[3], original_high[2]) ? "unchanged" : "changed!");
        
        if (!passed) {
            printf("  Difference: %.9g\n", fabsf(expected - res));
        }
        printf("\n");
    }
}

int main() {
    printf("===============================\n");
    printf("VFMADD213SS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg();
    test_reg_mem();
    
    printf("All VFMADD213SS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
