#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void print_words(int16_t *words, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", words[i]);
    }
    printf("\n");
}

int test_vpsubw() {
    printf("Testing VPSUBW instruction\n");
    printf("=========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int16_t src1[8] = {30000, -30000, 15000, -15000, 32767, -32768, 0, 1000};
        int16_t src2[8] = {1000, -1000, 20000, -20000, 1, -1, 500, -500};
        int16_t dst[8] = {0};

        // 预期结果(允许溢出)
        int16_t expected[8] = {29000, -29000, -5000, 5000, 32766, -32767, -500, 1500};

        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsubw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        print_words(src1, 8, "Input");
        print_words(src2, 8, "Subtrahend");
        print_words(dst, 8, "Result");
        print_words(expected, 8, "Expected");

        int pass = 1;
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %d, expected %d\n",
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) printf("[PASS]\n");
        else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    // 测试2: 128位内存操作数测试
    {
        printf("\nTest 2: 128-bit memory operand\n");
        
        int16_t src1[8] = {32767, -32768, 0, 1000, -1000, 20000, -20000, 500};
        int16_t src2_mem[8] = {1, -1, 500, -500, 30000, -30000, 15000, -15000};
        int16_t dst[8] = {0};

        // 修正后的预期结果
        int16_t expected[8] = {32766, -32767, -500, 1500, -31000, -15536, 30536, 15500};

        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsubw %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        print_words(src1, 8, "Input");
        print_words(src2_mem, 8, "Subtrahend (mem)");
        print_words(dst, 8, "Result");
        print_words(expected, 8, "Expected");

        int pass = 1;
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %d, expected %d\n",
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) printf("[PASS]\n");
        else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    // 测试3: 256位AVX测试
    {
        printf("\nTest 3: 256-bit AVX operation\n");
        
        int16_t src1[16] = {30000, -30000, 15000, -15000, 32767, -32768, 0, 1000,
                           -1000, 20000, -20000, 500, 32767, -32768, 0, 1000};
        int16_t src2[16] = {1000, -1000, 20000, -20000, 1, -1, 500, -500,
                          30000, -30000, 15000, -15000, 1, -1, 500, -500};
        int16_t dst[16] = {0};

        // 修正后的预期结果
        int16_t expected[16] = {29000, -29000, -5000, 5000, 32766, -32767, -500, 1500,
                               -31000, -15536, 30536, 15500, 32766, -32767, -500, 1500};

        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsubw %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        print_words(src1, 16, "Input");
        print_words(src2, 16, "Subtrahend");
        print_words(dst, 16, "Result");
        print_words(expected, 16, "Expected");

        int pass = 1;
        for (int i = 0; i < 16; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %d, expected %d\n",
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) printf("[PASS]\n");
        else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    printf("\nVPSUBW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    return test_vpsubw() ? 0 : 1;
}
