#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSLLQ指令测试
void test_vpsllq() {
    printf("=== Testing VPSLLQ (Vector Packed Shift Left Logical Quadword) ===\n");
    
    // 测试数据
    uint64_t src_data[8] = {
        0x0123456789ABCDEF, 0xFEDCBA9876543210,
        0x5555555555555555, 0xAAAAAAAAAAAAAAAA,
        0x0000000000000001, 0x8000000000000000,
        0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, dst128, shift128;
    uint64_t expected128[2];
    
    // 加载128位源数据
    src128 = _mm_loadu_si128((__m128i*)src_data);
    
    // 立即数移位测试（使用宏展开）
    printf("\n--- Immediate Shifts ---\n");
    #define TEST_IMM_SHIFT(shift) \
        do { \
            asm volatile ("vpsllq $" #shift ", %1, %0" : "=x"(dst128) : "x"(src128)); \
            printf("Test: VPSLLQ xmm, xmm, %d\n", shift); \
            print_xmm("Source", src128); \
            print_xmm("Result", dst128); \
            for (int j = 0; j < 2; j++) { \
                if (shift >= 64) { \
                    expected128[j] = 0; \
                } else { \
                    expected128[j] = src_data[j] << shift; \
                } \
            } \
            __m128i exp128 = _mm_loadu_si128((__m128i*)expected128); \
            printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL"); \
        } while(0)
    
    TEST_IMM_SHIFT(0);
    TEST_IMM_SHIFT(1);
    TEST_IMM_SHIFT(63);
    TEST_IMM_SHIFT(64);
    TEST_IMM_SHIFT(65);
    
    #undef TEST_IMM_SHIFT
    
    // 变量移位测试
    printf("\n--- Variable Shifts ---\n");
    uint64_t shift_values[] = {0, 1, 63, 64};
    for (int i = 0; i < sizeof(shift_values)/sizeof(shift_values[0]); i++) {
        uint64_t shift_val = shift_values[i];
        uint64_t shift_data[2] = {shift_val, 0}; // 只使用低64位
        shift128 = _mm_loadu_si128((__m128i*)shift_data);
        
        asm volatile ("vpsllq %1, %2, %0" : "=x"(dst128) : "x"(shift128), "x"(src128));
        printf("Test: VPSLLQ xmm, xmm, xmm (shift=%lu)\n", shift_val);
        print_xmm("Source", src128);
        print_xmm("Shift Counts", shift128);
        print_xmm("Result", dst128);
        
        // 计算预期值
        for (int j = 0; j < 2; j++) {
            if (shift_val >= 64) {
                expected128[j] = 0;
            } else {
                expected128[j] = src_data[j] << shift_val;
            }
        }
        __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
        printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    }
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, dst256;
    __m128i shift256;
    uint64_t expected256[4];
    
    // 加载256位源数据
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    
    // 立即数移位测试（使用宏展开）
    printf("\n--- Immediate Shifts ---\n");
    #define TEST_IMM_SHIFT_YMM(shift) \
        do { \
            asm volatile ("vpsllq $" #shift ", %1, %0" : "=x"(dst256) : "x"(src256)); \
            printf("Test: VPSLLQ ymm, ymm, %d\n", shift); \
            print_ymm("Source", src256); \
            print_ymm("Result", dst256); \
            for (int j = 0; j < 4; j++) { \
                if (shift >= 64) { \
                    expected256[j] = 0; \
                } else { \
                    expected256[j] = src_data[j] << shift; \
                } \
            } \
            __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256); \
            printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL"); \
        } while(0)
    
    TEST_IMM_SHIFT_YMM(0);
    TEST_IMM_SHIFT_YMM(1);
    TEST_IMM_SHIFT_YMM(63);
    TEST_IMM_SHIFT_YMM(64);
    TEST_IMM_SHIFT_YMM(65);
    
    #undef TEST_IMM_SHIFT_YMM
    
    // 变量移位测试
    printf("\n--- Variable Shifts ---\n");
    for (int i = 0; i < sizeof(shift_values)/sizeof(shift_values[0]); i++) {
        uint64_t shift_val = shift_values[i];
        uint64_t shift_data[2] = {shift_val, 0}; // 只使用低64位
        shift256 = _mm_loadu_si128((__m128i*)shift_data);
        
        asm volatile ("vpsllq %1, %2, %0" : "=x"(dst256) : "x"(shift256), "x"(src256));
        printf("Test: VPSLLQ ymm, ymm, xmm (shift=%lu)\n", shift_val);
        print_ymm("Source", src256);
        print_xmm("Shift Counts", shift256);
        print_ymm("Result", dst256);
        
        // 计算预期值
        for (int j = 0; j < 4; j++) {
            if (shift_val >= 64) {
                expected256[j] = 0;
            } else {
                expected256[j] = src_data[j] << shift_val;
            }
        }
        __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
        printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    }
}

int main() {
    test_vpsllq();
    return 0;
}
