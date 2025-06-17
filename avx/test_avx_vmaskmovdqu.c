#include "avx.h"
#include <stdio.h>
#include <string.h>

// VMASKMOVDQU指令测试
void test_vmaskmovdqu() {
    printf("=== Testing VMASKMOVDQU ===\n");
    
    // 测试数据初始化
    uint8_t mem[16] ALIGNED(16) = {0};
    uint8_t expected[16] = {0};
    __m128i xmm0, xmm1;
    // RDI 寄存器隐式指向目标内存
    register void* rdi asm("rdi") = mem;
    
    // 测试1: 全1掩码
    printf("\nTest 1: Full mask (all 1s)\n");
    uint8_t src1[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
    xmm0 = _mm_loadu_si128((__m128i*)src1);
    xmm1 = _mm_set1_epi8(0xFF); // 全1掩码
    
    memset(mem, 0, sizeof(mem));
    memset(expected, 0, sizeof(expected));
    memcpy(expected, src1, 16);
    
    _mm_storeu_si128((__m128i*)mem, _mm_setzero_si128());
    // VMASKMOVDQU 使用隐式 RDI 寄存器指向的目标内存
    asm volatile(
        "vmaskmovdqu %%xmm1, %%xmm0"
        : 
        : "D" (rdi), "x" (xmm0), "x" (xmm1)
        : "memory"
    );
    
    print_xmm("XMM0 (src)", xmm0);
    print_xmm("XMM1 (mask)", xmm1);
    printf("Memory after: ");
    for(int i=0; i<16; i++) printf("%02X ", mem[i]);
    printf("\n");
    
    if(memcmp(mem, expected, 16) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 交替掩码
    printf("\nTest 2: Alternating mask\n");
    uint8_t src2[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    uint8_t mask2[16] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
                        0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    xmm0 = _mm_loadu_si128((__m128i*)src2);
    xmm1 = _mm_loadu_si128((__m128i*)mask2);
    
    memset(mem, 0, sizeof(mem));
    memset(expected, 0, sizeof(expected));
    for(int i=0; i<16; i++) {
        if(mask2[i]) expected[i] = src2[i];
    }
    
    _mm_storeu_si128((__m128i*)mem, _mm_setzero_si128());
    // VMASKMOVDQU 使用隐式 RDI 寄存器指向的目标内存
    asm volatile(
        "vmaskmovdqu %%xmm1, %%xmm0"
        : 
        : "D" (rdi), "x" (xmm0), "x" (xmm1)
        : "memory"
    );
    
    print_xmm("XMM0 (src)", xmm0);
    print_xmm("XMM1 (mask)", xmm1);
    printf("Memory after: ");
    for(int i=0; i<16; i++) printf("%02X ", mem[i]);
    printf("\n");
    
    if(memcmp(mem, expected, 16) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
    
    // 测试3: 部分掩码
    printf("\nTest 3: Partial mask\n");
    uint8_t src3[16] = {0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
                        0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x80};
    uint8_t mask3[16] = {0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
                        0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00};
    xmm0 = _mm_loadu_si128((__m128i*)src3);
    xmm1 = _mm_loadu_si128((__m128i*)mask3);
    
    memset(mem, 0, sizeof(mem));
    memset(expected, 0, sizeof(expected));
    for(int i=0; i<16; i++) {
        if(mask3[i] & 0x80) expected[i] = src3[i];
    }
    
    _mm_storeu_si128((__m128i*)mem, _mm_setzero_si128());
    // VMASKMOVDQU 使用隐式 RDI 寄存器指向的目标内存
    asm volatile(
        "vmaskmovdqu %%xmm1, %%xmm0"
        : 
        : "D" (rdi), "x" (xmm0), "x" (xmm1)
        : "memory"
    );
    
    print_xmm("XMM0 (src)", xmm0);
    print_xmm("XMM1 (mask)", xmm1);
    printf("Memory after: ");
    for(int i=0; i<16; i++) printf("%02X ", mem[i]);
    printf("\n");
    
    if(memcmp(mem, expected, 16) == 0) {
        printf("Test 3 PASSED\n");
    } else {
        printf("Test 3 FAILED\n");
    }
}

int main() {
    test_vmaskmovdqu();
    return 0;
}
