#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印64位有符号整数数组
static void print_qwords(int64_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%ld ", value[i]);
    }
    printf("\n");
}

// VPMULDQ测试函数
int test_vpmuldq() {
    printf("Testing VPMULDQ instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含32位有符号整数
        int32_t src1[4] = {100000, -200000, 300000, -400000};
        int32_t src2[4] = {-150000, 250000, -350000, 450000};
        int64_t dst[2] = {0};

        // 预期结果
        int64_t expected[2] = {
            (int64_t)src1[0] * src2[0],
            (int64_t)src1[2] * src2[2]
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmuldq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        printf("Operand A: %d %d %d %d\n", src1[0], src1[1], src1[2], src1[3]);
        printf("Operand B: %d %d %d %d\n", src2[0], src2[1], src2[2], src2[3]);
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %ld, expected %ld\n", 
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
        int32_t src1[4] = {2147483647, -2147483648, 2147483647, -2147483648};
        int32_t src2[4] = {-2147483648, 2147483647, 2147483647, -2147483648};
        int64_t dst[2] = {0};

        // 预期结果
        int64_t expected[2] = {
            (int64_t)src1[0] * src2[0],
            (int64_t)src1[2] * src2[2]
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmuldq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        printf("Operand A: %d %d %d %d\n", src1[0], src1[1], src1[2], src1[3]);
        printf("Operand B: %d %d %d %d\n", src2[0], src2[1], src2[2], src2[3]);
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %ld, expected %ld\n", 
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
        int32_t src1[4] = {500000, -600000, 700000, -800000};
        
        // 内存操作数
        int32_t src2_mem[4] = {-400000, 650000, -750000, 900000};
        int64_t dst[2] = {0};

        // 预期结果
        int64_t expected[2] = {
            (int64_t)src1[0] * src2_mem[0],
            (int64_t)src1[2] * src2_mem[2]
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpmuldq %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        printf("Operand A (register): %d %d %d %d\n", src1[0], src1[1], src1[2], src1[3]);
        printf("Operand B (memory): %d %d %d %d\n", src2_mem[0], src2_mem[1], src2_mem[2], src2_mem[3]);
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %ld, expected %ld\n", 
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
        int32_t src1[8] = {100000, -200000, 300000, -400000, 500000, -600000, 700000, -800000};
        int32_t src2[8] = {-150000, 250000, -350000, 450000, -550000, 650000, -750000, 850000};
        int64_t dst[4] = {0};

        // 计算预期结果
        int64_t expected[4] = {
            (int64_t)src1[0] * src2[0],
            (int64_t)src1[2] * src2[2],
            (int64_t)src1[4] * src2[4],
            (int64_t)src1[6] * src2[6]
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpmuldq %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        printf("Operand A: ");
        for (int i = 0; i < 8; i++) printf("%d ", src1[i]);
        printf("\nOperand B: ");
        for (int i = 0; i < 8; i++) printf("%d ", src2[i]);
        printf("\n");
        print_qwords(dst, 4, "Result");
        print_qwords(expected, 4, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %ld, expected %ld\n", 
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

    printf("\nVPMULDQ tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpmuldq();
    return result ? 0 : 1;
}
