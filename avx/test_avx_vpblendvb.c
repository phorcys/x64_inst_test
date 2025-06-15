#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// VPBLENDVB指令测试
void test_vpblendvb() {
    printf("=== Testing VPBLENDVB ===\n");
    
    // 测试数据
    ALIGNED(16) uint8_t src1[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                   0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
    ALIGNED(16) uint8_t src2[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                   0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    ALIGNED(16) uint8_t ctrl[16] = {0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
                                   0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00};
    ALIGNED(16) uint8_t dst[16];
    
    // 加载测试数据到寄存器
    __m128i xmm1 = _mm_load_si128((__m128i*)src1);
    __m128i xmm2 = _mm_load_si128((__m128i*)src2);
    __m128i xmm_ctrl = _mm_load_si128((__m128i*)ctrl);
    
    // 测试1: 交替选择模式
    __m128i res;
    asm volatile (
        "vpblendvb %3, %1, %2, %0"
        : "=x"(res)
        : "x"(xmm2), "x"(xmm1), "x"(xmm_ctrl)
    );
    _mm_store_si128((__m128i*)dst, res);
    uint8_t expected1[16] = {0x01, 0x22, 0x03, 0x44, 0x05, 0x66, 0x07, 0x88,
                            0x09, 0xAA, 0x0B, 0xCC, 0x0D, 0xEE, 0x0F, 0x00};
    printf("Test1 - Alternating blend:\n");
    print_xmm("Expected", _mm_load_si128((__m128i*)expected1));
    print_xmm("Result  ", res);
    printf("Match: %s\n\n", cmp_xmm(res, _mm_load_si128((__m128i*)expected1)) ? "YES" : "NO");
    
    // 测试2: 全选源1模式(控制寄存器全0)
    __m128i zero_ctrl = _mm_setzero_si128();
    asm volatile (
        "vpblendvb %3, %1, %2, %0"
        : "=x"(res)
        : "x"(xmm2), "x"(xmm1), "x"(zero_ctrl)
    );
    _mm_store_si128((__m128i*)dst, res);
    printf("Test2 - All from src1:\n");
    print_xmm("Expected", xmm1);
    print_xmm("Result  ", res);
    printf("Match: %s\n\n", cmp_xmm(res, xmm1) ? "YES" : "NO");
    
    // 测试3: 全选源2模式(控制寄存器全0x80)
    __m128i all_ctrl = _mm_set1_epi8(0x80);
    asm volatile (
        "vpblendvb %3, %1, %2, %0"
        : "=x"(res)
        : "x"(xmm2), "x"(xmm1), "x"(all_ctrl)
    );
    _mm_store_si128((__m128i*)dst, res);
    printf("Test3 - All from src2:\n");
    print_xmm("Expected", xmm2);
    print_xmm("Result  ", res);
    printf("Match: %s\n\n", cmp_xmm(res, xmm2) ? "YES" : "NO");
    
    // 测试4: 寄存器-内存操作
    ALIGNED(16) uint8_t expected4[16] = {0x11, 0x02, 0x33, 0x04, 0x55, 0x06, 0x77, 0x08,
                                       0x99, 0x0A, 0xBB, 0x0C, 0xDD, 0x0E, 0xFF, 0x10};
    __m128i inv_ctrl = _mm_xor_si128(xmm_ctrl, _mm_set1_epi8(0x80));
    asm volatile (
        "vpblendvb %3, %1, %2, %0"
        : "=x"(res)
        : "m"(*(__m128i*)src2), "x"(xmm1), "x"(inv_ctrl)
    );
    _mm_store_si128((__m128i*)dst, res);
    printf("Test4 - Register-Memory blend:\n");
    print_xmm("Expected", _mm_load_si128((__m128i*)expected4));
    print_xmm("Result  ", res);
    printf("Match: %s\n\n", cmp_xmm(res, _mm_load_si128((__m128i*)expected4)) ? "YES" : "NO");
    
    // 测试5: 边界值测试(暂时注释掉，先确保基本功能)
    printf("Test5 - Boundary values blend: SKIPPED FOR NOW\n\n");
}

int main() {
    test_vpblendvb();
    return 0;
}
