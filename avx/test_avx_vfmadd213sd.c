#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

// 比较两个double值是否相等（考虑NaN和浮点精度）
static int compare_double(double a, double b) {
    if (isnan(a) && isnan(b)) return 1;
    if (isinf(a) && isinf(b) && (signbit(a) == signbit(b))) return 1;
    
    double diff = fabs(a - b);
    double max_val = fmax(fabs(a), fabs(b));
    return (diff < 1e-15) || (diff < max_val * 1e-15);
}

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

static void test_reg_reg() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_128[t].a, 
               ((uintptr_t)fma_cases_128[t].a % 16 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_128[t].b, 
               ((uintptr_t)fma_cases_128[t].b % 16 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_128[t].c, 
               ((uintptr_t)fma_cases_128[t].c % 16 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        __m128d va = _mm_loadu_pd(fma_cases_128[t].a);
        __m128d vb = _mm_loadu_pd(fma_cases_128[t].b);
        __m128d vc = _mm_loadu_pd(fma_cases_128[t].c);
        
        // 保存原始高位值
        double original_high = vb[1];
        
        // 内联汇编实现 VFMADD213SD (标量双精度)
        __asm__ __volatile__(
            "vfmadd213sd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        double res = vb[0];
        double expected = fma(fma_cases_128[t].b[0], fma_cases_128[t].a[0], fma_cases_128[t].c[0]);
        
        // 添加自动验证
        int passed = compare_double(expected, res) && 
                    compare_double(vb[1], original_high); // 检查高位不变
        
        printf("Test Case: %s (reg-reg) - %s\n", 
               fma_cases_128[t].desc, passed ? "PASSED" : "FAILED");
        printf("A=%.18g, B=%.18g, C=%.18g\n",
               fma_cases_128[t].a[0], fma_cases_128[t].b[0], fma_cases_128[t].c[0]);
        printf("Expected: %.18g, Result: %.18g\n", expected, res);
        printf("High element: %.18g (original: %.18g) - %s\n", 
               vb[1], original_high, 
               compare_double(vb[1], original_high) ? "unchanged" : "changed!");
        
        if (!passed) {
            printf("  Difference: %.18g\n", fabs(expected - res));
        }
        printf("\n");
    }
}

static void test_reg_mem() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_128[t].a, 
               ((uintptr_t)fma_cases_128[t].a % 16 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_128[t].b, 
               ((uintptr_t)fma_cases_128[t].b % 16 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_128[t].c, 
               ((uintptr_t)fma_cases_128[t].c % 16 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        __m128d va = _mm_loadu_pd(fma_cases_128[t].a);
        __m128d vb = _mm_loadu_pd(fma_cases_128[t].b);
        double c_val = fma_cases_128[t].c[0];  // 标量值
        
        // 保存原始高位值
        double original_high = vb[1];
        
        // 内联汇编实现 VFMADD213SD (寄存器-内存)
        __asm__ __volatile__(
            "vfmadd213sd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "m" (c_val)
        );
        
        double res = vb[0];
        double expected = fma(fma_cases_128[t].b[0], fma_cases_128[t].a[0], fma_cases_128[t].c[0]);
        
        // 添加自动验证
        int passed = compare_double(expected, res) && 
                    compare_double(vb[1], original_high); // 检查高位不变
        
        printf("Test Case: %s (reg-mem) - %s\n", 
               fma_cases_128[t].desc, passed ? "PASSED" : "FAILED");
        printf("A=%.18g, B=%.18g, C=%.18g\n",
               fma_cases_128[t].a[0], fma_cases_128[t].b[0], fma_cases_128[t].c[0]);
        printf("Expected: %.18g, Result: %.18g\n", expected, res);
        printf("High element: %.18g (original: %.18g) - %s\n", 
               vb[1], original_high, 
               compare_double(vb[1], original_high) ? "unchanged" : "changed!");
        
        if (!passed) {
            printf("  Difference: %.18g\n", fabs(expected - res));
        }
        printf("\n");
    }
}

int main() {
    printf("===============================\n");
    printf("VFMADD213SD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg();
    test_reg_mem();
    
    printf("All VFMADD213SD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
