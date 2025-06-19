#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印8位无符号整数数组
static void print_bytes(uint8_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%u ", value[i]);
    }
    printf("\n");
}

// VPMINUB测试函数
int test_vpminub() {
    printf("Testing VPMINUB instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含边界值和常规值
        uint8_t src1[16] = {10, 20, 30, 40, 50, 60, 70, 80, 
                            90, 100, 110, 120, 130, 140, 150, 160};
        uint8_t src2[16] = {15, 25, 35, 45, 55, 65, 75, 85,
                            95, 105, 115, 125, 135, 145, 155, 165};
        uint8_t dst[16] = {0};

        // 预期结果
        uint8_t expected[16] = {
            10, 20, 30, 40, 50, 60, 70, 80,
            90, 100, 110, 120, 130, 140, 150, 160
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpminub %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Operand A");
        print_bytes(src2, 16, "Operand B");
        print_bytes(dst, 16, "Result");
        print_bytes(expected, 16, "Expected");

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

    // 测试2: 128位边界值测试
    {
        printf("\nTest 2: 128-bit boundary values\n");
        
        // 测试数据包含边界值
        uint8_t src1[16] = {0, 255, 0, 255, 0, 255, 0, 255,
                            0, 255, 0, 255, 0, 255, 0, 255};
        uint8_t src2[16] = {255, 0, 255, 0, 255, 0, 255, 0,
                            255, 0, 255, 0, 255, 0, 255, 0};
        uint8_t dst[16] = {0};

        // 预期结果
        uint8_t expected[16] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpminub %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Operand A");
        print_bytes(src2, 16, "Operand B");
        print_bytes(dst, 16, "Result");
        print_bytes(expected, 16, "Expected");

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

    // 测试3: 128位内存操作数测试
    {
        printf("\nTest 3: 128-bit memory operand\n");
        
        // 寄存器操作数
        uint8_t src1[16] = {50, 100, 150, 200, 250, 200, 150, 100,
                            50, 100, 150, 200, 250, 200, 150, 100};
        
        // 内存操作数
        uint8_t src2_mem[16] = {
            100, 150, 200, 250, 200, 150, 100, 50,
            100, 150, 200, 250, 200, 150, 100, 50
        };
        uint8_t dst[16] = {0};

        // 预期结果
        uint8_t expected[16] = {
            50, 100, 150, 200, 200, 150, 100, 50,
            50, 100, 150, 200, 200, 150, 100, 50
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpminub %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Operand A (register)");
        print_bytes(src2_mem, 16, "Operand B (memory)");
        print_bytes(dst, 16, "Result");
        print_bytes(expected, 16, "Expected");

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

    // 测试4: 256位AVX测试
    {
        printf("\nTest 4: 256-bit AVX operation\n");
        
        // 初始化测试数据
        uint8_t src1[32], src2[32], dst[32];
        for (int i = 0; i < 32; i++) {
            src1[i] = (i % 2) ? 255 : 0;
            src2[i] = (i % 3) ? 0 : 255;
        }

        // 计算预期结果
        uint8_t expected[32];
        for (int i = 0; i < 32; i++) {
            expected[i] = (src1[i] < src2[i]) ? src1[i] : src2[i];
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpminub %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 32, "Operand A");
        print_bytes(src2, 32, "Operand B");
        print_bytes(dst, 32, "Result");
        print_bytes(expected, 32, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 32; i++) {
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

    printf("\nVPMINUB tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpminub();
    return result ? 0 : 1;
}
