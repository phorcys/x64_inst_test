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

int test_vpsubsb() {
    printf("Testing VPSUBSB instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int8_t src1[16] = {100, -100, 50, -50, 127, -128, 0, 1,
                          -1, 127, -128, 64, -64, 32, -32, 16};
        int8_t src2[16] = {50, -50, 100, -100, 1, 1, 1, -1,
                          -1, -1, -1, 64, 64, 32, 32, 16};
        int8_t dst[16] = {0};

        // 预期结果(考虑饱和情况)
        int8_t expected[16] = {50, -50, -50, 50, 126, -128, -1, 2,
                              0, 127, -127, 0, -128, 0, -64, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsubsb %%xmm1, %%xmm0, %%xmm2\n\t"
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
        
        int8_t src1[16] = {127, -128, 0, 1, -1, 127, -128, 64,
                          -64, 32, -32, 16, 8, -8, 4, -4};
        int8_t src2_mem[16] = {1, 1, 1, -1, -1, -1, -1, 64,
                              64, 32, 32, 16, 8, 8, 4, 4};
        int8_t dst[16] = {0};

        // 预期结果
        int8_t expected[16] = {126, -128, -1, 2, 0, 127, -127, 0,
                              -128, 0, -64, 0, 0, -16, 0, -8};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsubsb %2, %%xmm0, %%xmm1\n\t"
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
        
        int8_t src1[32] = {100, -100, 50, -50, 127, -128, 0, 1,
                          -1, 127, -128, 64, -64, 32, -32, 16,
                          100, -100, 50, -50, 127, -128, 0, 1,
                          -1, 127, -128, 64, -64, 32, -32, 16};
        int8_t src2[32] = {50, -50, 100, -100, 1, 1, 1, -1,
                          -1, -1, -1, 64, 64, 32, 32, 16,
                          50, -50, 100, -100, 1, 1, 1, -1,
                          -1, -1, -1, 64, 64, 32, 32, 16};
        int8_t dst[32] = {0};

        // 预期结果
        int8_t expected[32] = {50, -50, -50, 50, 126, -128, -1, 2,
                              0, 127, -127, 0, -128, 0, -64, 0,
                              50, -50, -50, 50, 126, -128, -1, 2,
                              0, 127, -127, 0, -128, 0, -64, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsubsb %%ymm1, %%ymm0, %%ymm2\n\t"
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

    printf("\nVPSUBSB tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsubsb();
    return result ? 0 : 1;
}
