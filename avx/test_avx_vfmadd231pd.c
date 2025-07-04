#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>
#include <string.h>
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

static int test_reg_reg_128() {
    int passed_count = 0;
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_128[t].a, 
               ((uintptr_t)fma_cases_128[t].a % 16 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_128[t].b, 
               ((uintptr_t)fma_cases_128[t].b % 16 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_128[t].c, 
               ((uintptr_t)fma_cases_128[t].c % 16 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        // 初始化：目标寄存器 = c，源寄存器1 = a，源寄存器2 = b
        __m128d vdest = _mm_loadu_pd(fma_cases_128[t].c);
        __m128d vsrc1 = _mm_loadu_pd(fma_cases_128[t].a);
        __m128d vsrc2 = _mm_loadu_pd(fma_cases_128[t].b);
        
        // 内联汇编实现 VFMADD231PD (128位寄存器-寄存器)
        // 语义: vdest = vsrc1 * vsrc2 + vdest
        __asm__ __volatile__(
            "vfmadd231pd %[src1], %[src2], %[dest]"
            : [dest] "+x" (vdest)
            : [src1] "x" (vsrc1), [src2] "x" (vsrc2)
        );
        
        double res[2];
        _mm_storeu_pd(res, vdest);
        
        int element_passed[2] = {0};
        int case_passed = 1;
        
        printf("Test Case: %s (128-bit reg-reg)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            double expected = fma(fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            element_passed[i] = compare_double(expected, res[i]);
            case_passed &= element_passed[i];
            
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n", i, 
                   fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("  Expected: %.9g, Result: %.9g - %s\n", 
                   expected, res[i], element_passed[i] ? "PASSED" : "FAILED");
        }
        
        if (case_passed) {
            passed_count++;
            printf("=> Test Case PASSED\n");
        } else {
            printf("=> Test Case FAILED\n");
        }
        printf("\n");
    }
    return passed_count;
}

static int test_reg_mem_128() {
    int passed_count = 0;
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_128[t].a, 
               ((uintptr_t)fma_cases_128[t].a % 16 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_128[t].b, 
               ((uintptr_t)fma_cases_128[t].b % 16 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_128[t].c, 
               ((uintptr_t)fma_cases_128[t].c % 16 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        // 初始化：目标寄存器 = c，源寄存器1 = a，源寄存器2 = b
        __m128d vdest = _mm_loadu_pd(fma_cases_128[t].c);
        __m128d vsrc1 = _mm_loadu_pd(fma_cases_128[t].a);
        __m128d vsrc2 = _mm_loadu_pd(fma_cases_128[t].b);
        
        // 内联汇编实现 VFMADD231PD (128位寄存器-寄存器)
        // 语义: vdest = vsrc1 * vsrc2 + vdest
        __asm__ __volatile__(
            "vfmadd231pd %[src1], %[src2], %[dest]"
            : [dest] "+x" (vdest)
            : [src1] "x" (vsrc1), [src2] "x" (vsrc2)
        );
        
        double res[2];
        _mm_storeu_pd(res, vdest);
        
        int element_passed[2] = {0};
        int case_passed = 1;
        
        printf("Test Case: %s (128-bit reg-mem)\n", fma_cases_128[t].desc);
        for (int i = 0; i < 2; i++) {
            double expected = fma(fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            element_passed[i] = compare_double(expected, res[i]);
            case_passed &= element_passed[i];
            
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n", i, 
                   fma_cases_128[t].a[i], fma_cases_128[t].b[i], fma_cases_128[t].c[i]);
            printf("  Expected: %.9g, Result: %.9g - %s\n", 
                   expected, res[i], element_passed[i] ? "PASSED" : "FAILED");
        }
        
        if (case_passed) {
            passed_count++;
            printf("=> Test Case PASSED\n");
        } else {
            printf("=> Test Case FAILED\n");
        }
        printf("\n");
    }
    return passed_count;
}

static int test_reg_reg_256() {
    int passed_count = 0;
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_256[t].a, 
               ((uintptr_t)fma_cases_256[t].a % 32 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_256[t].b, 
               ((uintptr_t)fma_cases_256[t].b % 32 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_256[t].c, 
               ((uintptr_t)fma_cases_256[t].c % 32 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        // 初始化：目标寄存器 = c，源寄存器1 = a，源寄存器2 = b
        __m256d vdest = _mm256_loadu_pd(fma_cases_256[t].c);
        __m256d vsrc1 = _mm256_loadu_pd(fma_cases_256[t].a);
        __m256d vsrc2 = _mm256_loadu_pd(fma_cases_256[t].b);
        
        // 内联汇编实现 VFMADD231PD (256位寄存器-寄存器)
        // 语义: vdest = vsrc1 * vsrc2 + vdest
        __asm__ __volatile__(
            "vfmadd231pd %[src1], %[src2], %[dest]"
            : [dest] "+x" (vdest)
            : [src1] "x" (vsrc1), [src2] "x" (vsrc2)
        );
        
        double res[4];
        _mm256_storeu_pd(res, vdest);
        
        int element_passed[4] = {0};
        int case_passed = 1;
        
        printf("Test Case: %s (256-bit reg-reg)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            double expected = fma(fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            element_passed[i] = compare_double(expected, res[i]);
            case_passed &= element_passed[i];
            
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n", i, 
                   fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("  Expected: %.9g, Result: %.9g - %s\n", 
                   expected, res[i], element_passed[i] ? "PASSED" : "FAILED");
        }
        
        if (case_passed) {
            passed_count++;
            printf("=> Test Case PASSED\n");
        } else {
            printf("=> Test Case FAILED\n");
        }
        printf("\n");
    }
    return passed_count;
}

static int test_reg_mem_256() {
    int passed_count = 0;
    for (int t = 0; t < TEST_CASE_COUNT; t++) {
        // 添加对齐检查
        printf("a ptr: %p, alignment: %s\n", fma_cases_256[t].a, 
               ((uintptr_t)fma_cases_256[t].a % 32 == 0) ? "aligned" : "unaligned");
        printf("b ptr: %p, alignment: %s\n", fma_cases_256[t].b, 
               ((uintptr_t)fma_cases_256[t].b % 32 == 0) ? "aligned" : "unaligned");
        printf("c ptr: %p, alignment: %s\n", fma_cases_256[t].c, 
               ((uintptr_t)fma_cases_256[t].c % 32 == 0) ? "aligned" : "unaligned");
        fflush(stdout);
        
        // 初始化：目标寄存器 = c，源寄存器1 = a，源寄存器2 = b
        __m256d vdest = _mm256_loadu_pd(fma_cases_256[t].c);
        __m256d vsrc1 = _mm256_loadu_pd(fma_cases_256[t].a);
        __m256d vsrc2 = _mm256_loadu_pd(fma_cases_256[t].b);
        
        // 内联汇编实现 VFMADD231PD (256位寄存器-寄存器)
        // 语义: vdest = vsrc1 * vsrc2 + vdest
        __asm__ __volatile__(
            "vfmadd231pd %[src1], %[src2], %[dest]"
            : [dest] "+x" (vdest)
            : [src1] "x" (vsrc1), [src2] "x" (vsrc2)
        );
        
        double res[4];
        _mm256_storeu_pd(res, vdest);
        
        int element_passed[4] = {0};
        int case_passed = 1;
        
        printf("Test Case: %s (256-bit reg-mem)\n", fma_cases_256[t].desc);
        for (int i = 0; i < 4; i++) {
            double expected = fma(fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            element_passed[i] = compare_double(expected, res[i]);
            case_passed &= element_passed[i];
            
            printf("Element %d: A=%.9g, B=%.9g, C=%.9g\n", i, 
                   fma_cases_256[t].a[i], fma_cases_256[t].b[i], fma_cases_256[t].c[i]);
            printf("  Expected: %.9g, Result: %.9g - %s\n", 
                   expected, res[i], element_passed[i] ? "PASSED" : "FAILED");
        }
        
        if (case_passed) {
            passed_count++;
            printf("=> Test Case PASSED\n");
        } else {
            printf("=> Test Case FAILED\n");
        }
        printf("\n");
    }
    return passed_count;
}

int main() {
    printf("==================================\n");
    printf("VFMADD231PD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    int total_cases = TEST_CASE_COUNT * 4; // 4种测试类型
    int passed_cases = 0;
    
    printf("=== Testing 128-bit register-register ===\n");
    passed_cases += test_reg_reg_128();
    
    printf("=== Testing 128-bit register-memory ===\n");
    passed_cases += test_reg_mem_128();
    
    printf("=== Testing 256-bit register-register ===\n");
    passed_cases += test_reg_reg_256();
    
    printf("=== Testing 256-bit register-memory ===\n");
    passed_cases += test_reg_mem_256();
    
    printf("==================================\n");
    printf("Test Summary: %d/%d cases passed\n", passed_cases, total_cases);
    printf("All VFMADD231PD tests completed. Results are for verification on physical CPU vs box64.\n");
    
    return (passed_cases == total_cases) ? 0 : 1;
}
