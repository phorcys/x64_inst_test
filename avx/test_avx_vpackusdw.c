#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPACKUSDW指令测试
void test_vpackusdw() {
    printf("--- Testing VPACKUSDW (Pack with Unsigned Saturation) ---\n");
    
    // 测试128位版本
    {
        printf("\n[128-bit test]\n");
        
        // 寄存器-寄存器测试
        __m128i a = _mm_setr_epi32(0x12345678, 0x9ABCDEF0, 0x7FFFFFFF, 0x80000000);
        __m128i b = _mm_setr_epi32(0x11223344, 0x55667788, 0x0000FFFF, 0xFFFF0000);
        __m128i result;
        
        // 使用内联汇编 - 128位版本
        asm volatile (
            "vpackusdw %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b)
            : "memory"
        );
        
        print_xmm("Input A", a);
        print_xmm("Input B", b);
        print_xmm("Result", result);
        
        // 预期结果 - 考虑饱和处理
        __m128i expected = _mm_setr_epi16(
            0xFFFF, 0x0000, 0xFFFF, 0x0000,
            0xFFFF, 0xFFFF, 0xFFFF, 0x0000
        );
        
        if (cmp_xmm(result, expected)) {
            printf("128-bit reg-reg test PASSED\n");
        } else {
            printf("128-bit reg-reg test FAILED\n");
        }
    }
    
    // 测试256位版本
    {
        printf("\n[256-bit test]\n");
        
        // 寄存器-寄存器测试
        __m256i a = _mm256_setr_epi32(
            0x12345678, 0x9ABCDEF0, 0x7FFFFFFF, 0x80000000,
            0x11223344, 0x55667788, 0x0000FFFF, 0xFFFF0000
        );
        __m256i b = _mm256_setr_epi32(
            0x11223344, 0x55667788, 0x0000FFFF, 0xFFFF0000,
            0x12345678, 0x9ABCDEF0, 0x7FFFFFFF, 0x80000000
        );
        __m256i result;
        
        // 使用内联汇编 - 256位版本
        asm volatile (
            "vpackusdw %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b)
            : "memory"
        );
        
        print_ymm("Input A", a);
        print_ymm("Input B", b);
        print_ymm("Result", result);
        
        // 预期结果 - 考虑饱和处理
        __m256i expected = _mm256_setr_epi16(
            0xFFFF, 0x0000, 0xFFFF, 0x0000,
            0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
            0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
            0xFFFF, 0x0000, 0xFFFF, 0x0000
        );
        
        if (cmp_ymm(result, expected)) {
            printf("256-bit reg-reg test PASSED\n");
        } else {
            printf("256-bit reg-reg test FAILED\n");
        }
    }
    
    // 测试内存操作数版本
    {
        printf("\n[Memory operand test]\n");
        
        // 准备内存操作数
        ALIGNED(16) int32_t mem_data[4] = {
            0x11223344, 0x55667788, 0x0000FFFF, 0xFFFF0000
        };
        
        __m128i a = _mm_setr_epi32(0x12345678, 0x9ABCDEF0, 0x7FFFFFFF, 0x80000000);
        __m128i result;
        
        // 使用内联汇编 - 内存操作数版本
        asm volatile (
            "vpackusdw %[mem], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [mem] "m" (*mem_data)
            : "memory"
        );
        
        print_xmm("Input A", a);
        printf("Memory operand: ");
        for (int i = 0; i < 4; i++) {
            printf("%08x ", mem_data[i]);
        }
        printf("\n");
        print_xmm("Result", result);
        
        // 预期结果 - 考虑饱和处理
        __m128i expected = _mm_setr_epi16(
            0xFFFF, 0x0000, 0xFFFF, 0x0000,
            0xFFFF, 0xFFFF, 0xFFFF, 0x0000
        );
        
        if (cmp_xmm(result, expected)) {
            printf("Memory operand test PASSED\n");
        } else {
            printf("Memory operand test FAILED\n");
        }
    }
}

int main() {
    test_vpackusdw();
    return 0;
}
