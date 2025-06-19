#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 打印字节数组
static void print_bytes(int8_t *bytes, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", bytes[i]);
    }
    printf("\n");
}

int test_vpsubb() {
    printf("Testing VPSUBB instruction\n");
    printf("=========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int8_t src1[16] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 127, -128, -50, -100};
        int8_t src2[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 1, -50, 100};
        int8_t dst[16] = {0};

        // 预期结果(考虑溢出情况)
        int8_t expected[16] = {9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 126, 127, 0, 56}; // -100 - 100 = -200 -> 56 (256-200)

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsubb %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Input");
        print_bytes(src2, 16, "Subtrahend");
        print_bytes(dst, 16, "Result");
        print_bytes(expected, 16, "Expected");

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

    // 测试2: 128位内存操作数测试
    {
        printf("\nTest 2: 128-bit memory operand\n");
        
        int8_t src1[16] = {100, -100, 50, -50, 25, -25, 10, -10, 5, -5, 2, -2, 1, -1, 0, 0};
        int8_t src2_mem[16] = {1, -1, 50, 50, 25, 25, 10, 10, 5, 5, 2, 2, 1, 1, 0, 0};
        int8_t dst[16] = {0};

        // 预期结果
        int8_t expected[16] = {99, -99, 0, -100, 0, -50, 0, -20, 0, -10, 0, -4, 0, -2, 0, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsubb %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Input");
        print_bytes(src2_mem, 16, "Subtrahend (mem)");
        print_bytes(dst, 16, "Result");
        print_bytes(expected, 16, "Expected");

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

    // 测试3: 256位AVX测试
    {
        printf("\nTest 3: 256-bit AVX operation\n");
        
        int8_t src1[32] = {127, -128, 100, -100, 50, -50, 25, -25, 
                          10, -10, 5, -5, 2, -2, 1, -1,
                          0, 0, 1, -1, 2, -2, 5, -5,
                          10, -10, 25, -25, 50, -50, 100, -100};
        int8_t src2[32] = {1, 1, 100, 100, 50, 50, 25, 25,
                          10, 10, 5, 5, 2, 2, 1, 1,
                          0, 0, 1, 1, 2, 2, 5, 5,
                          10, 10, 25, 25, 50, 50, 100, 100};
        int8_t dst[32] = {0};

        // 预期结果
        int8_t expected[32] = {126, 127, 0, 56, 0, -100, 0, -50,
                              0, -20, 0, -10, 0, -4, 0, -2,
                              0, 0, 0, -2, 0, -4, 0, -10,
                              0, -20, 0, -50, 0, -100, 0, 56};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsubb %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 32, "Input");
        print_bytes(src2, 32, "Subtrahend");
        print_bytes(dst, 32, "Result");
        print_bytes(expected, 32, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 32; i++) {
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

    printf("\nVPSUBB tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsubb();
    return result ? 0 : 1;
}
