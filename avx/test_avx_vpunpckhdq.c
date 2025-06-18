#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// 打印128位整数向量
static void print_m128i(const char* name, __m128i vec) {
    uint32_t *vals = (uint32_t*)&vec;
    printf("%s: [%u, %u, %u, %u]\n", name, vals[0], vals[1], vals[2], vals[3]);
}

// 打印256位整数向量
static void print_m256i(const char* name, __m256i vec) {
    uint32_t *vals = (uint32_t*)&vec;
    printf("%s: [%u, %u, %u, %u, %u, %u, %u, %u]\n", name, 
           vals[0], vals[1], vals[2], vals[3],
           vals[4], vals[5], vals[6], vals[7]);
}

// 主测试函数
int main() {
    int errors = 0;
    
    printf("=== Testing vpunpckhdq ===\n");
    
    // 测试128位寄存器-寄存器操作
    printf("\n[Test 1: 128-bit reg-reg]\n");
    {
        __m128i a = _mm_setr_epi32(0xA1A2A3A4, 0xB1B2B3B4, 0xC1C2C3C4, 0xD1D2D3D4);
        __m128i b = _mm_setr_epi32(0xE1E2E3E4, 0xF1F2F3F4, 0x01020304, 0x11121314);
        __m128i c;
        
        CLEAR_FLAGS;
        
        // 内联汇编实现vpunpckhdq
        asm volatile (
            "vpunpckhdq %2, %1, %0"
            : "=x"(c)
            : "x"(a), "x"(b)
        );
        
        // 预期结果：高64位交错 [C1C2C3C4, 01020304, D1D2D3D4, 11121314]
        __m128i expected = _mm_setr_epi32(0xC1C2C3C4, 0x01020304, 0xD1D2D3D4, 0x11121314);
        
        print_m128i("Input A", a);
        print_m128i("Input B", b);
        print_m128i("Result", c);
        print_m128i("Expected", expected);
        
        if (!cmp_xmm(c, expected)) {
            printf("Mismatch!\n");
            errors++;
        }
        
        uint64_t eflags = 0;
        asm volatile ("pushfq; pop %0" : "=r"(eflags));
        print_eflags_cond(eflags, 0);
    }
    
    // 测试128位寄存器-内存操作
    printf("\n[Test 2: 128-bit reg-mem]\n");
    {
        __m128i a = _mm_setr_epi32(0xFFFFFFFF, 0x00000000, 0x80000000, 0x7FFFFFFF);
        ALIGNED(16) uint32_t mem[4] = {0x55555555, 0xAAAAAAAA, 0x12345678, 0x9ABCDEF0};
        __m128i c;
        
        CLEAR_FLAGS;
        
        asm volatile (
            "vpunpckhdq %1, %2, %0"
            : "=x"(c)
            : "m"(*mem), "x"(a)
        );
        
        // 预期结果：高64位交错 [80000000, 12345678, 7FFFFFFF, 9ABCDEF0]
        __m128i expected = _mm_setr_epi32(0x80000000, 0x12345678, 0x7FFFFFFF, 0x9ABCDEF0);
        
        print_m128i("Input A", a);
        printf("Memory: [0x%X, 0x%X, 0x%X, 0x%X]\n", mem[0], mem[1], mem[2], mem[3]);
        print_m128i("Result", c);
        print_m128i("Expected", expected);
        
        if (!cmp_xmm(c, expected)) {
            printf("Mismatch!\n");
            errors++;
        }
        
        uint64_t eflags = 0;
        asm volatile ("pushfq; pop %0" : "=r"(eflags));
        print_eflags_cond(eflags, 0);
    }
    
    // 测试256位寄存器-寄存器操作
    printf("\n[Test 3: 256-bit reg-reg]\n");
    {
        __m256i a = _mm256_setr_epi32(
            0x11111111, 0x22222222, 0x33333333, 0x44444444,
            0x55555555, 0x66666666, 0x77777777, 0x88888888
        );
        __m256i b = _mm256_setr_epi32(
            0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC,
            0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x00000000
        );
        __m256i c;
        
        CLEAR_FLAGS;
        
        asm volatile (
            "vpunpckhdq %2, %1, %0"
            : "=x"(c)
            : "x"(a), "x"(b)
        );
        
        // 预期结果：高128位交错 [33333333, BBBBBBBB, 44444444, CCCCCCCC, 77777777, FFFFFFFF, 88888888, 00000000]
        __m256i expected = _mm256_setr_epi32(
            0x33333333, 0xBBBBBBBB, 0x44444444, 0xCCCCCCCC,
            0x77777777, 0xFFFFFFFF, 0x88888888, 0x00000000
        );
        
        print_m256i("Input A", a);
        print_m256i("Input B", b);
        print_m256i("Result", c);
        print_m256i("Expected", expected);
        
        if (!cmp_ymm(c, expected)) {
            printf("Mismatch!\n");
            errors++;
        }
        
        uint64_t eflags = 0;
        asm volatile ("pushfq; pop %0" : "=r"(eflags));
        print_eflags_cond(eflags, 0);
    }
    
    // 测试256位寄存器-内存操作
    printf("\n[Test 4: 256-bit reg-mem]\n");
    {
        __m256i a = _mm256_setr_epi32(
            0x00000000, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000,
            0x12345678, 0x9ABCDEF0, 0x55555555, 0xAAAAAAAA
        );
        ALIGNED(32) uint32_t mem[8] = {
            0x11111111, 0x22222222, 0x33333333, 0x44444444,
            0x55555555, 0x66666666, 0x77777777, 0x88888888
        };
        __m256i c;
        
        CLEAR_FLAGS;
        
        asm volatile (
            "vpunpckhdq %1, %2, %0"
            : "=x"(c)
            : "m"(*mem), "x"(a)
        );
        
        // 预期结果：高128位交错 [7FFFFFFF, 33333333, 80000000, 44444444, 55555555, 77777777, AAAAAAAA, 88888888]
        __m256i expected = _mm256_setr_epi32(
            0x7FFFFFFF, 0x33333333, 0x80000000, 0x44444444,
            0x55555555, 0x77777777, 0xAAAAAAAA, 0x88888888
        );
        
        print_m256i("Input A", a);
        printf("Memory: [");
        for (int i = 0; i < 8; i++) printf("0x%X%s", mem[i], i<7 ? ", " : "");
        printf("]\n");
        print_m256i("Result", c);
        print_m256i("Expected", expected);
        
        if (!cmp_ymm(c, expected)) {
            printf("Mismatch!\n");
            errors++;
        }
        
        uint64_t eflags = 0;
        asm volatile ("pushfq; pop %0" : "=r"(eflags));
        print_eflags_cond(eflags, 0);
    }
    
    printf("\n=== Tests complete: %d errors ===\n", errors);
    return errors ? 1 : 0;
}
