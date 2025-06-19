#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSRLW指令测试
void test_vpsrlw() {
    printf("=== Testing VPSRLW (Shift Right Logical Words) ===\n");
    
    // 测试数据（16位值）
    uint16_t src_data[16] = {
        0x1234, 0x7FFF, 0x8000, 0xFEDC,
        0x8765, 0x5555, 0xAAAA, 0xDEAD,
        0x2468, 0x1357, 0x9ABC, 0xDEF0,
        0x1111, 0x2222, 0x3333, 0x4444
    };
    
    // 128位测试 - 立即数移位
    printf("\n--- 128-bit Tests (Immediate Shift) ---\n");
    __m128i src128, dst128;
    uint16_t expected128[8];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 测试不同的移位量（使用宏展开）
    #define TEST_SHIFT(shift) \
        do { \
            asm volatile ("vpsrlw $" #shift ", %1, %0" : "=x"(dst128) : "x"(src128)); \
            printf("Test: VPSRLW xmm, xmm, %d\n", shift); \
            print_xmm("Source", src128); \
            print_xmm("Result", dst128); \
            for (int j = 0; j < 8; j++) { \
                if (shift > 15) { \
                    expected128[j] = 0; \
                } else { \
                    expected128[j] = src_data[j] >> shift; \
                } \
            } \
            __m128i exp128 = _mm_loadu_si128((__m128i*)expected128); \
            printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL"); \
        } while(0)
    
    TEST_SHIFT(1);
    TEST_SHIFT(4);
    TEST_SHIFT(8);
    TEST_SHIFT(15);
    TEST_SHIFT(16);
    
    #undef TEST_SHIFT
    
    // 128位测试 - 寄存器移位
    printf("\n--- 128-bit Tests (Register Shift) ---\n");
    __m128i shift128;
    uint64_t shift_value = 3; // 移位量
    
    // 加载移位量到XMM寄存器
    shift128 = _mm_set1_epi64x(shift_value);
    
    // 执行VPSRLW指令
    asm volatile ("vpsrlw %1, %2, %0" : "=x"(dst128) : "x"(shift128), "x"(src128));
    printf("Test: VPSRLW xmm, xmm, xmm\n");
    print_xmm("Source", src128);
    print_xmm("Shift Count", shift128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int j = 0; j < 8; j++) {
        if (shift_value > 15) {
            expected128[j] = 0;
        } else {
            expected128[j] = src_data[j] >> shift_value;
        }
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试 - 立即数移位
    printf("\n--- 256-bit Tests (Immediate Shift) ---\n");
    __m256i src256, dst256;
    uint16_t expected256[16];
    
    // 加载256位源数据
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    
    // 测试不同的移位量（使用宏展开）
    #define TEST_SHIFT_YMM(shift) \
        do { \
            asm volatile ("vpsrlw $" #shift ", %1, %0" : "=x"(dst256) : "x"(src256)); \
            printf("Test: VPSRLW ymm, ymm, %d\n", shift); \
            print_ymm("Source", src256); \
            print_ymm("Result", dst256); \
            for (int j = 0; j < 16; j++) { \
                if (shift > 15) { \
                    expected256[j] = 0; \
                } else { \
                    expected256[j] = src_data[j] >> shift; \
                } \
            } \
            __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256); \
            printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL"); \
        } while(0)
    
    TEST_SHIFT_YMM(1);
    TEST_SHIFT_YMM(4);
    TEST_SHIFT_YMM(8);
    TEST_SHIFT_YMM(15);
    TEST_SHIFT_YMM(16);
    
    #undef TEST_SHIFT_YMM
    
    // 256位测试 - 寄存器移位
    printf("\n--- 256-bit Tests (Register Shift) ---\n");
    __m128i shift256_low;
    
    // 加载移位量到XMM寄存器（只使用低128位）
    shift256_low = _mm_set1_epi64x(shift_value);
    
    // 执行VPSRLW指令
    asm volatile ("vpsrlw %1, %2, %0" : "=x"(dst256) : "x"(shift256_low), "x"(src256));
    printf("Test: VPSRLW ymm, ymm, xmm\n");
    print_ymm("Source", src256);
    print_xmm("Shift Count", shift256_low);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int j = 0; j < 16; j++) {
        if (shift_value > 15) {
            expected256[j] = 0;
        } else {
            expected256[j] = src_data[j] >> shift_value;
        }
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
}

int main() {
    test_vpsrlw();
    return 0;
}
