#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印64位无符号整数数组
static void print_qwords(uint64_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%lu ", value[i]);
    }
    printf("\n");
}

// VPSADBW测试函数
int test_vpsadbw() {
    printf("Testing VPSADBW instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        // 测试数据包含8位无符号整数
        uint8_t src1[16] = {10, 20, 30, 40, 50, 60, 70, 80, 
                           90, 100, 110, 120, 130, 140, 150, 160};
        uint8_t src2[16] = {15, 25, 35, 45, 55, 65, 75, 85,
                           95, 105, 115, 125, 135, 145, 155, 165};
        uint64_t dst[2] = {0};

        // 预期结果(绝对差之和)
        uint64_t expected[2] = {
            (uint64_t)(abs(10-15) + abs(20-25) + abs(30-35) + abs(40-45) + 
            abs(50-55) + abs(60-65) + abs(70-75) + abs(80-85)),
            (uint64_t)(abs(90-95) + abs(100-105) + abs(110-115) + abs(120-125) + 
            abs(130-135) + abs(140-145) + abs(150-155) + abs(160-165))
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsadbw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %lu, expected %lu\n", 
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
        uint8_t src1[16] = {255, 255, 255, 255, 255, 255, 255, 255,
                            0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t src2[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                            255, 255, 255, 255, 255, 255, 255, 255};
        uint64_t dst[2] = {0};

        // 预期结果
        uint64_t expected[2] = {
            255*8, 255*8
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpsadbw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印输入输出
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %lu, expected %lu\n",
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
        uint8_t src1[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        
        // 内存操作数
        uint8_t src2_mem[16] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        uint64_t dst[2] = {0};

        // 预期结果(每8字节计算绝对差之和)
        uint64_t expected[2] = {
            abs(1-16) + abs(2-15) + abs(3-14) + abs(4-13) + 
            abs(5-12) + abs(6-11) + abs(7-10) + abs(8-9),
            abs(9-8) + abs(10-7) + abs(11-6) + abs(12-5) + 
            abs(13-4) + abs(14-3) + abs(15-2) + abs(16-1)
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpsadbw %2, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2_mem)
            : "xmm0", "xmm1", "memory"
        );

        // 打印输入输出
        print_qwords(dst, 2, "Result");
        print_qwords(expected, 2, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %lu, expected %lu\n",
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
        uint8_t src1[32] = {10, 20, 30, 40, 50, 60, 70, 80, 
                           90, 100, 110, 120, 130, 140, 150, 160,
                           1, 2, 3, 4, 5, 6, 7, 8,
                           9, 10, 11, 12, 13, 14, 15, 16};
        uint8_t src2[32] = {15, 25, 35, 45, 55, 65, 75, 85,
                           95, 105, 115, 125, 135, 145, 155, 165,
                           16, 15, 14, 13, 12, 11, 10, 9,
                           8, 7, 6, 5, 4, 3, 2, 1};
        uint64_t dst[4] = {0};

        // 计算预期结果(每8字节计算绝对差之和)
        uint64_t expected[4] = {
            (uint64_t)(abs(10-15) + abs(20-25) + abs(30-35) + abs(40-45) + 
            abs(50-55) + abs(60-65) + abs(70-75) + abs(80-85)),
            (uint64_t)(abs(90-95) + abs(100-105) + abs(110-115) + abs(120-125) + 
            abs(130-135) + abs(140-145) + abs(150-155) + abs(160-165)),
            (uint64_t)(abs(1-16) + abs(2-15) + abs(3-14) + abs(4-13) + 
            abs(5-12) + abs(6-11) + abs(7-10) + abs(8-9)),
            (uint64_t)(abs(9-8) + abs(10-7) + abs(11-6) + abs(12-5) + 
            abs(13-4) + abs(14-3) + abs(15-2) + abs(16-1))
        };

        // 执行指令
        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpsadbw %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "ymm0", "ymm1", "ymm2", "memory"
        );

        // 打印输入输出
        print_qwords(dst, 4, "Result");
        print_qwords(expected, 4, "Expected");

        // 验证结果
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at element %d: got %lu, expected %lu\n",
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

    printf("\nVPSADBW tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    int result = test_vpsadbw();
    return result ? 0 : 1;
}
