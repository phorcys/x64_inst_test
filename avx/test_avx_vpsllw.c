#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 打印128位向量的原始字节（小端顺序）
static void print_xmm_hex(__m128i xmm) {
    uint8_t buf[16];
    memcpy(buf, &xmm, 16);
    for(int i=0; i<16; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印256位向量的原始字节（小端顺序）
static void print_ymm_hex(__m256i ymm) {
    uint8_t buf[32];
    memcpy(buf, &ymm, 32);
    for(int i=0; i<32; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印128位向量（16位整数视图）
static void print_m128i_u16(const char* name, __m128i xmm) {
    uint16_t u16[8];
    memcpy(u16, &xmm, 16);
    printf("%s: [0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x]\n", 
           name, u16[0], u16[1], u16[2], u16[3], u16[4], u16[5], u16[6], u16[7]);
}

// 打印256位向量（16位整数视图）
static void print_m256i_u16(const char* name, __m256i ymm) {
    uint16_t u16[16];
    memcpy(u16, &ymm, 32);
    printf("%s: [0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x,\n"
           "      0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x]\n", 
           name, u16[0], u16[1], u16[2], u16[3], u16[4], u16[5], u16[6], u16[7],
           u16[8], u16[9], u16[10], u16[11], u16[12], u16[13], u16[14], u16[15]);
}

#define print_128 print_m128i_u16
#define print_256 print_m256i_u16
#define cmp_256 cmp_ymm
#define cmp_128 cmp_xmm
#define _mm128_load_si128 _mm_load_si128
#define _mm128_set1_epi32 _mm_set1_epi32

// 测试宏定义 - 128位立即数版本
#define TEST_VPSLLW_IMM_128(desc, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[8] = { \
        0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, \
        0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF  \
    }; \
    if (strstr(desc, "all ones") == NULL) { \
        src_data[0] = 0x0001; \
        src_data[1] = 0x0002; \
        src_data[2] = 0x0004; \
        src_data[3] = 0x0008; \
        src_data[4] = 0x0010; \
        src_data[5] = 0x0020; \
        src_data[6] = 0x0040; \
        src_data[7] = 0x8000; \
    } \
    __m128i src = _mm_load_si128((__m128i*)src_data); \
    __m128i dst; \
    \
    asm volatile ("vpsllw %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
    \
    print_128("  SRC     ", src); \
    printf("  imm     : %d\n", shift_val); \
    print_128("  Expected", expected); \
    print_128("  Actual  ", dst); \
    printf("  Expected (raw): "); \
    print_xmm_hex(expected); \
    printf("  Actual   (raw): "); \
    print_xmm_hex(dst); \
    \
    if (!cmp_128(dst, expected)) { \
        printf("  [FAIL] %s\n\n", desc); \
        ret = 1; \
    } else { \
        printf("  [PASS] %s\n\n", desc); \
    } \
} while(0)

// 测试宏定义 - 256位立即数版本
#define TEST_VPSLLW_IMM_256(desc, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[16] = { \
        0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, \
        0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, \
        0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, \
        0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF  \
    }; \
    if (strstr(desc, "all ones") == NULL) { \
        src_data[0] = 0x0001; \
        src_data[1] = 0x0002; \
        src_data[2] = 0x0004; \
        src_data[3] = 0x0008; \
        src_data[4] = 0x0010; \
        src_data[5] = 0x0020; \
        src_data[6] = 0x0040; \
        src_data[7] = 0x0080; \
        src_data[8] = 0x0100; \
        src_data[9] = 0x0200; \
        src_data[10] = 0x0400; \
        src_data[11] = 0x0800; \
        src_data[12] = 0x1000; \
        src_data[13] = 0x2000; \
        src_data[14] = 0x4000; \
        src_data[15] = 0x8000; \
    } \
    __m256i src = _mm256_load_si256((__m256i*)src_data); \
    __m256i dst; \
    \
    asm volatile ("vpsllw %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
    \
    print_256("  SRC     ", src); \
    printf("  imm     : %d\n", shift_val); \
    print_256("  Expected", expected); \
    print_256("  Actual  ", dst); \
    printf("  Expected (raw): "); \
    print_ymm_hex(expected); \
    printf("  Actual   (raw): "); \
    print_ymm_hex(dst); \
    \
    if (!cmp_256(dst, expected)) { \
        printf("  [FAIL] %s\n\n", desc); \
        ret = 1; \
    } else { \
        printf("  [PASS] %s\n\n", desc); \
    } \
} while(0)

// 测试宏定义 - 128位寄存器版本  
#define TEST_VPSLLW_REG_128(desc, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[8] = { \
        0x0001, 0x0002, 0x0004, 0x0008, \
        0x0010, 0x0020, 0x0040, 0x8000  \
    }; \
    __m128i src = _mm_load_si128((__m128i*)src_data); \
    __m128i dst; \
    ALIGNED(16) uint64_t shift_val_data = shift_val; \
    \
    asm volatile ("vmovq %1, %%xmm2\n\t vpsllw %%xmm2, %2, %0" : "=x"(dst) : "m"(shift_val_data), "x"(src): "xmm2"); \
    \
    print_128("  SRC     ", src); \
    printf("  COUNT   : %d\n", shift_val); \
    print_128("  Expected", expected); \
    print_128("  Actual  ", dst); \
    printf("  Expected (raw): "); \
    print_xmm_hex(expected); \
    printf("  Actual   (raw): "); \
    print_xmm_hex(dst); \
    \
    if (!cmp_128(dst, expected)) { \
        printf("  [FAIL] %s\n\n", desc); \
        ret = 1; \
    } else { \
        printf("  [PASS] %s\n\n", desc); \
    } \
} while(0)

// 测试宏定义 - 256位寄存器版本  
#define TEST_VPSLLW_REG_256(desc, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[16] = { \
        0x0001, 0x0002, 0x0004, 0x0008, \
        0x0010, 0x0020, 0x0040, 0x0080, \
        0x0100, 0x0200, 0x0400, 0x0800, \
        0x1000, 0x2000, 0x4000, 0x8000  \
    }; \
    __m256i src = _mm256_load_si256((__m256i*)src_data); \
    __m256i dst; \
    ALIGNED(16) uint64_t shift_val_data = shift_val; \
    \
    asm volatile ("vmovq %1, %%xmm2\n\t vpsllw %%xmm2, %2, %0" : "=x"(dst) : "m"(shift_val_data), "x"(src): "xmm2"); \
    \
    print_256("  SRC     ", src); \
    printf("  COUNT   : %d\n", shift_val); \
    print_256("  Expected", expected); \
    print_256("  Actual  ", dst); \
    printf("  Expected (raw): "); \
    print_ymm_hex(expected); \
    printf("  Actual   (raw): "); \
    print_ymm_hex(dst); \
    \
    if (!cmp_256(dst, expected)) { \
        printf("  [FAIL] %s\n\n", desc); \
        ret = 1; \
    } else { \
        printf("  [PASS] %s\n\n", desc); \
    } \
} while(0)

// 测试宏定义 - 128位内存版本
#define TEST_VPSLLW_MEM_128(desc, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[8] = { \
        0x0001, 0x0002, 0x0004, 0x0008, \
        0x0010, 0x0020, 0x0040, 0x8000  \
    }; \
    ALIGNED(16) uint64_t shift_data = shift_val; \
    __m128i src = _mm_load_si128((__m128i*)src_data); \
    __m128i dst; \
    \
    asm volatile ("vpsllw %1, %2, %0" : "=x"(dst) : "m"(shift_data), "x"(src)); \
    \
    print_128("  SRC     ", src); \
    printf("  mem_shift: %d\n", shift_val); \
    print_128("  Expected", expected); \
    print_128("  Actual  ", dst); \
    printf("  Expected (raw): "); \
    print_xmm_hex(expected); \
    printf("  Actual   (raw): "); \
    print_xmm_hex(dst); \
    \
    if (!cmp_128(dst, expected)) { \
        printf("  [FAIL] %s\n\n", desc); \
        ret = 1; \
    } else { \
        printf("  [PASS] %s\n\n", desc); \
    } \
} while(0)

// 测试宏定义 - 256位内存版本
#define TEST_VPSLLW_MEM_256(desc, shift_val, expected) do { \
    ALIGNED(32) uint16_t src_data[16] = { \
        0x0001, 0x0002, 0x0004, 0x0008, \
        0x0010, 0x0020, 0x0040, 0x0080, \
        0x0100, 0x0200, 0x0400, 0x0800, \
        0x1000, 0x2000, 0x4000, 0x8000  \
    }; \
    ALIGNED(16) uint64_t shift_data = shift_val; \
    __m256i src = _mm256_load_si256((__m256i*)src_data); \
    __m256i dst; \
    \
    asm volatile ("vpsllw %1, %2, %0" : "=x"(dst) : "m"(shift_data), "x"(src)); \
    \
    print_256("  SRC     ", src); \
    printf("  mem_shift: %d\n", shift_val); \
    print_256("  Expected", expected); \
    print_256("  Actual  ", dst); \
    printf("  Expected (raw): "); \
    print_ymm_hex(expected); \
    printf("  Actual   (raw): "); \
    print_ymm_hex(dst); \
    \
    if (!cmp_256(dst, expected)) { \
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
    
    // 128位测试 - 立即数
    TEST_VPSLLW_IMM_128("VPSLLW xmm, imm8=0", 0, 
                   _mm_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                  0x0010, 0x0020, 0x0040, 0x8000));
    
    TEST_VPSLLW_IMM_128("VPSLLW xmm, imm8=1", 1, 
                   _mm_setr_epi16(0x0002, 0x0004, 0x0008, 0x0010,
                                  0x0020, 0x0040, 0x0080, 0x0000));
    // 128位测试 - 寄存器               
    TEST_VPSLLW_REG_128("VPSLLW xmm, xmm=1", 1,
                   _mm_setr_epi16(0x0002, 0x0004, 0x0008, 0x0010,
                                  0x0020, 0x0040, 0x0080, 0x0000));
    // 128位测试 - 内存
    TEST_VPSLLW_MEM_128("VPSLLW xmm, mem=1", 1,
                   _mm_setr_epi16(0x0002, 0x0004, 0x0008, 0x0010,
                                  0x0020, 0x0040, 0x0080, 0x0000));
                
    // 256位测试 - 立即数
    TEST_VPSLLW_IMM_256("VPSLLW ymm, imm8=0", 0,
                   _mm256_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                     0x0010, 0x0020, 0x0040, 0x0080,
                                     0x0100, 0x0200, 0x0400, 0x0800,
                                     0x1000, 0x2000, 0x4000, 0x8000));
    
    TEST_VPSLLW_IMM_256("VPSLLW ymm, imm8=3", 3,
                   _mm256_setr_epi16(0x0008, 0x0010, 0x0020, 0x0040,
                                     0x0080, 0x0100, 0x0200, 0x0400,
                                     0x0800, 0x1000, 0x2000, 0x4000,
                                     0x8000, 0x0000, 0x0000, 0x0000));
    // 256位测试 - 寄存器
    TEST_VPSLLW_REG_256("VPSLLW ymm, xmm=3", 3,
                   _mm256_setr_epi16(0x0008, 0x0010, 0x0020, 0x0040,
                                     0x0080, 0x0100, 0x0200, 0x0400,
                                     0x0800, 0x1000, 0x2000, 0x4000,
                                     0x8000, 0x0000, 0x0000, 0x0000));
    // 256位测试 - 内存
    TEST_VPSLLW_MEM_256("VPSLLW ymm, mem=3", 3,
                   _mm256_setr_epi16(0x0008, 0x0010, 0x0020, 0x0040,
                                     0x0080, 0x0100, 0x0200, 0x0400,
                                     0x0800, 0x1000, 0x2000, 0x4000,
                                     0x8000, 0x0000, 0x0000, 0x0000));
    
    // 边界测试 - 立即数
    TEST_VPSLLW_IMM_128("VPSLLW xmm, imm8=15", 15, 
                   _mm_setr_epi16(0x8000, 0x0000, 0x0000, 0x0000,
                                  0x0000, 0x0000, 0x0000, 0x0000));
    // 边界测试 - 寄存器
    TEST_VPSLLW_REG_128("VPSLLW xmm, xmm=15", 15,
                   _mm_setr_epi16(0x8000, 0x0000, 0x0000, 0x0000,
                                  0x0000, 0x0000, 0x0000, 0x0000));
    // 边界测试 - 内存
    TEST_VPSLLW_MEM_128("VPSLLW xmm, mem=15", 15,
                   _mm_setr_epi16(0x8000, 0x0000, 0x0000, 0x0000,
                                  0x0000, 0x0000, 0x0000, 0x0000));
    
    // 测试不移位
    TEST_VPSLLW_IMM_128("VPSLLW xmm, imm8=0", 0, 
                   _mm_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                  0x0010, 0x0020, 0x0040, 0x8000));
    TEST_VPSLLW_REG_128("VPSLLW xmm, xmm=0", 0,
                   _mm_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                  0x0010, 0x0020, 0x0040, 0x8000));
    TEST_VPSLLW_MEM_128("VPSLLW xmm, mem=0", 0,
                   _mm_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                  0x0010, 0x0020, 0x0040, 0x8000));
    
    // 测试全1值
    TEST_VPSLLW_IMM_128("VPSLLW xmm, imm8=1 (all ones)", 1, 
                   _mm_setr_epi16(0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
                                  0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE));
    
    // 边界测试 - 立即数
    TEST_VPSLLW_IMM_256("VPSLLW ymm, imm8=16", 16, _mm256_setzero_si256());
    // 边界测试 - 寄存器
    TEST_VPSLLW_REG_256("VPSLLW ymm, xmm=16", 16, _mm256_setzero_si256());
    // 边界测试 - 内存
    TEST_VPSLLW_MEM_256("VPSLLW ymm, mem=16", 16, _mm256_setzero_si256());
    
    // 测试不移位
    TEST_VPSLLW_IMM_256("VPSLLW ymm, imm8=0", 0,
                   _mm256_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                     0x0010, 0x0020, 0x0040, 0x0080,
                                     0x0100, 0x0200, 0x0400, 0x0800,
                                     0x1000, 0x2000, 0x4000, 0x8000));
    TEST_VPSLLW_REG_256("VPSLLW ymm, xmm=0", 0,
                   _mm256_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                     0x0010, 0x0020, 0x0040, 0x0080,
                                     0x0100, 0x0200, 0x0400, 0x0800,
                                     0x1000, 0x2000, 0x4000, 0x8000));
    TEST_VPSLLW_MEM_256("VPSLLW ymm, mem=0", 0,
                   _mm256_setr_epi16(0x0001, 0x0002, 0x0004, 0x0008,
                                     0x0010, 0x0020, 0x0040, 0x0080,
                                     0x0100, 0x0200, 0x0400, 0x0800,
                                     0x1000, 0x2000, 0x4000, 0x8000));
    
    // 测试全1值
    TEST_VPSLLW_IMM_256("VPSLLW ymm, imm8=1 (all ones)", 1,
                   _mm256_setr_epi16(0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
                                     0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
                                     0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
                                     0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE));
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsllw();
}
