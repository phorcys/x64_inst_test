#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 打印双字数组
static void print_dwords(int32_t *dwords, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", dwords[i]);
    }
    printf("\n");
}

int test_vpsignd() {
    printf("Testing VPSIGND instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int32_t src1[4] = {10, -20, 30, -40};
        int32_t src2[4] = {1, -1, 0, 1};
        int32_t dst[4] = {0};

        // 预期结果:
        // src2为正: 保持src1
        // src2为负: 取反src1
        // src2为零: 置零
        int32_t expected[4] = {10, 20, 0, -40};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsignd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 4, "Input");
        print_dwords(src2, 4, "Signs");
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

    // 测试2: 128位内存操作数测试
    {
        printf("\nTest 2: 128-bit memory operand\n");
        
        int32_t src1[4] = {100, -200, 300, -400};
        int32_t src2_mem[4] = {-1, 1, 0, -1};
        int32_t dst[4] = {0};

        // 预期结果
        int32_t expected[4] = {-100, -200, 0, 400};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsignd %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 4, "Input");
        print_dwords(src2_mem, 4, "Signs (mem)");
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

    // 测试3: 256位AVX测试
    {
        printf("\nTest 3: 256-bit AVX operation\n");
        
        int32_t src1[8] = {1, -2, 3, -4, 5, -6, 7, -8};
        int32_t src2[8] = {-1, 1, 0, -1, 1, 0, -1, 1};
        int32_t dst[8] = {0};

        // 预期结果
        int32_t expected[8] = {-1, -2, 0, 4, 5, 0, -7, -8};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsignd %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_dwords(src1, 8, "Input");
        print_dwords(src2, 8, "Signs");
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

    printf("\nVPSIGND tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsignd();
    return result ? 0 : 1;
}
