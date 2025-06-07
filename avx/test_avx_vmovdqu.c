#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// 测试VMOVDQU指令
void test_vmovdqu() {
    int errors = 0;
    printf("Testing VMOVDQU instruction...\n");

    // 测试128位寄存器到寄存器传输
    {
        uint8_t data[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        __m128i src, dst;
        __m128i expected;

        memcpy(&src, data, sizeof(data));
        expected = src;

        // 使用内联汇编实现VMOVDQU
        asm volatile("vmovdqu %1, %0" : "=x"(dst) : "m"(src));
        
        if (!cmp_xmm(dst, expected)) {
            printf("Error in 128-bit reg-to-reg transfer\n");
            print_xmm("Expected", expected);
            print_xmm("Got", dst);
            errors++;
        }
    }

    // 测试256位寄存器到寄存器传输
    {
        uint8_t data[32];
        for (int i = 0; i < 32; i++) data[i] = i * 5;
        
        __m256i src, dst;
        __m256i expected;
        
        memcpy(&src, data, sizeof(data));
        expected = src;

        asm volatile("vmovdqu %1, %0" : "=x"(dst) : "m"(src));
        
        if (!cmp_ymm(dst, expected)) {
            printf("Error in 256-bit reg-to-reg transfer\n");
            print_ymm("Expected", expected);
            print_ymm("Got", dst);
            errors++;
        }
    }

    // 测试128位内存到寄存器传输（未对齐）
    {
        uint8_t mem_data[18] = {0}; // 额外2字节用于创建未对齐地址
        uint8_t* unaligned_ptr = mem_data + 1; // 未对齐地址
        
        for (int i = 0; i < 16; i++) unaligned_ptr[i] = 0xF0 + i;
        
        __m128i dst;
        __m128i expected;
        memcpy(&expected, unaligned_ptr, 16);

        asm volatile("vmovdqu %1, %0" : "=x"(dst) : "m"(*unaligned_ptr));
        
        if (!cmp_xmm(dst, expected)) {
            printf("Error in 128-bit mem-to-reg (unaligned)\n");
            print_xmm("Expected", expected);
            print_xmm("Got", dst);
            errors++;
        }
    }

    // 测试256位内存到寄存器传输（未对齐）
    {
        uint8_t mem_data[34] = {0}; // 额外2字节用于创建未对齐地址
        uint8_t* unaligned_ptr = mem_data + 1; // 未对齐地址
        
        for (int i = 0; i < 32; i++) unaligned_ptr[i] = 0xE0 + i;
        
        __m256i dst;
        __m256i expected;
        memcpy(&expected, unaligned_ptr, 32);

        asm volatile("vmovdqu %1, %0" : "=x"(dst) : "m"(*unaligned_ptr));
        
        if (!cmp_ymm(dst, expected)) {
            printf("Error in 256-bit mem-to-reg (unaligned)\n");
            print_ymm("Expected", expected);
            print_ymm("Got", dst);
            errors++;
        }
    }

    // 测试128位寄存器到内存传输（未对齐）
    {
        uint8_t mem_data[18] = {0}; // 额外2字节用于创建未对齐地址
        uint8_t* unaligned_ptr = mem_data + 1;
        uint8_t expected[16];
        
        for (int i = 0; i < 16; i++) expected[i] = 0xD0 + i;
        
        __m128i src;
        memcpy(&src, expected, 16);

        asm volatile("vmovdqu %1, %0" : "=m"(*unaligned_ptr) : "x"(src));
        
        if (memcmp(unaligned_ptr, expected, 16) != 0) {
            printf("Error in 128-bit reg-to-mem (unaligned)\n");
            printf("Expected: ");
            for (int i = 0; i < 16; i++) printf("%02X ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 16; i++) printf("%02X ", unaligned_ptr[i]);
            printf("\n");
            errors++;
        }
    }

    // 测试256位寄存器到内存传输（未对齐）
    {
        uint8_t mem_data[34] = {0}; // 额外2字节用于创建未对齐地址
        uint8_t* unaligned_ptr = mem_data + 1;
        uint8_t expected[32];
        
        for (int i = 0; i < 32; i++) expected[i] = 0xC0 + i;
        
        __m256i src;
        memcpy(&src, expected, 32);

        asm volatile("vmovdqu %1, %0" : "=m"(*unaligned_ptr) : "x"(src));
        
        if (memcmp(unaligned_ptr, expected, 32) != 0) {
            printf("Error in 256-bit reg-to-mem (unaligned)\n");
            printf("Expected: ");
            for (int i = 0; i < 32; i++) printf("%02X ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 32; i++) printf("%02X ", unaligned_ptr[i]);
            printf("\n");
            errors++;
        }
    }

    // 边界值测试：全0
    {
        __m256i src = _mm256_setzero_si256();
        __m256i dst;
        
        asm volatile("vmovdqu %1, %0" : "=x"(dst) : "m"(src));
        
        if (!cmp_ymm(dst, src)) {
            printf("Error in all-zero test\n");
            errors++;
        }
    }

    // 边界值测试：全1
    {
        __m256i src = _mm256_set1_epi8(0xFF);
        __m256i dst;
        
        asm volatile("vmovdqu %1, %0" : "=x"(dst) : "m"(src));
        
        if (!cmp_ymm(dst, src)) {
            printf("Error in all-ones test\n");
            errors++;
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VMOVDQU tests passed successfully!\n");
    } else {
        printf("VMOVDQU tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vmovdqu();
    return 0;
}
