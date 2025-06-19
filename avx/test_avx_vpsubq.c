#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 打印四字数组
static void print_qwords(int64_t *qwords, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%ld ", qwords[i]);
    }
    printf("\n");
}

int test_vpsubq() {
    printf("Testing VPSUBQ instruction\n");
    printf("=========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int64_t src1[2] = {9000000000000000000LL, -9000000000000000000LL};
        int64_t src2[2] = {1000000000000000000LL, -1000000000000000000LL};
        int64_t dst[2] = {0};

        // 预期结果(考虑溢出情况)
        int64_t expected[2] = {8000000000000000000LL, -8000000000000000000LL};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsubq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_qwords(src1, 2, "Input");
        print_qwords(src2, 2, "Subtrahend");
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %lld, expected %lld\n",
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
        
        int64_t src1[2] = {INT64_MAX, INT64_MIN};
        int64_t src2_mem[2] = {1, 1};
        int64_t dst[2] = {0};

        // 预期结果
        int64_t expected[2] = {INT64_MAX-1, INT64_MAX}; // INT64_MIN - 1 wraps around

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsubq %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_qwords(src1, 2, "Input");
        print_qwords(src2_mem, 2, "Subtrahend (mem)");
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %lld, expected %lld\n",
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
        
        int64_t src1[4] = {9000000000000000000LL, -9000000000000000000LL,
                          5000000000000000000LL, -5000000000000000000LL};
        int64_t src2[4] = {1000000000000000000LL, -1000000000000000000LL,
                          2000000000000000000LL, 3000000000000000000LL};
        int64_t dst[4] = {0};

        // 预期结果
        int64_t expected[4] = {8000000000000000000LL, -8000000000000000000LL,
                              3000000000000000000LL, -8000000000000000000LL}; // -5e18 - 3e18 = -8e18

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsubq %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_qwords(src1, 4, "Input");
        print_qwords(src2, 4, "Subtrahend");
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

    printf("\nVPSUBQ tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsubq();
    return result ? 0 : 1;
}
