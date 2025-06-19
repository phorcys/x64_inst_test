#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印16位无符号整数数组
static void print_words(uint16_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%u ", value[i]);
    }
    printf("\n");
}

// VPMULHUW测试函数
int test_vpmulhuw() {
    printf("Testing VPMULHUW instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含16位无符号整数
        uint16_t src1[8] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
        uint16_t src2[8] = {1500, 2500, 3500, 4500, 5500, 6500, 7500, 8500};
        uint16_t dst[8] = {0};

        // 预期结果(高16位乘法结果)
        uint16_t expected[8];
        for (int i = 0; i < 8; i++) {
            uint32_t product = (uint32_t)src1[i] * src2[i];
            expected[i] = product >> 16;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmulhuw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_words(src1, 8, "Operand A");
        print_words(src2, 8, "Operand B");
        print_words(dst, 8, "Result");
        print_words(expected, 8, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %u, expected %u\n", 
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    // 测试2: 128位边界值测试
    {
        printf("\nTest 2: 128-bit boundary values\n");
        
        // 测试数据包含边界值
        uint16_t src1[8] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535};
        uint16_t src2[8] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535};
        uint16_t dst[8] = {0};

        // 预期结果
        uint16_t expected[8];
        for (int i = 0; i < 8; i++) {
            uint32_t product = (uint32_t)src1[i] * src2[i];
            expected[i] = product >> 16;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmulhuw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_words(src1, 8, "Operand A");
        print_words(src2, 8, "Operand B");
        print_words(dst, 8, "Result");
        print_words(expected, 8, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %u, expected %u\n", 
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    // 测试3: 128位内存操作数测试
    {
        printf("\nTest 3: 128-bit memory operand\n");
        
        // 寄存器操作数
        uint16_t src1[8] = {5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000};
        
        // 内存操作数
        uint16_t src2_mem[8] = {4000, 6500, 7500, 8500, 9500, 10500, 11500, 12500};
        uint16_t dst[8] = {0};

        // 预期结果
        uint16_t expected[8];
        for (int i = 0; i < 8; i++) {
            uint32_t product = (uint32_t)src1[i] * src2_mem[i];
            expected[i] = product >> 16;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpmulhuw %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_words(src1, 8, "Operand A (register)");
        print_words(src2_mem, 8, "Operand B (memory)");
        print_words(dst, 8, "Result");
        print_words(expected, 8, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %u, expected %u\n", 
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    // 测试4: 256位AVX测试
    {
        printf("\nTest 4: 256-bit AVX operation\n");
        
        // 初始化测试数据
        uint16_t src1[16], src2[16], dst[16];
        for (int i = 0; i < 16; i++) {
            src1[i] = 1000 * (i+1);
            src2[i] = 500 * (i+1);
        }

        // 计算预期结果
        uint16_t expected[16];
        for (int i = 0; i < 16; i++) {
            uint32_t product = (uint32_t)src1[i] * src2[i];
            expected[i] = product >> 16;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpmulhuw %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_words(src1, 16, "Operand A");
        print_words(src2, 16, "Operand B");
        print_words(dst, 16, "Result");
        print_words(expected, 16, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 16; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %u, expected %u\n", 
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    printf("\nVPMULHUW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpmulhuw();
    return result ? 0 : 1;
}
