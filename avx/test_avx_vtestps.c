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
static void test_vtestps_128(const char* name, __m128 a, __m128 b) {
    int flags;
    
    asm volatile(
        "vtestps %[b], %[a]\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movl %%eax, %[flags]"
        : [flags] "=r"(flags)
        : [a] "x"(a), [b] "xm"(b)
        : "rax", "cc");
    print_vector128("  A       ", a);
    print_m128i_hex((__m128i)a, "  A (hex) ");
    print_vector128("  B       ", b);
    print_m128i_hex((__m128i)b, "  B (hex) ");
    printf("\nTest 128: %s\n", name);
    print_flags("Result flags", flags);
}

// 测试256位版本
static void test_vtestps_256(const char* name, __m256 a, __m256 b) {
    int flags;
    
    asm volatile(
        "vtestps %[b], %[a]\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movl %%eax, %[flags]"
        : [flags] "=r"(flags)
        : [a] "x"(a), [b] "xm"(b)
        : "rax", "cc");
    
    printf("\nTest 256: %s\n", name);
    print_vector256("  A       ", a);
    print_m256i_hex((__m256i)a, "  A (hex) ");
    print_vector256("  B       ", b);
    print_m256i_hex((__m256i)b, "  B (hex) ");
    print_flags("Result flags", flags);
}

int main() {
    // 测试数据
    __m128 a128 = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 b128 = _mm_set_ps(0.0f, -1.0f, -2.0f, -3.0f);
    __m128 nan128 = _mm_set_ps(NAN, INFINITY, -INFINITY, 0.0f);
    __m128 zero128 = _mm_setzero_ps();
    
    __m256 a256 = _mm256_set_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 b256 = _mm256_set_ps(0.0f, -1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f);
    __m256 nan256 = _mm256_set_ps(NAN, INFINITY, -INFINITY, 0.0f, NAN, INFINITY, -INFINITY, 0.0f);
    __m256 zero256 = _mm256_setzero_ps();
    
    // 128位测试
    test_vtestps_128("Normal values", a128, b128);
    test_vtestps_128("With NaN/Inf", nan128, b128);
    test_vtestps_128("Zero test", zero128, zero128);
    
    // 256位测试
    test_vtestps_256("Normal values", a256, b256);
    test_vtestps_256("With NaN/Inf", nan256, b256);
    test_vtestps_256("Zero test", zero256, zero256);
    
    return 0;
}
