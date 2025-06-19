#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印32位无符号整数数组
static void print_dwords(uint32_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%u ", value[i]);
    }
    printf("\n");
}

// VPMINUD测试函数
int test_vpminud() {
    printf("Testing VPMINUD instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含边界值和常规值
        uint32_t src1[4] = {100000, 200000, 300000, 400000};
        uint32_t src2[4] = {150000, 250000, 350000, 450000};
        uint32_t dst[4] = {0};

        // 预期结果
        uint32_t expected[4] = {100000, 200000, 300000, 400000};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpminud %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 4, "Operand A");
        print_dwords(src2, 4, "Operand B");
        print_dwords(dst, 4, "Result");
        print_dwords(expected, 4, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
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
        uint32_t src1[4] = {0, 4294967295, 0, 4294967295};
        uint32_t src2[4] = {4294967295, 0, 4294967295, 0};
        uint32_t dst[4] = {0};

        // 预期结果
        uint32_t expected[4] = {0, 0, 0, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpminud %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 4, "Operand A");
        print_dwords(src2, 4, "Operand B");
        print_dwords(dst, 4, "Result");
        print_dwords(expected, 4, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
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
        uint32_t src1[4] = {500000, 600000, 700000, 800000};
        
        // 内存操作数
        uint32_t src2_mem[4] = {400000, 650000, 750000, 900000};
        uint32_t dst[4] = {0};

        // 预期结果
        uint32_t expected[4] = {400000, 600000, 700000, 800000};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpminud %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 4, "Operand A (register)");
        print_dwords(src2_mem, 4, "Operand B (memory)");
        print_dwords(dst, 4, "Result");
        print_dwords(expected, 4, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
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
        uint32_t src1[8], src2[8], dst[8];
        for (int i = 0; i < 8; i++) {
            src1[i] = (i % 2) ? 4294967295 : 0;
            src2[i] = (i % 3) ? 0 : 4294967295;
        }

        // 计算预期结果
        uint32_t expected[8];
        for (int i = 0; i < 8; i++) {
            expected[i] = (src1[i] < src2[i]) ? src1[i] : src2[i];
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpminud %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 8, "Operand A");
        print_dwords(src2, 8, "Operand B");
        print_dwords(dst, 8, "Result");
        print_dwords(expected, 8, "Expected");

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

    printf("\nVPMINUD tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpminud();
    return result ? 0 : 1;
}
