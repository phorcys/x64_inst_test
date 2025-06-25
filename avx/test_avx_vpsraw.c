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

// 测试宏定义 (修正源数据初始化)
#define TEST_VPSRAW(desc, width, shift_val, expected) do { \
    ALIGNED(32) int16_t src_data[16]; \
    const int16_t src_values[16] = { \
        0x0001, 0x8002, 0x4004, 0xC008, \
        0x2010, 0xA020, 0x1040, 0x9080, \
        0x0801, 0x8402, 0x4204, 0xC108, \
        0x2110, 0xA220, 0x1440, 0x9880  \
    }; \
    memcpy(src_data, src_values, sizeof(src_data)); \
    __m##width##i src = _mm##width##_load_si##width((__m##width##i*)src_data); \
    __m##width##i dst; \
    \
    asm volatile ("vpsraw %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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

// 测试VPSRAW指令
int test_vpsraw() {
    int ret = 0;
    const char* test_name = "VPSRAW tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试
    TEST_VPSRAW("VPSRAW xmm, 1", 128, 1, 
                _mm_setr_epi16(0x0000, 0xC001, 0x2002, 0xE004,
                               0x1008, 0xD010, 0x0820, 0xC840));
                
    TEST_VPSRAW("VPSRAW xmm, 2", 128, 2,
                _mm_setr_epi16(0x0000, 0xE000, 0x1001, 0xF002,
                               0x0804, 0xE808, 0x0410, 0xE420));
                
    // 256位测试 (修正预期值)
    TEST_VPSRAW("VPSRAW ymm, 3", 256, 3,
                _mm256_setr_epi16(0x0000, 0xF000, 0x0800, 0xF801,
                                  0x0402, 0xF404, 0x0208, 0xF210,
                                  0x0100, 0xF080, 0x0840, 0xF821,
                                  0x0422, 0xF444, 0x0288, 0xF310));
    
    // 边界测试
    TEST_VPSRAW("VPSRAW xmm, 15", 128, 15, 
                _mm_setr_epi16(0x0000, 0xFFFF, 0x0000, 0xFFFF,
                               0x0000, 0xFFFF, 0x0000, 0xFFFF));
    TEST_VPSRAW("VPSRAW ymm, 16", 256, 16, 
                _mm256_setr_epi16(0x0000, 0xFFFF, 0x0000, 0xFFFF,
                                  0x0000, 0xFFFF, 0x0000, 0xFFFF,
                                  0x0000, 0xFFFF, 0x0000, 0xFFFF,
                                  0x0000, 0xFFFF, 0x0000, 0xFFFF));
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsraw();
}
