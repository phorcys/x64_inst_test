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
#define TEST_VPSRLQ(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0x8000000000000001, 0x4000000000000002, \
        0x2000000000000004, 0x1000000000000008  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsrlq %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试VPSRLQ指令
int test_vpsrlq() {
    int ret = 0;
    const char* test_name = "VPSRLQ tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试 (修正预期值)
    TEST_VPSRLQ("VPSRLQ xmm, 1", 128, 1, 
                _mm_setr_epi64x(0x2000000000000001, 0x4000000000000000));
                
    TEST_VPSRLQ("VPSRLQ xmm, 2", 128, 2,
                _mm_setr_epi64x(0x1000000000000000, 0x2000000000000000));
                
    // 256位测试
    TEST_VPSRLQ("VPSRLQ ymm, 3", 256, 3,
                _mm256_setr_epi64x(0x1000000000000000, 0x0800000000000000,
                                   0x0400000000000000, 0x0200000000000001));
    
    // 边界测试
    TEST_VPSRLQ("VPSRLQ xmm, 63", 128, 63, 
                _mm_setr_epi64x(0x0000000000000000, 0x0000000000000001));
    TEST_VPSRLQ("VPSRLQ ymm, 64", 256, 64, _mm256_setzero_si256());
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsrlq();
}
