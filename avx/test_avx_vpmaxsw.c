#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印16位整数数组
static void print_words(int16_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", value[i]);
    }
    printf("\n");
}

// VPMAXSW测试函数
int test_vpmaxsw() {
    printf("Testing VPMAXSW instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含边界值和常规值
        int16_t src1[8] = {32767, -32768, 10000, -20000, 0, 15000, -5000, 25000};
        int16_t src2[8] = {-32768, 32767, -15000, 20000, 10000, 0, -25000, 5000};
        int16_t dst[8] = {0};

        // 预期结果
        int16_t expected[8] = {
            32767, 32767, 10000, 20000, 10000, 15000, -5000, 25000
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmaxsw %%xmm1, %%xmm0, %%xmm2\n\t"
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

    // 测试2: 128位内存操作数测试
    {
        printf("\nTest 2: 128-bit memory operand\n");
        
        // 寄存器操作数
        int16_t src1[8] = {30000, -31000, 32000, -31000, -31000, 32000, 32000, -31000};
        
        // 内存操作数
        int16_t src2_mem[8] = {
            29000, -32000, 30000, -31000, 32000, -31000, -31000, 32000
        };
        int16_t dst[8] = {0};

        // 预期结果
        int16_t expected[8] = {
            30000, -31000, 32000, -31000, 32000, 32000, 32000, 32000
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpmaxsw %2, %%xmm0, %%xmm1\n\t"
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

    // 测试3: 256位AVX测试
    {
        printf("\nTest 3: 256-bit AVX operation\n");
        
        // 初始化测试数据
        int16_t src1[16], src2[16], dst[16];
        for (int i = 0; i < 16; i++) {
            src1[i] = (i % 2) ? 32767 : -32768;
            src2[i] = (i % 3) ? -32768 : 32767;
        }

        // 计算预期结果
        int16_t expected[16];
        for (int i = 0; i < 16; i++) {
            expected[i] = (src1[i] > src2[i]) ? src1[i] : src2[i];
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpmaxsw %%ymm1, %%ymm0, %%ymm2\n\t"
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

    printf("\nVPMAXSW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpmaxsw();
    return result ? 0 : 1;
}
