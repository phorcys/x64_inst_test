#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印32位有符号整数数组
static void print_dwords(int32_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", value[i]);
    }
    printf("\n");
}

// VPMINSD测试函数
int test_vpminsd() {
    printf("Testing VPMINSD instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;
    
    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含边界值和常规值
        int32_t src1[4] = {10000, -20000, 30000, -40000};
        int32_t src2[4] = {-15000, 25000, -35000, 45000};
        int32_t dst[4] = {0};

        // 预期结果
        int32_t expected[4] = {-15000, -20000, -35000, -40000};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpminsd %%xmm1, %%xmm0, %%xmm2\n\t"
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
        int32_t src1[4] = {-2147483648, 2147483647, -2147483647, 2147483646};
        int32_t src2[4] = {2147483647, -2147483648, 2147483646, -2147483647};
        int32_t dst[4] = {0};

        // 预期结果
        int32_t expected[4] = {-2147483648, -2147483648, -2147483647, -2147483647};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpminsd %%xmm1, %%xmm0, %%xmm2\n\t"
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
        int32_t src1[4] = {50000, -60000, 70000, -80000};
        
        // 内存操作数
        int32_t src2_mem[4] = {-40000, 65000, -75000, 90000};
        int32_t dst[4] = {0};

        // 预期结果
        int32_t expected[4] = {-40000, -60000, -75000, -80000};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpminsd %2, %%xmm0, %%xmm1\n\t"
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
        int32_t src1[8], src2[8], dst[8];
        for (int i = 0; i < 8; i++) {
            src1[i] = (i % 2) ? 2147483647 : -2147483648;
            src2[i] = (i % 3) ? -2147483648 : 2147483647;
        }

        // 计算预期结果
        int32_t expected[8];
        for (int i = 0; i < 8; i++) {
            expected[i] = (src1[i] < src2[i]) ? src1[i] : src2[i];
        }

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpminsd %%ymm1, %%ymm0, %%ymm2\n\t"
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
    
    printf("\nVPMINSD tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpminsd();
    return result ? 0 : 1;
}
