#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define print_256 print_ymm
#define print_128 print_xmm
#define cmp_256 cmp_ymm
#define cmp_128 cmp_xmm
#define _mm128_load_si128 _mm_load_si128
#define _mm128_set1_epi32 _mm_set1_epi32

// 测试宏定义
#define TEST_VPSLLDQ(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint8_t src_data[32] = { \
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, \
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, \
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpslldq %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
    \
    print_##width("  SRC     ", src); \
    printf("  imm     : %d\n", shift_val); \
    print_##width("  Expected", expected); \
    print_##width("  Actual  ", dst); \
    \
    if (!cmp_##width(dst, expected)) { \
        printf("  [FAIL] %s\n\n", desc); \
        ret = 1; \
    } else { \
        printf("  [PASS] %s\n\n", desc); \
    } \
} while(0)

// 测试VPSLLDQ指令
int test_vpslldq() {
    int ret = 0;
    const char* test_name = "VPSLLDQ tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSLLDQ("VPSLLDQ xmm, 1", 128, 1, 
                _mm_setr_epi8(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F));
                
    TEST_VPSLLDQ("VPSLLDQ xmm, 4", 128, 4,
                _mm_setr_epi8(0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04,
                              0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C));
                
    // 256位测试
    TEST_VPSLLDQ("VPSLLDQ ymm, 8", 256, 8,
                _mm256_setr_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18));
    
    // 边界测试
    TEST_VPSLLDQ("VPSLLDQ xmm, 16", 128, 16, _mm_setzero_si128());
    TEST_VPSLLDQ("VPSLLDQ ymm, 32", 256, 32, _mm256_setzero_si256());
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpslldq();
}
