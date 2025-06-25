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
#define TEST_VPSLLD(desc, width, shift_type, shift_val, expected) do { \
    ALIGNED(32) int32_t src_data[8] = { \
        0x00000001, 0x00000002, 0x00000004, 0x00000008, \
        0x00000010, 0x00000020, 0x00000040, 0x00000080 \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    if (shift_type == 'I') { \
        asm volatile ("vpslld %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
    } else if (shift_type == 'R') { \
        asm volatile ("vmovd %1, %%xmm2\n\t" \
                     "vpslld %%xmm2, %2, %0" \
                     : "=x"(dst) \
                     : "r"(shift_val), "x"(src) \
                     : "xmm2"); \
    } else if (shift_type == 'M') { \
        ALIGNED(16) int32_t shift = shift_val; \
        asm volatile ("vmovd %1, %%xmm2\n\t" \
                     "vpslld %%xmm2, %2, %0" \
                     : "=x"(dst) \
                     : "r"(shift), "x"(src) \
                     : "xmm2"); \
    } \
    print_##width("  SRC     ", src); \
    if (shift_type == 'I') printf("  imm     : %d\n", shift_val); \
    if (shift_type == 'R') print_xmm("  COUNT   ", _mm_set1_epi32(shift_val)); \
    if (shift_type == 'M') printf("  mem_shift: %d\n", shift_val); \
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

#define TEST_VPSLLD_BOUNDARY(desc, width, src_val, shift_val, expected, shift_type) do { \
    __m##width##i src = _mm##width##_set1_epi32(src_val); \
    __m##width##i dst; \
    \
    if (shift_type == 'I') { \
        asm volatile ("vpslld %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
    } else if (shift_type == 'R') { \
        __m128i count = _mm_set1_epi32(shift_val); \
        asm volatile ("vmovd %1, %%xmm2\n\t" \
                     "vpslld %%xmm2, %2, %0" \
                     : "=x"(dst) \
                     : "r"(_mm_cvtsi128_si32(count)), "x"(src) \
                     : "xmm2"); \
    } else if (shift_type == 'M') { \
        ALIGNED(16) int32_t shift = shift_val; \
        asm volatile ("vmovd %1, %%xmm2\n\t" \
                     "vpslld %%xmm2, %2, %0" \
                     : "=x"(dst) \
                     : "r"(shift), "x"(src) \
                     : "xmm2"); \
    } \
    \
    print_##width("  SRC     ", src); \
    if (shift_type == 'I') printf("  imm     : %d\n", shift_val); \
    if (shift_type == 'R') print_xmm("  COUNT   ", _mm_set1_epi32(shift_val)); \
    if (shift_type == 'M') printf("  mem_shift: %d\n", shift_val); \
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

// 测试VPSLLD指令
int test_vpslld() {
    int ret = 0;
    const char* test_name = "VPSLLD tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSLLD("VPSLLD xmm, xmm, 1 (imm)", 128, 'I', 1, 
                _mm_setr_epi32(0x00000002, 0x00000004, 0x00000008, 0x00000010));
                
    TEST_VPSLLD("VPSLLD xmm, xmm, xmm (count=2)", 128, 'R', 2,
                _mm_setr_epi32(0x00000004, 0x00000008, 0x00000010, 0x00000020));
                
    TEST_VPSLLD("VPSLLD xmm, xmm, m128 (shift=1)", 128, 'M', 1,
                _mm_setr_epi32(0x00000002, 0x00000004, 0x00000008, 0x00000010));
                
    // 256位测试
    TEST_VPSLLD("VPSLLD ymm, ymm, m128 (shift=2)", 256, 'M', 2,
                _mm256_setr_epi32(0x00000004, 0x00000008, 0x00000010, 0x00000020,
                                  0x00000040, 0x00000080, 0x00000100, 0x00000200));
                
    TEST_VPSLLD("VPSLLD ymm, ymm, 3 (imm)", 256, 'I', 3,
                _mm256_setr_epi32(0x00000008, 0x00000010, 0x00000020, 0x00000040,
                                  0x00000080, 0x00000100, 0x00000200, 0x00000400));
                
    TEST_VPSLLD("VPSLLD ymm, ymm, xmm (count=4)", 256, 'R', 4,
                _mm256_setr_epi32(0x00000010, 0x00000020, 0x00000040, 0x00000080,
                                  0x00000100, 0x00000200, 0x00000400, 0x00000800));
    
    // 边界测试
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 31 (imm)", 128, 0xFFFFFFFF, 31, 
                         _mm_setr_epi32(0x80000000, 0x80000000, 0x80000000, 0x80000000), 'I');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, xmm (count=31)", 128, 0x7FFFFFFF, 31, 
                         _mm_setr_epi32(0x80000000, 0x80000000, 0x80000000, 0x80000000), 'R');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, m128 (shift=31)", 128, 0x80000000, 31, 
                         _mm_setr_epi32(0x00000000, 0x00000000, 0x00000000, 0x00000000), 'M');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 32 (imm)", 128, 0x00000001, 32, 
                         _mm_setzero_si128(), 'I');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 33 (imm)", 128, 0x00000001, 33, 
                         _mm_setzero_si128(), 'I');
    
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 31 (imm)", 256, 0xFFFFFFFF, 31, 
                         _mm256_setr_epi32(0x80000000, 0x80000000, 0x80000000, 0x80000000,
                                           0x80000000, 0x80000000, 0x80000000, 0x80000000), 'I');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, xmm (count=31)", 256, 0x7FFFFFFF, 31, 
                         _mm256_setr_epi32(0x80000000, 0x80000000, 0x80000000, 0x80000000,
                                           0x80000000, 0x80000000, 0x80000000, 0x80000000), 'R');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, m128 (shift=31)", 256, 0x80000000, 31, 
                         _mm256_setr_epi32(0x00000000, 0x00000000, 0x00000000, 0x00000000,
                                           0x00000000, 0x00000000, 0x00000000, 0x00000000), 'M');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 32 (imm)", 256, 0x00000001, 32, 
                         _mm256_setzero_si256(), 'I');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 33 (imm)", 256, 0x00000001, 33, 
                         _mm256_setzero_si256(), 'I');
    
    // 补充缺失的边界测试组合
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 32 (reg)", 128, 0x00000001, 32, 
                         _mm_setzero_si128(), 'R');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 33 (reg)", 128, 0x00000001, 33, 
                         _mm_setzero_si128(), 'R');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 32 (mem)", 128, 0x00000001, 32, 
                         _mm_setzero_si128(), 'M');
    TEST_VPSLLD_BOUNDARY("VPSLLD xmm, xmm, 33 (mem)", 128, 0x00000001, 33, 
                         _mm_setzero_si128(), 'M');
    
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 32 (reg)", 256, 0x00000001, 32, 
                         _mm256_setzero_si256(), 'R');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 33 (reg)", 256, 0x00000001, 33, 
                         _mm256_setzero_si256(), 'R');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 32 (mem)", 256, 0x00000001, 32, 
                         _mm256_setzero_si256(), 'M');
    TEST_VPSLLD_BOUNDARY("VPSLLD ymm, ymm, 33 (mem)", 256, 0x00000001, 33, 
                         _mm256_setzero_si256(), 'M');
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpslld();
}
