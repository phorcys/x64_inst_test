#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "avx.h"

// 测试VMOVDQA指令
void test_vmovdqa() {
    int errors = 0;
    printf("Testing VMOVDQA instruction...\n");

    // 确保内存对齐
    #define ALIGNMENT 32
    void* aligned_ptr = NULL;
    
    // 测试128位寄存器到寄存器传输
    {
        uint8_t data[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        __m128i src, dst;
        __m128i expected;

        memcpy(&src, data, sizeof(data));
        expected = src;

        // 使用内联汇编实现VMOVDQA
        asm volatile("vmovdqa %1, %0" : "=x"(dst) : "x"(src));
        
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

        asm volatile("vmovdqa %1, %0" : "=x"(dst) : "x"(src));
        
        if (!cmp_ymm(dst, expected)) {
            printf("Error in 256-bit reg-to-reg transfer\n");
            print_ymm("Expected", expected);
            print_ymm("Got", dst);
            errors++;
        }
    }

    // 测试128位对齐内存到寄存器传输
    {
        if (posix_memalign(&aligned_ptr, 16, 16) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        uint8_t* aligned_mem = (uint8_t*)aligned_ptr;
        
        for (int i = 0; i < 16; i++) aligned_mem[i] = 0xF0 + i;
        
        __m128i dst;
        __m128i expected;
        memcpy(&expected, aligned_mem, 16);

        asm volatile("vmovdqa %1, %0" : "=x"(dst) : "m"(*aligned_mem));
        
        if (!cmp_xmm(dst, expected)) {
            printf("Error in 128-bit mem-to-reg (aligned)\n");
            print_xmm("Expected", expected);
            print_xmm("Got", dst);
            errors++;
        }
        free(aligned_ptr);
    }

    // 测试256位对齐内存到寄存器传输
    {
        if (posix_memalign(&aligned_ptr, 32, 32) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        uint8_t* aligned_mem = (uint8_t*)aligned_ptr;
        
        for (int i = 0; i < 32; i++) aligned_mem[i] = 0xE0 + i;
        
        __m256i dst;
        __m256i expected;
        memcpy(&expected, aligned_mem, 32);

        asm volatile("vmovdqa %1, %0" : "=x"(dst) : "m"(*aligned_mem));
        
        if (!cmp_ymm(dst, expected)) {
            printf("Error in 256-bit mem-to-reg (aligned)\n");
            print_ymm("Expected", expected);
            print_ymm("Got", dst);
            errors++;
        }
        free(aligned_ptr);
    }

    // 测试128位寄存器到对齐内存传输
    {
        if (posix_memalign(&aligned_ptr, 16, 16) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        uint8_t* aligned_mem = (uint8_t*)aligned_ptr;
        uint8_t expected[16];
        
        for (int i = 0; i < 16; i++) expected[i] = 0xD0 + i;
        
        __m128i src;
        memcpy(&src, expected, 16);

        asm volatile("vmovdqa %1, %0" : "=m"(*aligned_mem) : "x"(src));
        
        if (memcmp(aligned_mem, expected, 16) != 0) {
            printf("Error in 128-bit reg-to-mem (aligned)\n");
            printf("Expected: ");
            for (int i = 0; i < 16; i++) printf("%02X ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 16; i++) printf("%02X ", aligned_mem[i]);
            printf("\n");
            errors++;
        }
        free(aligned_ptr);
    }

    // 测试256位寄存器到对齐内存传输
    {
        if (posix_memalign(&aligned_ptr, 32, 32) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        uint8_t* aligned_mem = (uint8_t*)aligned_ptr;
        uint8_t expected[32];
        
        for (int i = 0; i < 32; i++) expected[i] = 0xC0 + i;
        
        __m256i src;
        memcpy(&src, expected, 32);

        asm volatile("vmovdqa %1, %0" : "=m"(*aligned_mem) : "x"(src));
        
        if (memcmp(aligned_mem, expected, 32) != 0) {
            printf("Error in 256-bit reg-to-mem (aligned)\n");
            printf("Expected: ");
            for (int i = 0; i < 32; i++) printf("%02X ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 32; i++) printf("%02X ", aligned_mem[i]);
            printf("\n");
            errors++;
        }
        free(aligned_ptr);
    }

    // 边界值测试：全0
    {
        __m256i src = _mm256_setzero_si256();
        __m256i dst;
        
        asm volatile("vmovdqa %1, %0" : "=x"(dst) : "x"(src));
        
        if (!cmp_ymm(dst, src)) {
            printf("Error in all-zero test\n");
            errors++;
        }
    }

    // 边界值测试：全1
    {
        __m256i src = _mm256_set1_epi8(0xFF);
        __m256i dst;
        
        asm volatile("vmovdqa %1, %0" : "=x"(dst) : "x"(src));
        
        if (!cmp_ymm(dst, src)) {
            printf("Error in all-ones test\n");
            errors++;
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VMOVDQA tests passed successfully!\n");
    } else {
        printf("VMOVDQA tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vmovdqa();
    return 0;
}
