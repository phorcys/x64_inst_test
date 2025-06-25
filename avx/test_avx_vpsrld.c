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
#define TEST_VPSRLD(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint32_t src_data[8] = { \
        0x80000001, 0x40000002, 0x20000004, 0x10000008, \
        0x08000010, 0x04000020, 0x02000040, 0x01000080  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsrld %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试VPSRLD指令
int test_vpsrld() {
    int ret = 0;
    const char* test_name = "VPSRLD tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSRLD("VPSRLD xmm, 1", 128, 1, 
                _mm_setr_epi32(0x40000000, 0x20000001, 
                               0x10000002, 0x08000004));
                
    TEST_VPSRLD("VPSRLD xmm, 2", 128, 2,
                _mm_setr_epi32(0x20000000, 0x10000000,
                               0x08000001, 0x04000002));
                
    // 256位测试
    TEST_VPSRLD("VPSRLD ymm, 3", 256, 3,
                _mm256_setr_epi32(0x10000000, 0x08000000,
                                  0x04000000, 0x02000001,
                                  0x01000002, 0x00800004,
                                  0x00400008, 0x00200010));
    
    // 边界测试
    TEST_VPSRLD("VPSRLD xmm, 31", 128, 31, 
                _mm_setr_epi32(0x00000001, 0x00000000,
                               0x00000000, 0x00000000));
    TEST_VPSRLD("VPSRLD ymm, 32", 256, 32, 
                _mm256_setzero_si256());
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsrld();
}
