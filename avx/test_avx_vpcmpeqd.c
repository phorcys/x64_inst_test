#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印双字向量
static void print_dword_vec(const char *name, const uint32_t *vec, int len) {
    printf("%s: [", name);
    for (int i = 0; i < len; i++) {
        printf("%08x", vec[i]);
        if (i < len - 1) printf(" ");
    }
    printf("]\n");
}

// 测试单个比较操作
static int test_case(const char *name, const uint32_t *a, const uint32_t *b, int len) {
    ALIGNED(32) uint32_t result[8] = {0};
    int passed = 1;
    
    if (len == 4) {
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vb = _mm_loadu_si128((const __m128i*)b);
        __m128i vres;
        
        asm volatile("vpcmpeqd %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm_storeu_si128((__m128i*)result, vres);
    } else if (len == 8) {
        __m256i va = _mm256_loadu_si256((const __m256i*)a);
        __m256i vb = _mm256_loadu_si256((const __m256i*)b);
        __m256i vres;
        
        asm volatile("vpcmpeqd %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm256_storeu_si256((__m256i*)result, vres);
    }
    
    // 验证结果
    for (int i = 0; i < len; i++) {
        uint32_t expected = (a[i] == b[i]) ? 0xFFFFFFFF : 0x00000000;
        if (result[i] != expected) {
            passed = 0;
            break;
        }
    }
    
    printf("\nTest: %s\n", name);
    print_dword_vec("Operand A", a, len);
    print_dword_vec("Operand B", b, len);
    print_dword_vec("Result", result, len);
    printf("Result: %s\n", passed ? "PASS" : "FAIL");
    
    return passed;
}

int main() {
    int total = 0, passed = 0;
    
    // 测试128位版本
    {
        // 基本相等测试
        uint32_t a1[4] = {0x00000000, 0x11111111, 0x22222222, 0xFFFFFFFF};
        uint32_t b1[4] = {0x00000000, 0x11111111, 0x22222222, 0xFFFFFFFF};
        passed += test_case("VPCMPEQD 128-bit equal", a1, b1, 4);
        total++;
        
        // 零值测试
        uint32_t a2[4] = {0, 0, 0, 0};
        uint32_t b2[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
        passed += test_case("VPCMPEQD 128-bit zero vs 0xFFFFFFFF", a2, b2, 4);
        total++;
        
        // 模式测试
        uint32_t a3[4] = {0x55555555, 0x55555555, 0x55555555, 0x55555555};
        uint32_t b3[4] = {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA};
        passed += test_case("VPCMPEQD 128-bit pattern", a3, b3, 4);
        total++;
        
        // 边界值测试
        uint32_t a4[4] = {0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF};
        uint32_t b4[4] = {0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF};
        passed += test_case("VPCMPEQD 128-bit boundaries", a4, b4, 4);
        total++;
        
        // 部分相等测试
        uint32_t a5[4] = {1, 2, 3, 4};
        uint32_t b5[4] = {1, 0, 3, 0};
        passed += test_case("VPCMPEQD 128-bit partial equal", a5, b5, 4);
        total++;
    }
    
    // 测试256位版本
    {
        uint32_t a1[8];
        uint32_t b1[8];
        for (int i = 0; i < 8; i++) {
            a1[i] = i * 0x11111111;
            b1[i] = i * 0x11111111;
        }
        passed += test_case("VPCMPEQD 256-bit equal", a1, b1, 8);
        total++;
        
        uint32_t a2[8] = {0};
        uint32_t b2[8] = {0xFFFFFFFF};
        passed += test_case("VPCMPEQD 256-bit zero vs 0xFFFFFFFF", a2, b2, 8);
        total++;
        
        uint32_t a3[8];
        uint32_t b3[8];
        for (int i = 0; i < 8; i++) {
            a3[i] = i % 2 ? 0x55555555 : 0xAAAAAAAA;
            b3[i] = i % 2 ? 0xAAAAAAAA : 0x55555555;
        }
        passed += test_case("VPCMPEQD 256-bit pattern", a3, b3, 8);
        total++;
        
        // 边界值测试
        uint32_t a4[8] = {0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 
                          0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF};
        uint32_t b4[8] = {0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 
                          0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF};
        passed += test_case("VPCMPEQD 256-bit boundaries", a4, b4, 8);
        total++;
        
        // 部分相等测试
        uint32_t a5[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        uint32_t b5[8] = {1, 0, 3, 0, 5, 0, 7, 0};
        passed += test_case("VPCMPEQD 256-bit partial equal", a5, b5, 8);
        total++;
    }
    
    // 测试内存操作数
    {
        // 对齐内存测试
        ALIGNED(32) uint32_t mem_ops[8] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
        uint32_t a[4] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
        passed += test_case("VPCMPEQD 128-bit memory operand (aligned)", a, mem_ops, 4);
        total++;
        
        // 非对齐内存测试
        uint32_t unaligned_mem[6] = {0, 0xCAFEBABE, 0xDEADBEEF, 0x12345678, 0, 0};
        uint32_t* unaligned_ptr = &unaligned_mem[1]; // 故意不对齐
        
        uint32_t c[4] = {0xCAFEBABE, 0xDEADBEEF, 0x12345678, 0};
        passed += test_case("VPCMPEQD 128-bit memory operand (unaligned)", c, unaligned_ptr, 4);
        total++;
    }
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
