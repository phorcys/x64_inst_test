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
#define TEST_VPSLLW(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[16] = { \
        0x0001, 0x0002, 0x0004, 0x0008, \
        0x0010, 0x0020, 0x0040, 0x0080, \
        0x0100, 0x0200, 0x0400, 0x0800, \
        0x1000, 0x2000, 0x4000, 0x8000  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsllw %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试VPSLLW指令
int test_vpsllw() {
    int ret = 0;
    const char* test_name = "VPSLLW tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSLLW("VPSLLW xmm, 1", 128, 1, 
                _mm_setr_epi16(0x0002, 0x0004, 0x0008, 0x0010,
                               0x0020, 0x0040, 0x0080, 0x0100));
                
    TEST_VPSLLW("VPSLLW xmm, 2", 128, 2,
                _mm_setr_epi16(0x0004, 0x0008, 0x0010, 0x0020,
                               0x0040, 0x0080, 0x0100, 0x0200));
                
    // 256位测试
    TEST_VPSLLW("VPSLLW ymm, 3", 256, 3,
                _mm256_setr_epi16(0x0008, 0x0010, 0x0020, 0x0040,
                                  0x0080, 0x0100, 0x0200, 0x0400,
                                  0x0800, 0x1000, 0x2000, 0x4000,
                                  0x8000, 0x0000, 0x0000, 0x0000));
    
    // 边界测试
    TEST_VPSLLW("VPSLLW xmm, 15", 128, 15, 
                _mm_setr_epi16(0x8000, 0x0000, 0x0000, 0x0000,
                               0x0000, 0x0000, 0x0000, 0x0000));
    TEST_VPSLLW("VPSLLW ymm, 16", 256, 16, _mm256_setzero_si256());
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsllw();
}
