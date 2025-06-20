#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印标志位
static void print_flags(const char* name, int flags) {
    printf("%s: ZF=%d CF=%d\n", name, (flags>>6)&1, (flags>>0)&1);
}

// 测试128位版本
static void test_vtestpd_128(const char* name, __m128d a, __m128d b) {
    int flags;
    
    asm volatile(
        "vtestpd %[b], %[a]\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movl %%eax, %[flags]"
        : [flags] "=r"(flags)
        : [a] "x"(a), [b] "xm"(b)
        : "rax", "cc");
    
    printf("\nTest 128: %s\n", name);
    print_flags("Result flags", flags);
}

// 测试256位版本
static void test_vtestpd_256(const char* name, __m256d a, __m256d b) {
    int flags;
    
    asm volatile(
        "vtestpd %[b], %[a]\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movl %%eax, %[flags]"
        : [flags] "=r"(flags)
        : [a] "x"(a), [b] "xm"(b)
        : "rax", "cc");
    
    printf("\nTest 256: %s\n", name);
    print_flags("Result flags", flags);
}

int main() {
    // 测试数据
    __m128d a128 = _mm_set_pd(1.0, 2.0);
    __m128d b128 = _mm_set_pd(0.0, -1.0);
    __m128d nan128 = _mm_set_pd(NAN, INFINITY);
    __m128d zero128 = _mm_setzero_pd();
    
    __m256d a256 = _mm256_set_pd(1.0, 2.0, 3.0, 4.0);
    __m256d b256 = _mm256_set_pd(0.0, -1.0, -2.0, -3.0);
    __m256d nan256 = _mm256_set_pd(NAN, INFINITY, -INFINITY, 0.0);
    __m256d zero256 = _mm256_setzero_pd();
    
    // 128位测试
    test_vtestpd_128("Normal values", a128, b128);
    test_vtestpd_128("With NaN/Inf", nan128, b128);
    test_vtestpd_128("Zero test", zero128, zero128);
    
    // 256位测试
    test_vtestpd_256("Normal values", a256, b256);
    test_vtestpd_256("With NaN/Inf", nan256, b256);
    test_vtestpd_256("Zero test", zero256, zero256);
    
    return 0;
}
