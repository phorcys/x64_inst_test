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
#define TEST_VPSRLW(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[16] = { \
        0x8001, 0x4002, 0x2004, 0x1008, \
        0x0810, 0x0420, 0x0240, 0x0180, \
        0x8002, 0x4004, 0x2008, 0x1010, \
        0x0820, 0x0440, 0x0280, 0x0100  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsrlw %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试VPSRLW指令
int test_vpsrlw() {
    int ret = 0;
    const char* test_name = "VPSRLW tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSRLW("VPSRLW xmm, 1", 128, 1, 
                _mm_setr_epi16(0x4000, 0x2001, 0x1002, 0x0804,
                               0x0408, 0x0210, 0x0120, 0x00C0));
                
    TEST_VPSRLW("VPSRLW xmm, 2", 128, 2,
                _mm_setr_epi16(0x2000, 0x1000, 0x0801, 0x0402,
                               0x0204, 0x0108, 0x0090, 0x0060));
                
    // 256位测试
    TEST_VPSRLW("VPSRLW ymm, 3", 256, 3,
                _mm256_setr_epi16(0x1000, 0x0800, 0x0400, 0x0201,
                                  0x0102, 0x0084, 0x0048, 0x0030,
                                  0x4001, 0x2002, 0x1004, 0x0808,
                                  0x0410, 0x0220, 0x0140, 0x0080));
    
    // 边界测试
    TEST_VPSRLW("VPSRLW xmm, 15", 128, 15, 
                _mm_setr_epi16(0x0001, 0x0000, 0x0000, 0x0000,
                               0x0000, 0x0000, 0x0000, 0x0000));
    TEST_VPSRLW("VPSRLW ymm, 16", 256, 16, _mm256_setzero_si256());
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsrlw();
}
