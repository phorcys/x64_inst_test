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

// 测试宏定义 - 立即数版本
#define TEST_VPSLLQ_IMM(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0x0000000000000001, 0x0000000000000002, \
        0x0000000000000004, 0x0000000000000008  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsllq %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试宏定义 - 寄存器版本  
#define TEST_VPSLLQ_REG(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0x0000000000000001, 0x0000000000000002, \
        0x0000000000000004, 0x0000000000000008  \
    }; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vmovd %1, %%xmm2\n\t vpsllq %%xmm2, %2, %0" : "=x"(dst) : "r"(shift_val), "x"(src): "xmm2"); \
    \
    print_##width("  SRC     ", src); \
    printf("  COUNT   : %d\n", shift_val); \
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

// 测试宏定义 - 内存版本
#define TEST_VPSLLQ_MEM(desc, width, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0x0000000000000001, 0x0000000000000002, \
        0x0000000000000004, 0x0000000000000008  \
    }; \
    ALIGNED(16) uint64_t shift_data[2] = {shift_val, shift_val}; \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsllq %1, %2, %0" : "=x"(dst) : "m"(*(__m128i*)shift_data), "x"(src)); \
    \
    print_##width("  SRC     ", src); \
    printf("  mem_shift: %d\n", shift_val); \
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


// 测试VPSLLQ指令
int test_vpsllq() {
    int ret = 0;
    const char* test_name = "VPSLLQ tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试 - 立即数
    TEST_VPSLLQ_IMM("VPSLLQ xmm, imm8=1", 128, 1, 
                   _mm_setr_epi64x(0x0000000000000004, 0x0000000000000002));
    // 128位测试 - 寄存器               
    TEST_VPSLLQ_REG("VPSLLQ xmm, xmm=1", 128, 1,
                   _mm_setr_epi64x(0x0000000000000004, 0x0000000000000002));
    // 128位测试 - 内存
    TEST_VPSLLQ_MEM("VPSLLQ xmm, mem=1", 128, 1,
                   _mm_setr_epi64x(0x0000000000000004, 0x0000000000000002));
                
    // 256位测试 - 立即数
    TEST_VPSLLQ_IMM("VPSLLQ ymm, imm8=3", 256, 3,
                   _mm256_setr_epi64x(0x0000000000000008, 0x0000000000000010,
                                      0x0000000000000020, 0x0000000000000040));
    // 256位测试 - 寄存器
    TEST_VPSLLQ_REG("VPSLLQ ymm, xmm=3", 256, 3,
                   _mm256_setr_epi64x(0x0000000000000008, 0x0000000000000010,
                                      0x0000000000000020, 0x0000000000000040));
    // 256位测试 - 内存
    TEST_VPSLLQ_MEM("VPSLLQ ymm, mem=3", 256, 3,
                   _mm256_setr_epi64x(0x0000000000000008, 0x0000000000000010,
                                      0x0000000000000020, 0x0000000000000040));
    
    // 边界测试 - 立即数
    TEST_VPSLLQ_IMM("VPSLLQ xmm, imm8=63", 128, 63, 
                   _mm_setr_epi64x(0x0000000000000000, 0x8000000000000000));
    // 边界测试 - 寄存器
    TEST_VPSLLQ_REG("VPSLLQ xmm, xmm=63", 128, 63,
                   _mm_setr_epi64x(0x0000000000000000, 0x8000000000000000));
    // 边界测试 - 内存
    TEST_VPSLLQ_MEM("VPSLLQ xmm, mem=63", 128, 63,
                   _mm_setr_epi64x(0x0000000000000000, 0x8000000000000000));
    
    // 边界测试 - 立即数
    TEST_VPSLLQ_IMM("VPSLLQ ymm, imm8=64", 256, 64, _mm256_setzero_si256());
    // 边界测试 - 寄存器
    TEST_VPSLLQ_REG("VPSLLQ ymm, xmm=64", 256, 64, _mm256_setzero_si256());
    // 边界测试 - 内存
    TEST_VPSLLQ_MEM("VPSLLQ ymm, mem=64", 256, 64, _mm256_setzero_si256());
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsllq();
}
