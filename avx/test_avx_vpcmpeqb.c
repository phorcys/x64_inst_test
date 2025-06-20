#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印字节向量
static void print_byte_vec(const char *name, const uint8_t *vec, int len) {
    printf("%s: [", name);
    for (int i = 0; i < len; i++) {
        printf("%02x", vec[i]);
        if (i < len - 1) printf(" ");
    }
    printf("]\n");
}

// 测试单个比较操作
static int test_case(const char *name, const uint8_t *a, const uint8_t *b, int len) {
    ALIGNED(32) uint8_t result[32] = {0};
    int passed = 1;
    
    if (len == 16) {
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vb = _mm_loadu_si128((const __m128i*)b);
        __m128i vres;
        
        asm volatile("vpcmpeqb %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm_storeu_si128((__m128i*)result, vres);
    } else if (len == 32) {
        __m256i va = _mm256_loadu_si256((const __m256i*)a);
        __m256i vb = _mm256_loadu_si256((const __m256i*)b);
        __m256i vres;
        
        asm volatile("vpcmpeqb %[b], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [b] "xm"(vb));
        
        _mm256_storeu_si256((__m256i*)result, vres);
    }
    
    // 验证结果
    for (int i = 0; i < len; i++) {
        uint8_t expected = (a[i] == b[i]) ? 0xFF : 0x00;
        if (result[i] != expected) {
            passed = 0;
            break;
        }
    }
    
    printf("\nTest: %s\n", name);
    print_byte_vec("Operand A", a, len);
    print_byte_vec("Operand B", b, len);
    print_byte_vec("Result", result, len);
    printf("Result: %s\n", passed ? "PASS" : "FAIL");
    
    return passed;
}

int main() {
    int total = 0, passed = 0;
    
    // 测试128位版本
    {
        uint8_t a1[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
                         0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
        uint8_t b1[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
                         0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
        passed += test_case("VPCMPEQB 128-bit equal", a1, b1, 16);
        total++;
        
        uint8_t a2[16] = {0};
        uint8_t b2[16] = {0xFF};
        passed += test_case("VPCMPEQB 128-bit zero vs 0xFF", a2, b2, 16);
        total++;
        
        uint8_t a3[16] = {0x55};
        uint8_t b3[16] = {0xAA};
        passed += test_case("VPCMPEQB 128-bit pattern", a3, b3, 16);
        total++;
    }
    
    // 测试256位版本
    {
        uint8_t a1[32];
        uint8_t b1[32];
        for (int i = 0; i < 32; i++) {
            a1[i] = i;
            b1[i] = i;
        }
        passed += test_case("VPCMPEQB 256-bit equal", a1, b1, 32);
        total++;
        
        uint8_t a2[32] = {0};
        uint8_t b2[32] = {0xFF};
        passed += test_case("VPCMPEQB 256-bit zero vs 0xFF", a2, b2, 32);
        total++;
        
        uint8_t a3[32];
        uint8_t b3[32];
        for (int i = 0; i < 32; i++) {
            a3[i] = i % 2 ? 0x55 : 0xAA;
            b3[i] = i % 2 ? 0xAA : 0x55;
        }
        passed += test_case("VPCMPEQB 256-bit pattern", a3, b3, 32);
        total++;
    }
    
    // 测试内存操作数
    {
        ALIGNED(32) uint8_t mem_ops[32] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
                                          0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10};
        uint8_t a[16] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
                        0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10};
        
        __m128i va = _mm_loadu_si128((const __m128i*)a);
        __m128i vres;
        
        asm volatile("vpcmpeqb %[mem], %[a], %[res]"
                    : [res] "=x"(vres)
                    : [a] "x"(va), [mem] "m"(*mem_ops));
        
        uint8_t result[16];
        _mm_storeu_si128((__m128i*)result, vres);
        
        int mem_passed = 1;
        for (int i = 0; i < 16; i++) {
            if (result[i] != 0xFF) {
                mem_passed = 0;
                break;
            }
        }
        
        printf("\nTest: VPCMPEQB with memory operand\n");
        print_byte_vec("Operand A", a, 16);
        print_byte_vec("Operand B (mem)", mem_ops, 16);
        print_byte_vec("Result", result, 16);
        printf("Result: %s\n", mem_passed ? "PASS" : "FAIL");
        passed += mem_passed;
        total++;
    }
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
