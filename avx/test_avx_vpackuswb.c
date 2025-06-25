#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPACKUSWB指令测试
void test_vpackuswb() {
    printf("--- Testing VPACKUSWB (Pack with Unsigned Saturation) ---\n");
    
    // 测试128位版本
    {
        printf("\n[128-bit test]\n");
        
        // 寄存器-寄存器测试
        __m128i a = _mm_setr_epi16(0x1234, 0x5678, 0x9ABC, 0xDEF0, 
                                  0x7FFF, 0x8000, 0x00FF, 0x0100);
        __m128i b = _mm_setr_epi16(0x1122, 0x3344, 0x5566, 0x7788,
                                  0x00FF, 0x8000, 0x7FFF, 0x0000);
        __m128i result;
        
        // 使用内联汇编 - 简化实现
        asm volatile (
            "vpackuswb %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b)
            : "memory"
        );
        
        print_xmm("Input A", a);
        print_xmm("Input B", b);
        print_xmm("Result", result);
        
        // 预期结果 - 修正为饱和打包后的正确值
        __m128i expected = _mm_setr_epi8(
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00
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
        __m256i a = _mm256_setr_epi16(
            0x1234, 0x5678, 0x9ABC, 0xDEF0, 0x7FFF, 0x8000, 0x00FF, 0x0100,
            0x1122, 0x3344, 0x5566, 0x7788, 0x00FF, 0x8000, 0x7FFF, 0x0000
        );
        __m256i b = _mm256_setr_epi16(
            0x1122, 0x3344, 0x5566, 0x7788, 0x00FF, 0x8000, 0x7FFF, 0x0000,
            0x1234, 0x5678, 0x9ABC, 0xDEF0, 0x7FFF, 0x8000, 0x00FF, 0x0100
        );
        __m256i result;
        
        // 使用内联汇编 - 简化实现
        asm volatile (
            "vpackuswb %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b)
            : "memory"
        );
        
        print_ymm("Input A", a);
        print_ymm("Input B", b);
        print_ymm("Result", result);
        
        // 预期结果 - 修正为饱和打包后的正确值
        __m256i expected = _mm256_setr_epi8(
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF
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
        ALIGNED(16) int16_t mem_data[8] = {
            0x1122, 0x3344, 0x5566, 0x7788, 0x00FF, 0x8000, 0x7FFF, 0x0000
        };
        
        __m128i a = _mm_setr_epi16(0x1234, 0x5678, 0x9ABC, 0xDEF0, 
                                  0x7FFF, 0x8000, 0x00FF, 0x0100);
        __m128i result;
        
        // 使用内联汇编 - 简化实现
        asm volatile (
            "vpackuswb %[mem], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [mem] "m" (*mem_data)
            : "memory"
        );
        
        print_xmm("Input A", a);
        printf("Memory operand: ");
        for (int i = 0; i < 8; i++) {
            printf("%04x ", mem_data[i]);
        }
        printf("\n");
        print_xmm("Result", result);
        
        // 预期结果 - 修正为饱和打包后的正确值
        __m128i expected = _mm_setr_epi8(
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00
        );
        
        if (cmp_xmm(result, expected)) {
            printf("Memory operand test PASSED\n");
        } else {
            printf("Memory operand test FAILED\n");
        }
    }
}

int main() {
    test_vpackuswb();
    return 0;
}
