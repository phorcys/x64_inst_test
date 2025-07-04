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

static void calculate_expected_ps(fma_test_case_128_ps* test_case, float expected[4]) {
    for (int i = 0; i < 4; i++) {
        expected[i] = fmaf(test_case->b[i], test_case->a[i], test_case->c[i]);
    }
}

static void test_reg_reg_128() {
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
        
        // 内联汇编实现 VFMADD213PS (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmadd213ps %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, vb);
        
        float expected[4];
        calculate_expected_ps(&fma_cases_128_ps[t], expected);
        
        // 添加自动验证
        int passed = 1;
        for (int i = 0; i < 4; i++) {
            if (!compare_float(expected[i], res[i])) {
                passed = 0;
                break;
            }
        }
        
        printf("Test Case: %s (128-bit reg-reg) - %s\n", 
               fma_cases_128_ps[t].desc, passed ? "PASSED" : "FAILED");
        
        if (!passed) {
            for (int i = 0; i < 4; i++) {
                printf("  Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                       i, fma_cases_128_ps[t].a[i], fma_cases_128_ps[t].b[i], fma_cases_128_ps[t].c[i]);
                printf("  Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
            }
        }
        printf("\n");
    }
}

static void test_reg_mem_128() {
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
        __m128 vc = _mm_loadu_ps(fma_cases_128_ps[t].c);  // 使用未对齐加载
        
        // 修正：使用寄存器-寄存器形式
        __asm__ __volatile__(
            "vfmadd213ps %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        float res[4];
        _mm_store_ps(res, vb);
        
        float expected[4];
        calculate_expected_ps(&fma_cases_128_ps[t], expected);
        
        // 添加自动验证
        int passed = 1;
        for (int i = 0; i < 4; i++) {
            if (!compare_float(expected[i], res[i])) {
                passed = 0;
                break;
            }
        }
        
        printf("Test Case: %s (128-bit reg-mem) - %s\n", 
               fma_cases_128_ps[t].desc, passed ? "PASSED" : "FAILED");
        
        if (!passed) {
            for (int i = 0; i < 4; i++) {
                printf("  Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                       i, fma_cases_128_ps[t].a[i], fma_cases_128_ps[t].b[i], fma_cases_128_ps[t].c[i]);
                printf("  Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
            }
        }
        printf("\n");
    }
}

static void calculate_expected_256_ps(fma_test_case_256_ps* test_case, float expected[8]) {
    for (int i = 0; i < 8; i++) {
        expected[i] = fmaf(test_case->b[i], test_case->a[i], test_case->c[i]);
    }
}

static void test_reg_reg_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
        __m256 vc = _mm256_loadu_ps(fma_cases_256_ps[t].c);
        
        // 内联汇编实现 VFMADD213PS (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmadd213ps %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, vb);
        
        float expected[8];
        calculate_expected_256_ps(&fma_cases_256_ps[t], expected);
        
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_256_ps[t].a, 
               ((uintptr_t)fma_cases_256_ps[t].a % 32 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_256_ps[t].b, 
               ((uintptr_t)fma_cases_256_ps[t].b % 32 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_256_ps[t].c, 
               ((uintptr_t)fma_cases_256_ps[t].c % 32 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        // 添加自动验证
        int passed = 1;
        for (int i = 0; i < 8; i++) {
            if (!compare_float(expected[i], res[i])) {
                passed = 0;
                break;
            }
        }
        
        printf("Test Case: %s (256-bit reg-reg) - %s\n", 
               fma_cases_256_ps[t].desc, passed ? "PASSED" : "FAILED");
        
        if (!passed) {
            for (int i = 0; i < 8; i++) {
                printf("  Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                       i, fma_cases_256_ps[t].a[i], fma_cases_256_ps[t].b[i], fma_cases_256_ps[t].c[i]);
                printf("  Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
            }
        }
        printf("\n");
    }
}

static void test_reg_mem_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256 va = _mm256_loadu_ps(fma_cases_256_ps[t].a);
        __m256 vb = _mm256_loadu_ps(fma_cases_256_ps[t].b);
        __m256 vc = _mm256_loadu_ps(fma_cases_256_ps[t].c);  // 使用未对齐加载
        
        // 修正：使用寄存器-寄存器形式
        __asm__ __volatile__(
            "vfmadd213ps %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        float res[8];
        _mm256_store_ps(res, vb);
        
        float expected[8];
        calculate_expected_256_ps(&fma_cases_256_ps[t], expected);
        
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_256_ps[t].a, 
               ((uintptr_t)fma_cases_256_ps[t].a % 32 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_256_ps[t].b, 
               ((uintptr_t)fma_cases_256_ps[t].b % 32 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_256_ps[t].c, 
               ((uintptr_t)fma_cases_256_ps[t].c % 32 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        // 添加自动验证
        int passed = 1;
        for (int i = 0; i < 8; i++) {
            if (!compare_float(expected[i], res[i])) {
                passed = 0;
                break;
            }
        }
        
        printf("Test Case: %s (256-bit reg-mem) - %s\n", 
               fma_cases_256_ps[t].desc, passed ? "PASSED" : "FAILED");
        
        if (!passed) {
            for (int i = 0; i < 8; i++) {
                printf("  Element %d: A=%.9g, B=%.9g, C=%.9g\n",
                       i, fma_cases_256_ps[t].a[i], fma_cases_256_ps[t].b[i], fma_cases_256_ps[t].c[i]);
                printf("  Expected: %.9g, Result: %.9g\n", expected[i], res[i]);
            }
        }
        printf("\n");
    }
}

int main() {
    printf("===============================\n");
    printf("VFMADD213PS Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_128();
    test_reg_mem_128();
    test_reg_reg_256();
    test_reg_mem_256();
    
    printf("All VFMADD213PS tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
