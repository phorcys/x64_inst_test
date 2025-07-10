#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include "avx.h"

// 测试VPSRLW指令
void test_vpsrlw() {
    printf("----- Testing VPSRLW -----\n");
    
    // 测试数据
    __m128i a128 = _mm_setr_epi16(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x0000, 0x1234);
    __m128i b128 = _mm_setr_epi16(0x8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4);
    __m128i res128;
    
    // 使用正确的AVX2函数：向量移位
    res128 = _mm_srl_epi16(a128, b128);
    
    printf("[128-bit Reg-Reg]\n");
    print_m128i_hex(a128, "Input");
    print_m128i_hex(b128, "Shift");
    print_m128i_hex(res128, "Result");
    printf("\n");
    
    // 测试寄存器-内存操作
    __m128i mem_shift = _mm_set1_epi16(4);
    res128 = _mm_srl_epi16(a128, mem_shift);
    
    printf("[128-bit Reg-Mem]\n");
    print_m128i_hex(a128, "Input");
    printf("Shift: 4 (memory)\n");
    print_m128i_hex(res128, "Result");
    printf("\n");
    
    // 测试立即数操作
    res128 = _mm_srli_epi16(a128, 3);
    
    printf("[128-bit Imm]\n");
    print_m128i_hex(a128, "Input");
    printf("Shift: 3 (immediate)\n");
    print_m128i_hex(res128, "Result");
    printf("\n");
    
    // 256位测试
    __m256i a256 = _mm256_setr_epi16(
        0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 
        0xEEEE, 0xFFFF, 0x0000, 0x1234,
        0x5555, 0x6666, 0x7777, 0x8888,
        0x9999, 0xAAAA, 0xBBBB, 0xCCCC
    );
    __m128i b256 = _mm_setr_epi16(
        5, 0, 0, 0, 15, 16, 0, 8
    );
    __m256i res256;
    
    // 使用正确的AVX2函数：向量移位
    res256 = _mm256_srl_epi16(a256, b256);
    
    printf("[256-bit Reg-Reg]\n");
    print_m256i_hex(a256, "Input");
    print_m128i_hex(b256, "Shift");
    print_m256i_hex(res256, "Result");
    printf("\n");
    
    // 边界测试：移位计数16（应得全0）
    res128 = _mm_srli_epi16(a128, 16);
    
    printf("[Boundary]\n");
    print_m128i_hex(a128, "Input");
    printf("Shift: 16 (should be all zeros)\n");
    print_m128i_hex(res128, "Result");
    printf("\n");
    
    printf("VPSRLW tests completed.\n");
}

int main() {
    test_vpsrlw();
    return 0;
}
