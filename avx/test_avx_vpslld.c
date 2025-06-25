#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试VPSLLD指令
int test_vpslld() {
    int ret = 0;
    const char* test_name = "VPSLLD tests  128-bits";
    
    printf("=== %s ===\n", test_name);
    
    // 测试数据
    ALIGNED(32) int32_t src_data[8] = {
        0x00000001, 0x00000002, 0x00000004, 0x00000008,
        0x00000010, 0x00000020, 0x00000040, 0x00000080
    };
    
    // 测试128位立即数移位
    {
        __m128i src = _mm_load_si128((__m128i*)src_data);
        __m128i dst;
        
        // vpslld xmm, xmm, imm8 (移位1位)
        asm volatile (
            "vpslld $1, %1, %0"
            : "=x"(dst)
            : "x"(src)
        );
        
        __m128i expected = _mm_setr_epi32(
            0x00000002, 0x00000004, 0x00000008, 0x00000010
        );
        print_xmm("  SRC     ", src);
        printf("  imm     : %d\n", 1);
        print_xmm("  Expected", expected);
        print_xmm("  Actual  ", dst);        
        if (!cmp_xmm(dst, expected)) {
            printf("  [FAIL] VPSLLD xmm, xmm, 1\n\n");
            ret = 1;
        } else {
            printf("  [PASS] VPSLLD xmm, xmm, 1\n\n");
        }
    }
    
    // 测试128位寄存器移位
    {
        __m128i src = _mm_load_si128((__m128i*)src_data);
        __m128i count = _mm_set1_epi32(2);
        __m128i dst;
        
        // vpslld xmm, xmm, xmm (使用vmovd加载移位值)
        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpslld %%xmm2, %2, %0"
            : "=x"(dst)
            : "r"(_mm_cvtsi128_si32(count)), "x"(src)
            : "xmm2"
        );
        
        __m128i expected = _mm_setr_epi32(
            0x00000004, 0x00000008, 0x00000010, 0x00000020
        );
        print_xmm("  SRC     ", src);
        print_xmm("  COUNT   ", count);
        print_xmm("  Expected", expected);
        print_xmm("  Actual  ", dst);        
        if (!cmp_xmm(dst, expected)) {
            printf("  [FAIL] VPSLLD xmm, xmm, xmm (count=2)\n\n");
            ret = 1;
        } else {
            printf("  [PASS] VPSLLD xmm, xmm, xmm (count=2)\n\n");
        }
    }
    
    // 测试128位内存操作数移位
    {
        __m128i src = _mm_load_si128((__m128i*)src_data);
        ALIGNED(16) int32_t shift_val = 1;
        __m128i dst;
        
        // vpslld xmm, xmm, m128
        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpslld %%xmm2, %2, %0"
            : "=x"(dst)
            : "r"(shift_val), "x"(src)
            : "xmm2"
        );
        
        __m128i expected = _mm_setr_epi32(
            0x00000002, 0x00000004, 0x00000008, 0x00000010
        );
        
        print_xmm("  SRC     ", src);
        printf("  mem_shift: %d\n", shift_val);
        print_xmm("  Expected", expected);
        print_xmm("  Actual  ", dst);
        if (!cmp_xmm(dst, expected)) {
            printf("  [FAIL] VPSLLD xmm, xmm, m128 (shift=1)\n\n");
            ret = 1;
        } else {
            printf("  [PASS] VPSLLD xmm, xmm, m128 (shift=1)\n\n");
        }
    }

    // 测试256位内存操作数移位
    {
        __m256i src = _mm256_load_si256((__m256i*)src_data);
        ALIGNED(16) int32_t shift_val = 2;
        __m256i dst;
        
        // vpslld ymm, ymm, m128
        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpslld %%xmm2, %2, %0"
            : "=x"(dst)
            : "r"(shift_val), "x"(src)
            : "xmm2"
        );
        
        __m256i expected = _mm256_setr_epi32(
            0x00000004, 0x00000008, 0x00000010, 0x00000020,
            0x00000040, 0x00000080, 0x00000100, 0x00000200
        );
        
        print_ymm("  SRC     ", src);
        printf("  mem_shift: %d\n", shift_val);
        print_ymm("  Expected", expected);
        print_ymm("  Actual  ", dst);
        if (!cmp_ymm(dst, expected)) {
            printf("  [FAIL] VPSLLD ymm, ymm, m128 (shift=2)\n\n");
            ret = 1;
        } else {
            printf("  [PASS] VPSLLD ymm, ymm, m128 (shift=2)\n\n");
        }
    }
    
    // 测试256位立即数移位
    {
        __m256i src = _mm256_load_si256((__m256i*)src_data);
        __m256i dst;
        
        // vpslld ymm, ymm, imm8 (移位3位)
        asm volatile (
            "vpslld $3, %1, %0"
            : "=x"(dst)
            : "x"(src)
        );
        
        __m256i expected = _mm256_setr_epi32(
            0x00000008, 0x00000010, 0x00000020, 0x00000040,
            0x00000080, 0x00000100, 0x00000200, 0x00000400
        );
        
        print_ymm("  SRC     ", src);
        printf("  imm     : %d\n", 3);
        print_ymm("  Expected", expected);
        print_ymm("  Actual  ", dst);
        if (!cmp_ymm(dst, expected)) {
            printf("  [FAIL] VPSLLD ymm, ymm, 3\n\n");
            ret = 1;
        } else {
            printf("  [PASS] VPSLLD ymm, ymm, 3\n\n");
        }
    }
    
    // 测试256位寄存器移位
    {
        __m256i src = _mm256_load_si256((__m256i*)src_data);
        __m128i count = _mm_set1_epi32(4);
        __m256i dst;
        
        // vpslld ymm, ymm, xmm (使用vmovd加载移位值)
        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpslld %%xmm2, %2, %0"
            : "=x"(dst)
            : "r"(_mm_cvtsi128_si32(count)), "x"(src)
            : "xmm2"
        );
        
        __m256i expected = _mm256_setr_epi32(
            0x00000010, 0x00000020, 0x00000040, 0x00000080,
            0x00000100, 0x00000200, 0x00000400, 0x00000800
        );
        
        print_ymm("  SRC     ", src);
        print_xmm("  COUNT   ", count);
        print_ymm("  Expected", expected);
        print_ymm("  Actual  ", dst);
        if (!cmp_ymm(dst, expected)) {
            printf("  [FAIL] VPSLLD ymm, ymm, xmm (count=4)\n\n");
            ret = 1;
        } else {
            printf("  [PASS] VPSLLD ymm, ymm, xmm (count=4)\n\n");
        }
    }
    
    // 测试128位边界情况
    {
        __m128i src = _mm_setr_epi32(0xFFFFFFFF, 0x7FFFFFFF, 0x80000000, 0x00000001);
        ALIGNED(16) int32_t shift_val = 31;
        __m128i count = _mm_set1_epi32(31);
        
        // 测试31位移位 - 立即数
        {
            __m128i dst;
            asm volatile ("vpslld $31, %1, %0" : "=x"(dst) : "x"(src));
            __m128i expected = _mm_setr_epi32(0x80000000, 0x80000000, 0x00000000, 0x80000000);
            print_xmm("  SRC     ", src);
            printf("  imm     : %d\n", 31);
            print_xmm("  Expected", expected);
            print_xmm("  Actual  ", dst);
            if (!cmp_xmm(dst, expected)) {
                printf("  [FAIL] VPSLLD xmm, xmm, 31 (imm)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD xmm, xmm, 31 (imm)\n\n");
            }
        }
        
        // 测试31位移位 - 寄存器
        {
            __m128i dst;
            asm volatile ("vmovd %1, %%xmm2\n\t"
                         "vpslld %%xmm2, %2, %0"
                         : "=x"(dst)
                         : "r"(_mm_cvtsi128_si32(count)), "x"(src)
                         : "xmm2");
            __m128i expected = _mm_setr_epi32(0x80000000, 0x80000000, 0x00000000, 0x80000000);
            print_xmm("  SRC     ", src);
            print_xmm("  COUNT   ", count);
            print_xmm("  Expected", expected);
            print_xmm("  Actual  ", dst);
            if (!cmp_xmm(dst, expected)) {
                printf("  [FAIL] VPSLLD xmm, xmm, xmm (count=31)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD xmm, xmm, xmm (count=31)\n\n");
            }
        }
        
        // 测试31位移位 - 内存
        {
            __m128i dst;
            asm volatile ("vmovd %1, %%xmm2\n\t"
                         "vpslld %%xmm2, %2, %0"
                         : "=x"(dst)
                         : "r"(shift_val), "x"(src)
                         : "xmm2");
            __m128i expected = _mm_setr_epi32(0x80000000, 0x80000000, 0x00000000, 0x80000000);
            print_xmm("  SRC     ", src);
            printf("  mem_shift: %d\n", shift_val);
            print_xmm("  Expected", expected);
            print_xmm("  Actual  ", dst);
            if (!cmp_xmm(dst, expected)) {
                printf("  [FAIL] VPSLLD xmm, xmm, m128 (shift=31)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD xmm, xmm, m128 (shift=31)\n\n");
            }
        }
        
        // 测试32位移位 - 立即数
        {
            __m128i dst;
            asm volatile ("vpslld $32, %1, %0" : "=x"(dst) : "x"(src));
            __m128i expected = _mm_setzero_si128();
            print_xmm("  SRC     ", src);
            printf("  imm     : %d\n", 32);
            print_xmm("  Expected", expected);
            print_xmm("  Actual  ", dst);
            if (!cmp_xmm(dst, expected)) {
                printf("  [FAIL] VPSLLD xmm, xmm, 32 (imm)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD xmm, xmm, 32 (imm)\n\n");
            }
        }
        
        // 测试33位移位 - 立即数
        {
            __m128i dst;
            asm volatile ("vpslld $33, %1, %0" : "=x"(dst) : "x"(src));
            __m128i expected = _mm_setzero_si128();
            print_xmm("  SRC     ", src);
            printf("  imm     : %d\n", 33);
            print_xmm("  Expected", expected);
            print_xmm("  Actual  ", dst);
            if (!cmp_xmm(dst, expected)) {
                printf("  [FAIL] VPSLLD xmm, xmm, 33 (imm)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD xmm, xmm, 33 (imm)\n\n");
            }
        }
    }
    
    // 测试256位边界情况
    {
        __m256i src = _mm256_setr_epi32(
            0xFFFFFFFF, 0x7FFFFFFF, 0x80000000, 0x00000001,
            0xFFFFFFFF, 0x7FFFFFFF, 0x80000000, 0x00000001
        );
        ALIGNED(16) int32_t shift_val = 31;
        __m128i count = _mm_set1_epi32(31);
        
        // 测试31位移位 - 立即数
        {
            __m256i dst;
            asm volatile ("vpslld $31, %1, %0" : "=x"(dst) : "x"(src));
            __m256i expected = _mm256_setr_epi32(
                0x80000000, 0x80000000, 0x00000000, 0x80000000,
                0x80000000, 0x80000000, 0x00000000, 0x80000000
            );
            print_ymm("  SRC     ", src);
            printf("  imm     : %d\n", 31);
            print_ymm("  Expected", expected);
            print_ymm("  Actual  ", dst);
            if (!cmp_ymm(dst, expected)) {
                printf("  [FAIL] VPSLLD ymm, ymm, 31 (imm)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD ymm, ymm, 31 (imm)\n\n");
            }
        }
        
        // 测试31位移位 - 寄存器
        {
            __m256i dst;
            asm volatile ("vmovd %1, %%xmm2\n\t"
                         "vpslld %%xmm2, %2, %0"
                         : "=x"(dst)
                         : "r"(_mm_cvtsi128_si32(count)), "x"(src)
                         : "xmm2");
            __m256i expected = _mm256_setr_epi32(
                0x80000000, 0x80000000, 0x00000000, 0x80000000,
                0x80000000, 0x80000000, 0x00000000, 0x80000000
            );
            print_ymm("  SRC     ", src);
            print_xmm("  COUNT   ", count);
            print_ymm("  Expected", expected);
            print_ymm("  Actual  ", dst);
            if (!cmp_ymm(dst, expected)) {
                printf("  [FAIL] VPSLLD ymm, ymm, xmm (count=31)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD ymm, ymm, xmm (count=31)\n\n");
            }
        }
        
        // 测试31位移位 - 内存
        {
            __m256i dst;
            asm volatile ("vpslld %1, %2, %0"
                         : "=x"(dst)
                         : "m"(shift_val), "x"(src));
            __m256i expected = _mm256_setr_epi32(
                0x80000000, 0x80000000, 0x00000000, 0x80000000,
                0x80000000, 0x80000000, 0x00000000, 0x80000000
            );
            print_ymm("  SRC     ", src);
            printf("  mem_shift: %d\n", shift_val);
            print_ymm("  Expected", expected);
            print_ymm("  Actual  ", dst);
            if (!cmp_ymm(dst, expected)) {
                printf("  [FAIL] VPSLLD ymm, ymm, m128 (shift=31)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD ymm, ymm, m128 (shift=31)\n\n");
            }
        }
        
        // 测试32位移位 - 立即数
        {
            __m256i dst;
            asm volatile ("vpslld $32, %1, %0" : "=x"(dst) : "x"(src));
            __m256i expected = _mm256_setzero_si256();
            print_ymm("  SRC     ", src);
            printf("  imm     : %d\n", 32);
            print_ymm("  Expected", expected);
            print_ymm("  Actual  ", dst);
            if (!cmp_ymm(dst, expected)) {
                printf("  [FAIL] VPSLLD ymm, ymm, 32 (imm)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD ymm, ymm, 32 (imm)\n\n");
            }
        }
        
        // 测试33位移位 - 立即数
        {
            __m256i dst;
            asm volatile ("vpslld $33, %1, %0" : "=x"(dst) : "x"(src));
            __m256i expected = _mm256_setzero_si256();
            print_ymm("  SRC     ", src);
            printf("  imm     : %d\n", 33);
            print_ymm("  Expected", expected);
            print_ymm("  Actual  ", dst);
            if (!cmp_ymm(dst, expected)) {
                printf("  [FAIL] VPSLLD ymm, ymm, 33 (imm)\n\n");
                ret = 1;
            } else {
                printf("  [PASS] VPSLLD ymm, ymm, 33 (imm)\n\n");
            }
        }
    }
    
    printf("=== %s %s ===\n", test_name, ret ? "FAILED" : "PASSED");
    return ret;
}

int main() {
    return test_vpslld();
}
