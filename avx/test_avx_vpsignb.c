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

int test_vpsignb() {
    printf("Testing VPSIGNB instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        int8_t src1[16] = {10, -20, 30, -40, 50, -60, 70, -80,
                          90, -100, 110, -120, 120, -120, 120, -120};
        int8_t src2[16] = {1, -1, 0, 1, -1, 0, 1, -1,
                          0, 1, -1, 0, 1, -1, 0, 1};
        int8_t dst[16] = {0};

        // 预期结果:
        // src2为正: 保持src1
        // src2为负: 取反src1
        // src2为零: 置零
        int8_t expected[16] = {10, 20, 0, -40, -50, 0, 70, 80,
                              0, -100, -110, 0, 120, 120, 0, -120};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsignb %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Input");
        print_bytes(src2, 16, "Signs");
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
        
        int8_t src1[16] = {1, -2, 3, -4, 5, -6, 7, -8, 9, -10, 11, -12, 13, -14, 15, -16};
        int8_t src2_mem[16] = {-1, 1, 0, -1, 1, 0, -1, 1, 0, -1, 1, 0, -1, 1, 0, -1};
        int8_t dst[16] = {0};

        // 预期结果
        int8_t expected[16] = {-1, -2, 0, 4, 5, 0, -7, -8, 0, 10, 11, 0, -13, -14, 0, 16};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsignb %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 16, "Input");
        print_bytes(src2_mem, 16, "Signs (mem)");
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
        
        int8_t src1[32] = {1, -2, 3, -4, 5, -6, 7, -8, 9, -10, 11, -12, 13, -14, 15, -16,
                           -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11, 12, -13, 14, -15, 16};
        int8_t src2[32] = {-1, 1, 0, -1, 1, 0, -1, 1, 0, -1, 1, 0, -1, 1, 0, -1,
                           1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1};
        int8_t dst[32] = {0};

        // 预期结果
        int8_t expected[32] = {-1, -2, 0, 4, 5, 0, -7, -8, 0, 10, 11, 0, -13, -14, 0, 16,
                              -1, -2, 0, 4, 5, 0, -7, -8, 0, 10, 11, 0, -13, -14, 0, 16};

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsignb %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_bytes(src1, 32, "Input");
        print_bytes(src2, 32, "Signs");
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

    printf("\nVPSIGNB tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsignb();
    return result ? 0 : 1;
}
