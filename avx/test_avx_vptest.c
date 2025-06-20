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
static void test_vptest_128(const char* name, __m128i a, __m128i b) {
    int flags;
    
    asm volatile(
        "vptest %[b], %[a]\n\t"
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
static void test_vptest_256(const char* name, __m256i a, __m256i b) {
    int flags;
    
    asm volatile(
        "vptest %[b], %[a]\n\t"
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
    __m128i a128 = _mm_set_epi32(0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA);
    __m128i b128 = _mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xAAAAAAAA, 0x55555555);
    __m128i zero128 = _mm_setzero_si128();
    
    __m256i a256 = _mm256_set_epi32(0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA,
                                   0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788);
    __m256i b256 = _mm256_set_epi32(0x00000000, 0xFFFFFFFF, 0xAAAAAAAA, 0x55555555,
                                   0x87654321, 0x0FEDCBA9, 0x44332211, 0x88776655);
    __m256i zero256 = _mm256_setzero_si256();
    
    // 128位测试
    test_vptest_128("Normal case", a128, b128);
    test_vptest_128("All zeros", zero128, zero128);
    test_vptest_128("Self test", a128, a128);
    
    // 256位测试
    test_vptest_256("Normal case", a256, b256);
    test_vptest_256("All zeros", zero256, zero256);
    test_vptest_256("Self test", a256, a256);
    
    return 0;
}
