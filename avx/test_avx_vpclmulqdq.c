#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include "avx.h"

// 无进位乘法参考实现 (软件实现)
static __m128i clmul_reference(__m128i a, __m128i b, uint8_t imm8) {
    uint64_t a_part = (imm8 & 1) ? _mm_extract_epi64(a, 1) : _mm_extract_epi64(a, 0);
    uint64_t b_part = (imm8 & 0x10) ? _mm_extract_epi64(b, 1) : _mm_extract_epi64(b, 0);
    
    uint64_t res_high = 0;
    uint64_t res_low = 0;
    
    // 修正的无进位乘法算法
    for (int i = 0; i < 64; i++) {
        if (a_part & (1ULL << i)) {
            // 处理低位部分
            res_low ^= b_part << i;
            // 处理高位部分（当i>0时）
            if (i > 0) {
                res_high ^= b_part >> (64 - i);
            }
        }
    }
    
    return _mm_set_epi64x(res_high, res_low);
}

// 宏定义实现不同立即数版本的VPCLMULQDQ
#define DEFINE_VPCLMUL_FUNC(imm) \
static inline __m128i vpclmulqdq_imm##imm(__m128i a, __m128i b) { \
    __m128i res; \
    asm volatile("vpclmulqdq $"#imm ", %2, %1, %0" \
                 : "=x"(res) \
                 : "x"(a), "m"(b)); \
    return res; \
}

// 定义所有需要的立即数版本
DEFINE_VPCLMUL_FUNC(0x00)
DEFINE_VPCLMUL_FUNC(0x01)
DEFINE_VPCLMUL_FUNC(0x10)
DEFINE_VPCLMUL_FUNC(0x11)

// 测试用例结构
typedef struct {
    const char* name;
    uint8_t imm8;
    __m128i a128;
    __m128i b128;
} TestCase128;

typedef struct {
    const char* name;
    uint8_t imm8;
    __m256i a256;
    __m256i b256;
} TestCase256;

int main() {
    printf("Starting VPCLMULQDQ tests...\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // ================= 128位测试用例 =================
    TestCase128 test_cases128[] = {
        // 基本测试
        {"imm=0x00 (low x low)", 0x00, 
         _mm_setr_epi64x(0x0000000000000000, 0x0000000000000000),
         _mm_setr_epi64x(0x0000000000000000, 0x0000000000000000)},
        
        {"imm=0x11 (high x high)", 0x11,
         _mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000),
         _mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000)},
        
        {"imm=0x01 (high x low)", 0x01,
         _mm_setr_epi64x(0x123456789ABCDEF0, 0x0FEDCBA987654321),
         _mm_setr_epi64x(0x1122334455667788, 0x99AABBCCDDEEFF00)},
        
        {"imm=0x10 (low x high)", 0x10,
         _mm_setr_epi64x(0x0011223344556677, 0x8899AABBCCDDEEFF),
         _mm_setr_epi64x(0xFFEEDDCCBBAA9988, 0x7766554433221100)},
        
        // 边界值测试
        {"All ones", 0x00,
         _mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000),
         _mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000)},
        
        {"Alternating bits", 0x11,
         _mm_setr_epi64x(0xAAAAAAAAAAAAAAAA, 0x5555555555555555),
         _mm_setr_epi64x(0x5555555555555555, 0xAAAAAAAAAAAAAAAA)},
        
        {"AES-GCM polynomial", 0x00,
         _mm_setr_epi64x(0x0000000000000087, 0x0000000000000000),
         _mm_setr_epi64x(0x0000000000000087, 0x0000000000000000)},
    };
    
    // 执行128位测试
    int num_128_tests = sizeof(test_cases128)/sizeof(TestCase128);
    for (int i = 0; i < num_128_tests; i++) {
        TestCase128* tc = &test_cases128[i];
        total_tests++;
        
        printf("\nTest %d/%d: %s\n", i+1, num_128_tests, tc->name);
        printf("imm8: 0x%02X\n", tc->imm8);
        
        // 打印输入值
        print_xmm("Input A", tc->a128);
        print_xmm("Input B", tc->b128);
        
        // 根据立即数选择正确的指令版本
        __m128i res_asm;
        switch(tc->imm8) {
            case 0x00: res_asm = vpclmulqdq_imm0x00(tc->a128, tc->b128); break;
            case 0x01: res_asm = vpclmulqdq_imm0x01(tc->a128, tc->b128); break;
            case 0x10: res_asm = vpclmulqdq_imm0x10(tc->a128, tc->b128); break;
            case 0x11: res_asm = vpclmulqdq_imm0x11(tc->a128, tc->b128); break;
            default:
                printf("❌ Invalid imm8 value: 0x%02X\n", tc->imm8);
                res_asm = _mm_setzero_si128();
        }
        __m128i res_ref = clmul_reference(tc->a128, tc->b128, tc->imm8);
        
        // 打印结果
        print_xmm("ASM Result", res_asm);
        print_xmm("Ref Result", res_ref);
        
        // 结果比对
        if (cmp_xmm(res_asm, res_ref)) {
            printf("✅ Test PASSED\n");
            passed_tests++;
        } else {
            printf("❌ Test FAILED\n");
        }
    }
    
    // ================= 256位测试用例 =================
    TestCase256 test_cases256[] = {
        {"256-bit: imm=0x00", 0x00,
         _mm256_setr_epi64x(0, 0, 0, 0),
         _mm256_setr_epi64x(0, 0, 0, 0)},
         
        {"256-bit: imm=0x11", 0x11,
         _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 
                            0xAAAAAAAAAAAAAAAA, 0x5555555555555555),
         _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000,
                            0x5555555555555555, 0xAAAAAAAAAAAAAAAA)},
    };
    
    // 执行256位测试
    int num_256_tests = sizeof(test_cases256)/sizeof(TestCase256);
    for (int i = 0; i < num_256_tests; i++) {
        TestCase256* tc = &test_cases256[i];
        total_tests++;
        
        printf("\nTest %d/%d: %s\n", num_128_tests+i+1, num_128_tests+num_256_tests, tc->name);
        printf("imm8: 0x%02X\n", tc->imm8);
        
        // 打印输入值
        print_ymm("Input A", tc->a256);
        print_ymm("Input B", tc->b256);
        
        // 256位指令执行 - 需要分情况处理
        __m256i res_asm;
        switch(tc->imm8) {
            case 0x00:
                asm volatile("vpclmulqdq $0x00, %2, %1, %0"
                             : "=x"(res_asm) : "x"(tc->a256), "m"(tc->b256)); 
                break;
            case 0x01:
                asm volatile("vpclmulqdq $0x01, %2, %1, %0"
                             : "=x"(res_asm) : "x"(tc->a256), "m"(tc->b256)); 
                break;
            case 0x10:
                asm volatile("vpclmulqdq $0x10, %2, %1, %0"
                             : "=x"(res_asm) : "x"(tc->a256), "m"(tc->b256)); 
                break;
            case 0x11:
                asm volatile("vpclmulqdq $0x11, %2, %1, %0"
                             : "=x"(res_asm) : "x"(tc->a256), "m"(tc->b256)); 
                break;
            default:
                printf("❌ Invalid imm8 value: 0x%02X\n", tc->imm8);
                res_asm = _mm256_setzero_si256();
        }
        
        // 256位参考实现（分成两个128位操作）
        __m128i a_low = _mm256_extractf128_si256(tc->a256, 0);
        __m128i a_high = _mm256_extractf128_si256(tc->a256, 1);
        __m128i b_low = _mm256_extractf128_si256(tc->b256, 0);
        __m128i b_high = _mm256_extractf128_si256(tc->b256, 1);
        
        __m128i res_low = clmul_reference(a_low, b_low, tc->imm8);
        __m128i res_high = clmul_reference(a_high, b_high, tc->imm8);
        __m256i res_ref = _mm256_set_m128i(res_high, res_low);
        
        // 打印结果
        print_ymm("ASM Result", res_asm);
        print_ymm("Ref Result", res_ref);
        
        // 结果比对
        if (cmp_ymm(res_asm, res_ref)) {
            printf("✅ Test PASSED\n");
            passed_tests++;
        } else {
            printf("❌ Test FAILED\n");
        }
    }
    
    // ================= 测试总结 =================
    printf("\n===== Test Summary =====\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    return (total_tests == passed_tests) ? 0 : 1;
}
