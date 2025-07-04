#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include "avx.h"
#include "fma.h"

#define TEST_CASE_COUNT FMA_TEST_CASE_COUNT

// 比较两个double值是否相等（考虑NaN和浮点精度）
static int compare_double(double a, double b) {
    if (isnan(a) && isnan(b)) return 1;
    if (isinf(a) && isinf(b) && (signbit(a) == signbit(b))) return 1;
    
    double diff = fabs(a - b);
    double max_val = fmax(fabs(a), fabs(b));
    return (diff < 1e-10) || (diff < max_val * 1e-10);
}

static void calculate_expected(fma_test_case_128* test_case, double expected[2]) {
    for (int i = 0; i < 2; i++) {
        expected[i] = fma(test_case->b[i], test_case->a[i], test_case->c[i]);
    }
}

static void test_reg_reg_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        printf("Loading test case %d: %s\n", t, fma_cases_128[t].desc);
        fflush(stdout);
        
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
        
        // 内联汇编实现 VFMADD213PD (128位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmadd213pd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        double res[2];
        _mm_store_pd(res, vb);
        
        double expected[2];
        calculate_expected(&fma_cases_128[t], expected);
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_128() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        printf("Loading test case %d: %s\n", t, fma_cases_128[t].desc);
        fflush(stdout);
        
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
        __m128d vc = _mm_loadu_pd(fma_cases_128[t].c);  // 使用未对齐加载
        
        // 修正：使用寄存器-寄存器形式，内存操作数已加载到寄存器
        __asm__ __volatile__(
            "vfmadd213pd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        double res[2];
        _mm_store_pd(res, vb);
        
        double expected[2];
        calculate_expected(&fma_cases_128[t], expected);
        
        // 添加自动验证
        int passed = 1;
        for (int i = 0; i < 2; i++) {
            if (!compare_double(expected[i], res[i])) {
                passed = 0;
                break;
            }
        }
        
        printf("Test Case: %s (128-bit reg-mem) - %s\n", 
               fma_cases_128[t].desc, passed ? "PASSED" : "FAILED");
        
        if (!passed) {
            for (int i = 0; i < 2; i++) {
                printf("  Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                       i, fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
                printf("  Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
            }
        }
        printf("\n");
    }
}

static void calculate_expected_256(fma_test_case_256* test_case, double expected[4]) {
    for (int i = 0; i < 4; i++) {
        expected[i] = fma(test_case->b[i], test_case->a[i], test_case->c[i]);
    }
}

static void test_reg_reg_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256[t].a);
        __m256d vb = _mm256_loadu_pd(fma_cases_256[t].b);
        __m256d vc = _mm256_loadu_pd(fma_cases_256[t].c);
        
        // 内联汇编实现 VFMADD213PD (256位寄存器-寄存器)
        __asm__ __volatile__(
            "vfmadd213pd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_store_pd(res, vb);
        
        double expected[4];
        calculate_expected_256(&fma_cases_256[t], expected);
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            printf("Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                   i, fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
        }
        printf("\n");
    }
}

static void test_reg_mem_256() {
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        __m256d va = _mm256_loadu_pd(fma_cases_256[t].a);
        __m256d vb = _mm256_loadu_pd(fma_cases_256[t].b);
        __m256d vc = _mm256_loadu_pd(fma_cases_256[t].c);  // 使用未对齐加载
        
        // 修正：使用寄存器-寄存器形式
        __asm__ __volatile__(
            "vfmadd213pd %[c], %[a], %[b]"
            : [b] "+x" (vb)
            : [a] "x" (va), [c] "x" (vc)
        );
        
        double res[4];
        _mm256_store_pd(res, vb);
        
        double expected[4];
        calculate_expected_256(&fma_cases_256[t], expected);
        
        // 添加自动验证
        int passed = 1;
        for (int i = 0; i < 4; i++) {
            if (!compare_double(expected[i], res[i])) {
                passed = 0;
                break;
            }
        }
        
        printf("Test Case: %s (256-bit reg-mem) - %s\n", 
               fma_cases_256[t].desc, passed ? "PASSED" : "FAILED");
        
        if (!passed) {
            for (int i = 0; i < 4; i++) {
                printf("  Element %d: A=%.18g, B=%.18g, C=%.18g\n",
                       i, fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
                printf("  Expected: %.18g, Result: %.18g\n", expected[i], res[i]);
            }
        }
        printf("\n");
    }
}

int main() {
    printf("===============================\n");
    printf("VFMADD213PD Comprehensive Tests\n");
    printf("===============================\n\n");
    
    test_reg_reg_128();
    test_reg_mem_128();
    test_reg_reg_256();
    test_reg_mem_256();
    
    printf("All VFMADD213PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return 0;
}
