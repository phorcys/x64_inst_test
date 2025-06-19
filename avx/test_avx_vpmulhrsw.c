#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印16位有符号整数数组
static void print_words(int16_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", value[i]);
    }
    printf("\n");
}

// VPMULHRSW测试函数
int test_vpmulhrsw() {
    printf("Testing VPMULHRSW instruction\n");
    printf("===========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含16位有符号整数
        int16_t src1[8] = {1000, -2000, 3000, -4000, 5000, -6000, 7000, -8000};
        int16_t src2[8] = {-1500, 2500, -3500, 4500, -5500, 6500, -7500, 8500};
        int16_t dst[8] = {0};

        // 预期结果(带舍入的乘法结果)
        int16_t expected[8];
        for (int i = 0; i < 8; i++) {
            int32_t product = (int32_t)src1[i] * src2[i];
            expected[i] = (product + 0x4000) >> 15; // 带舍入的右移15位
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmulhrsw %%xmm1, %%xmm0, %%xmm2\n\t"
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
                printf("Mismatch at element %d: got %d, expected %d\n", 
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
        int16_t src1[8] = {32767, -32768, 32767, -32768, 32767, -32768, 32767, -32768};
        int16_t src2[8] = {-32768, 32767, 32767, -32768, -32768, 32767, 32767, -32768};
        int16_t dst[8] = {0};

        // 预期结果
        int16_t expected[8];
        for (int i = 0; i < 8; i++) {
            int32_t product = (int32_t)src1[i] * src2[i];
            expected[i] = (product + 0x4000) >> 15;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmulhrsw %%xmm1, %%xmm0, %%xmm2\n\t"
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
                printf("Mismatch at element %d: got %d, expected %d\n", 
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
        int16_t src1[8] = {5000, -6000, 7000, -8000, 9000, -10000, 11000, -12000};
        
        // 内存操作数
        int16_t src2_mem[8] = {-4000, 6500, -7500, 8500, -9500, 10500, -11500, 12500};
        int16_t dst[8] = {0};

        // 预期结果
        int16_t expected[8];
        for (int i = 0; i < 8; i++) {
            int32_t product = (int32_t)src1[i] * src2_mem[i];
            expected[i] = (product + 0x4000) >> 15;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpmulhrsw %2, %%xmm0, %%xmm1\n\t"
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
                printf("Mismatch at element %d: got %d, expected %d\n", 
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
        int16_t src1[16], src2[16], dst[16];
        for (int i = 0; i < 16; i++) {
            src1[i] = (i % 2) ? -1000 * (i+1) : 1000 * (i+1);
            src2[i] = (i % 3) ? -500 * (i+1) : 500 * (i+1);
        }

        // 计算预期结果
        int16_t expected[16];
        for (int i = 0; i < 16; i++) {
            int32_t product = (int32_t)src1[i] * src2[i];
            expected[i] = (product + 0x4000) >> 15;
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpmulhrsw %%ymm1, %%ymm0, %%ymm2\n\t"
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
                printf("Mismatch at element %d: got %d, expected %d\n", 
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

    printf("\nVPMULHRSW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpmulhrsw();
    return result ? 0 : 1;
}
