#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 打印字数组
static void print_words(uint16_t *words, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%u ", words[i]);
    }
    printf("\n");
}

int test_vpsubusw() {
    printf("Testing VPSUBUSW instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        uint16_t src1[8] = {60000, 30000, 15000, 0, 65535, 32768, 1, 50000};
        uint16_t src2[8] = {30000, 60000, 20000, 1, 1, 1, 2, 50000};
        uint16_t dst[8] = {0};

        // 预期结果(考虑饱和情况)
        uint16_t expected[8] = {30000, 0, 0, 0, 65534, 32767, 0, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsubusw %%xmm1, %%xmm0, %%xmm2\n\t"
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

    // 测试2: 128位内存操作数测试
    {
        printf("\nTest 2: 128-bit memory operand\n");
        
        uint16_t src1[8] = {65535, 0, 1, 32768, 60000, 30000, 15000, 50000};
        uint16_t src2_mem[8] = {1, 1, 2, 1, 30000, 60000, 20000, 50000};
        uint16_t dst[8] = {0};

        // 预期结果
        uint16_t expected[8] = {65534, 0, 0, 32767, 30000, 0, 0, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsubusw %2, %%xmm0, %%xmm1\n\t"
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

    // 测试3: 256位AVX测试
    {
        printf("\nTest 3: 256-bit AVX operation\n");
        
        uint16_t src1[16] = {60000, 30000, 15000, 0, 65535, 32768, 1, 50000,
                           60000, 30000, 15000, 0, 65535, 32768, 1, 50000};
        uint16_t src2[16] = {30000, 60000, 20000, 1, 1, 1, 2, 50000,
                           30000, 60000, 20000, 1, 1, 1, 2, 50000};
        uint16_t dst[16] = {0};

        // 预期结果
        uint16_t expected[16] = {30000, 0, 0, 0, 65534, 32767, 0, 0,
                               30000, 0, 0, 0, 65534, 32767, 0, 0};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsubusw %%ymm1, %%ymm0, %%ymm2\n\t"
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

    printf("\nVPSUBUSW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsubusw();
    return result ? 0 : 1;
}
