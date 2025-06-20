#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印四字向量
static void print_qword_vec(const char *name, const uint64_t *vec, int len) {
    printf("%s: [", name);
    for (int i = 0; i < len; i++) {
        printf("%016lx", vec[i]);
        if (i < len - 1) printf(" ");
    }
    printf("]\n");
}

// 测试单个比较操作
static int test_case(const char *name, const uint64_t *a, const uint64_t *b, int len) {
    ALIGNED(32) uint64_t result[4] = {0};
    int passed = 1;
    
    if (len == 2) {
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vb = _mm_loadu_si128((const __m128i*)b);
        __m128i vres;
        
        asm volatile("vpcmpeqq %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm_storeu_si128((__m128i*)result, vres);
    } else if (len == 4) {
        __m256i va = _mm256_loadu_si256((const __m256i*)a);
        __m256i vb = _mm256_loadu_si256((const __m256i*)b);
        __m256i vres;
        
        asm volatile("vpcmpeqq %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm256_storeu_si256((__m256i*)result, vres);
    }
    
    // 验证结果
    for (int i = 0; i < len; i++) {
        uint64_t expected = (a[i] == b[i]) ? 0xFFFFFFFFFFFFFFFF : 0x0000000000000000;
        if (result[i] != expected) {
            passed = 0;
            break;
        }
    }
    
    printf("\nTest: %s\n", name);
    print_qword_vec("Operand A", a, len);
    print_qword_vec("Operand B", b, len);
    print_qword_vec("Result", result, len);
    printf("Result: %s\n", passed ? "PASS" : "FAIL");
    
    return passed;
}

int main() {
    int total = 0, passed = 0;
    
    // 测试128位版本
    {
        uint64_t a1[2] = {0x0000000000000000, 0xFFFFFFFFFFFFFFFF};
        uint64_t b1[2] = {0x0000000000000000, 0xFFFFFFFFFFFFFFFF};
        passed += test_case("VPCMPEQQ 128-bit equal", a1, b1, 2);
        total++;
        
        uint64_t a2[2] = {0};
        uint64_t b2[2] = {0xFFFFFFFFFFFFFFFF};
        passed += test_case("VPCMPEQQ 128-bit zero vs 0xFFFFFFFFFFFFFFFF", a2, b2, 2);
        total++;
        
        uint64_t a3[2] = {0x5555555555555555};
        uint64_t b3[2] = {0xAAAAAAAAAAAAAAAA};
        passed += test_case("VPCMPEQQ 128-bit pattern", a3, b3, 2);
        total++;
    }
    
    // 测试256位版本
    {
        uint64_t a1[4];
        uint64_t b1[4];
        for (int i = 0; i < 4; i++) {
            a1[i] = i * 0x1111111111111111;
            b1[i] = i * 0x1111111111111111;
        }
        passed += test_case("VPCMPEQQ 256-bit equal", a1, b1, 4);
        total++;
        
        uint64_t a2[4] = {0};
        uint64_t b2[4] = {0xFFFFFFFFFFFFFFFF};
        passed += test_case("VPCMPEQQ 256-bit zero vs 0xFFFFFFFFFFFFFFFF", a2, b2, 4);
        total++;
        
        uint64_t a3[4];
        uint64_t b3[4];
        for (int i = 0; i < 4; i++) {
            a3[i] = i % 2 ? 0x5555555555555555 : 0xAAAAAAAAAAAAAAAA;
            b3[i] = i % 2 ? 0xAAAAAAAAAAAAAAAA : 0x5555555555555555;
        }
        passed += test_case("VPCMPEQQ 256-bit pattern", a3, b3, 4);
        total++;
    }
    
    // 测试内存操作数
    {
        ALIGNED(32) uint64_t mem_ops[4] = {0x0123456789ABCDEF, 0xFEDCBA9876543210};
        uint64_t a[2] = {0x0123456789ABCDEF, 0xFEDCBA9876543210};
        
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vres;
        
        asm volatile("vpcmpeqq %[mem], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [mem] "m"(*mem_ops));
        
        uint64_t result[2];
        _mm_storeu_si128((__m128i*)result, vres);
        
        int mem_passed = 1;
        for (int i = 0; i < 2; i++) {
            if (result[i] != 0xFFFFFFFFFFFFFFFF) {
                mem_passed = 0;
                break;
            }
        }
        
        printf("\nTest: VPCMPEQQ with memory operand\n");
        print_qword_vec("Operand A", a, 2);
        print_qword_vec("Operand B (mem)", mem_ops, 2);
        print_qword_vec("Result", result, 2);
        printf("Result: %s\n", mem_passed ? "PASS" : "FAIL");
        passed += mem_passed;
        total++;
    }
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
