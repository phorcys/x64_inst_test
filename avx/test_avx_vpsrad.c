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
#define TEST_VPSRAD(desc, width, shift_val, expected) do { \
    ALIGNED(32) int32_t src_data[8] = { \
        0x00000001, 0x80000002, 0x40000004, 0xC0000008, \
        0x20000010, 0xA0000020, 0x10000040, 0x90000080  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsrad %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试VPSRAD指令
int test_vpsrad() {
    int ret = 0;
    const char* test_name = "VPSRAD tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSRAD("VPSRAD xmm, 1", 128, 1, 
                _mm_setr_epi32(0x00000000, 0xC0000001, 
                               0x20000002, 0xE0000004));
                
    TEST_VPSRAD("VPSRAD xmm, 2", 128, 2,
                _mm_setr_epi32(0x00000000, 0xE0000000,
                               0x10000001, 0xF0000002));
                
    // 256位测试
    TEST_VPSRAD("VPSRAD ymm, 3", 256, 3,
                _mm256_setr_epi32(0x00000000, 0xF0000000,
                                  0x08000000, 0xF8000001,
                                  0x04000002, 0xF4000004,
                                  0x02000008, 0xF2000010));
    
    // 边界测试
    TEST_VPSRAD("VPSRAD xmm, 31", 128, 31, 
                _mm_setr_epi32(0x00000000, 0xFFFFFFFF,
                               0x00000000, 0xFFFFFFFF));
    TEST_VPSRAD("VPSRAD ymm, 32", 256, 32, 
                _mm256_setr_epi32(0x00000000, 0xFFFFFFFF,
                                  0x00000000, 0xFFFFFFFF,
                                  0x00000000, 0xFFFFFFFF,
                                  0x00000000, 0xFFFFFFFF));
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsrad();
}
