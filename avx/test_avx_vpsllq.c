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

// 打印128位向量（64位整数视图）
static void print_m128i_u64(const char* name, __m128i xmm) {
    uint64_t u64[2];
    memcpy(u64, &xmm, 16);
    printf("%s: [0x%016lx, 0x%016lx]\n", name, u64[0], u64[1]);
}

// 打印256位向量（64位整数视图）
static void print_m256i_u64(const char* name, __m256i ymm) {
    uint64_t u64[4];
    memcpy(u64, &ymm, 32);
    printf("%s: [0x%016lx, 0x%016lx, 0x%016lx, 0x%016lx]\n", 
           name, u64[0], u64[1], u64[2], u64[3]);
}

#define print_128 print_m128i_u64
#define print_256 print_m256i_u64
#define cmp_256 cmp_ymm
#define cmp_128 cmp_xmm
#define _mm128_load_si128 _mm_load_si128
#define _mm128_set1_epi32 _mm_set1_epi32

// 测试宏定义 - 128位立即数版本
#define TEST_VPSLLQ_IMM_128(desc, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[2] = { \
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF \
    }; \
    if (strstr(desc, "all ones") == NULL) { \
        src_data[0] = 0x0000000000000001; \
        src_data[1] = 0x0000000000000002; \
    } \
    __m128i src = _mm_load_si128((__m128i*)src_data); \
    __m128i dst; \
    \
    asm volatile ("vpsllq %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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
#define TEST_VPSLLQ_IMM_256(desc, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, \
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF  \
    }; \
    if (strstr(desc, "all ones") == NULL) { \
        src_data[0] = 0x0000000000000001; \
        src_data[1] = 0x0000000000000002; \
        src_data[2] = 0x0000000000000004; \
        src_data[3] = 0x0000000000000008; \
    } \
    __m256i src = _mm256_load_si256((__m256i*)src_data); \
    __m256i dst; \
    \
    asm volatile ("vpsllq %1, %2, %0" : "=x"(dst) : "i"(shift_val), "x"(src)); \
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
#define TEST_VPSLLQ_REG_128(desc, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[2] = { \
        0x0000000000000001, 0x0000000000000002 \
    }; \
    __m128i src = _mm_load_si128((__m128i*)src_data); \
    __m128i dst; \
    ALIGNED(16) uint64_t shift_val_data = shift_val; \
    \
    asm volatile ("vmovq %1, %%xmm2\n\t vpsllq %%xmm2, %2, %0" : "=x"(dst) : "m"(shift_val_data), "x"(src): "xmm2"); \
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
#define TEST_VPSLLQ_REG_256(desc, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0x0000000000000001, 0x0000000000000002, \
        0x0000000000000004, 0x0000000000000008  \
    }; \
    __m256i src = _mm256_load_si256((__m256i*)src_data); \
    __m256i dst; \
    ALIGNED(16) uint64_t shift_val_data = shift_val; \
    \
    asm volatile ("vmovq %1, %%xmm2\n\t vpsllq %%xmm2, %2, %0" : "=x"(dst) : "m"(shift_val_data), "x"(src): "xmm2"); \
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
#define TEST_VPSLLQ_MEM_128(desc, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[2] = { \
        0x0000000000000001, 0x0000000000000002 \
    }; \
    ALIGNED(16) uint64_t shift_data = shift_val; \
    __m128i src = _mm_load_si128((__m128i*)src_data); \
    __m128i dst; \
    \
    asm volatile ("vpsllq %1, %2, %0" : "=x"(dst) : "m"(shift_data), "x"(src)); \
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
#define TEST_VPSLLQ_MEM_256(desc, shift_val, expected) do { \
    ALIGNED(32) uint64_t src_data[4] = { \
        0x0000000000000001, 0x0000000000000002, \
        0x0000000000000004, 0x0000000000000008  \
    }; \
    ALIGNED(16) uint64_t shift_data = shift_val; \
    __m256i src = _mm256_load_si256((__m256i*)src_data); \
    __m256i dst; \
    \
    asm volatile ("vpsllq %1, %2, %0" : "=x"(dst) : "m"(shift_data), "x"(src)); \
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


// 测试VPSLLQ指令
int test_vpsllq() {
    int ret = 0;
    const char* test_name = "VPSLLQ tests";
    
    printf("=== %s ===\n", test_name);
    
    // 128位测试 - 立即数
    TEST_VPSLLQ_IMM_128("VPSLLQ xmm, imm8=0", 0, 
                   _mm_set_epi64x(0x0000000000000002, 0x0000000000000001));
    
    TEST_VPSLLQ_IMM_128("VPSLLQ xmm, imm8=1", 1, 
                   _mm_set_epi64x(0x0000000000000004, 0x0000000000000002));
    // 128位测试 - 寄存器               
    TEST_VPSLLQ_REG_128("VPSLLQ xmm, xmm=1", 1,
                   _mm_set_epi64x(0x0000000000000004, 0x0000000000000002));
    // 128位测试 - 内存
    TEST_VPSLLQ_MEM_128("VPSLLQ xmm, mem=1", 1,
                   _mm_set_epi64x(0x0000000000000004, 0x0000000000000002));
                
    // 256位测试 - 立即数
    TEST_VPSLLQ_IMM_256("VPSLLQ ymm, imm8=0", 0,
                   _mm256_setr_epi64x(0x0000000000000001, 0x0000000000000002,
                                      0x0000000000000004, 0x0000000000000008));
    
    TEST_VPSLLQ_IMM_256("VPSLLQ ymm, imm8=3", 3,
                   _mm256_setr_epi64x(0x0000000000000008, 0x0000000000000010,
                                      0x0000000000000020, 0x0000000000000040));
    // 256位测试 - 寄存器
    TEST_VPSLLQ_REG_256("VPSLLQ ymm, xmm=3", 3,
                   _mm256_setr_epi64x(0x0000000000000008, 0x0000000000000010,
                                      0x0000000000000020, 0x0000000000000040));
    // 256位测试 - 内存
    TEST_VPSLLQ_MEM_256("VPSLLQ ymm, mem=3", 3,
                   _mm256_setr_epi64x(0x0000000000000008, 0x0000000000000010,
                                      0x0000000000000020, 0x0000000000000040));
    
    // 边界测试 - 立即数
    TEST_VPSLLQ_IMM_128("VPSLLQ xmm, imm8=63", 63, 
                   _mm_set_epi64x(0x0000000000000000, 0x8000000000000000));
    // 边界测试 - 寄存器
    TEST_VPSLLQ_REG_128("VPSLLQ xmm, xmm=63", 63,
                   _mm_set_epi64x(0x0000000000000000, 0x8000000000000000));
    // 边界测试 - 内存
    TEST_VPSLLQ_MEM_128("VPSLLQ xmm, mem=63", 63,
                   _mm_set_epi64x(0x0000000000000000, 0x8000000000000000));
    
    // 测试不移位
    TEST_VPSLLQ_IMM_128("VPSLLQ xmm, imm8=0", 0, 
                   _mm_set_epi64x(0x0000000000000002, 0x0000000000000001));
    TEST_VPSLLQ_REG_128("VPSLLQ xmm, xmm=0", 0,
                   _mm_set_epi64x(0x0000000000000002, 0x0000000000000001));
    TEST_VPSLLQ_MEM_128("VPSLLQ xmm, mem=0", 0,
                   _mm_set_epi64x(0x0000000000000002, 0x0000000000000001));
    
    // 测试全1值
    TEST_VPSLLQ_IMM_128("VPSLLQ xmm, imm8=1 (all ones)", 1, 
                   _mm_set_epi64x(0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFE));
    
    // 边界测试 - 立即数
    TEST_VPSLLQ_IMM_256("VPSLLQ ymm, imm8=64", 64, _mm256_setzero_si256());
    // 边界测试 - 寄存器
    TEST_VPSLLQ_REG_256("VPSLLQ ymm, xmm=64", 64, _mm256_setzero_si256());
    // 边界测试 - 内存
    TEST_VPSLLQ_MEM_256("VPSLLQ ymm, mem=64", 64, _mm256_setzero_si256());
    
    // 测试不移位
    TEST_VPSLLQ_IMM_256("VPSLLQ ymm, imm8=0", 0,
                   _mm256_setr_epi64x(0x0000000000000001, 0x0000000000000002,
                                      0x0000000000000004, 0x0000000000000008));
    TEST_VPSLLQ_REG_256("VPSLLQ ymm, xmm=0", 0,
                   _mm256_setr_epi64x(0x0000000000000001, 0x0000000000000002,
                                      0x0000000000000004, 0x0000000000000008));
    TEST_VPSLLQ_MEM_256("VPSLLQ ymm, mem=0", 0,
                   _mm256_setr_epi64x(0x0000000000000001, 0x0000000000000002,
                                      0x0000000000000004, 0x0000000000000008));
    
    // 测试全1值
    TEST_VPSLLQ_IMM_256("VPSLLQ ymm, imm8=1 (all ones)", 1,
                   _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFE,
                                      0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFE));
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpsllq();
}
