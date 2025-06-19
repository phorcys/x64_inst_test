#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 打印字数组
static void print_words(int16_t *words, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", words[i]);
    }
    printf("\n");
}

int test_vpsubsw() {
    printf("Testing VPSUBSW instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int16_t src1[8] = {30000, -30000, 15000, -15000, 32767, -32768, 0, 1};
        int16_t src2[8] = {10000, -10000, 20000, -20000, 1, 1, 1, -1};
        int16_t dst[8] = {0};

        // 预期结果(考虑饱和情况)
        int16_t expected[8] = {20000, -20000, -5000, 5000, 32766, -32768, -1, 2};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsubsw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_words(src1, 8, "Input");
        print_words(src2, 8, "Subtrahend");
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
        
        int16_t src1[8] = {32767, -32768, 0, 1, -1, 32767, -32768, 16384};
        int16_t src2_mem[8] = {1, 1, 1, -1, -1, -1, -1, 16384};
        int16_t dst[8] = {0};

        // 预期结果
        int16_t expected[8] = {32766, -32768, -1, 2, 0, 32767, -32767, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsubsw %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_words(src1, 8, "Input");
        print_words(src2_mem, 8, "Subtrahend (mem)");
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
        
        int16_t src1[16] = {30000, -30000, 15000, -15000, 32767, -32768, 0, 1,
                          30000, -30000, 15000, -15000, 32767, -32768, 0, 1};
        int16_t src2[16] = {10000, -10000, 20000, -20000, 1, 1, 1, -1,
                          10000, -10000, 20000, -20000, 1, 1, 1, -1};
        int16_t dst[16] = {0};

        // 预期结果
        int16_t expected[16] = {20000, -20000, -5000, 5000, 32766, -32768, -1, 2,
                               20000, -20000, -5000, 5000, 32766, -32768, -1, 2};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsubsw %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_words(src1, 16, "Input");
        print_words(src2, 16, "Subtrahend");
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

    printf("\nVPSUBSW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsubsw();
    return result ? 0 : 1;
}
