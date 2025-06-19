#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "avx.h"

// 内联汇编宏定义
#define BLSR32(dst, src) \
    asm volatile ("blsrl %1, %0" : "=r" (dst) : "r" (src) : "cc", "memory")

#define BLSR64(dst, src) \
    asm volatile ("blsrq %1, %0" : "=r" (dst) : "r" (src) : "cc", "memory")

int main() {
    printf("Testing BLSR instruction\n");
    printf("=======================\n");

    // 测试 32 位 BLSR
    {
        struct TestCase32 {
            uint32_t input;
            uint32_t expected;
            uint32_t expected_cf;
        } cases[] = {
            {0x00000000, 0x00000000, 1},  // 零输入
            {0x00000001, 0x00000000, 0},  // 最低位
            {0x80000000, 0x00000000, 0},  // 最高位
            {0xFFFFFFFF, 0xFFFFFFFE, 0},  // 全1
            {0x12345678, 0x12345670, 0},  // 随机值
            {0x0000000F, 0x0000000E, 0},  // 低4位为1
            {0x00001000, 0x00000000, 0},  // 中间位测试(第12位)
            {0x00000001, 0x00000000, 0}   // 重复最低位测试
        };

        printf("\n32-bit BLSR tests:\n");
        for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
            uint32_t result;
            uint64_t rflags_before, rflags_after;

            // 保存RFLAGS
            asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_before));

            // 测试寄存器操作数和内存操作数
            if (i % 2 == 0) {
                BLSR32(result, cases[i].input);  // 寄存器操作数
            } else {
                uint32_t mem_input = cases[i].input;
                asm volatile ("blsrl %1, %0" : "=r"(result) : "m"(mem_input));  // 内存操作数
            }

            // 获取RFLAGS
            asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_after));

            // 计算CF标志
            uint32_t cf = (rflags_after & 1) ? 1 : 0;

            printf("Test %zu: Input=0x%08X\n", i+1, cases[i].input);
            printf("  Expected: 0x%08X, Result: 0x%08X\n", cases[i].expected, result);
            printf("  Expected CF: %d, Actual CF: %d\n", cases[i].expected_cf, cf);

            if (result != cases[i].expected || cf != cases[i].expected_cf) {
                printf("  [FAIL] Test failed!\n");
            } else {
                printf("  [PASS] Test passed!\n");
            }

            print_eflags_cond((uint32_t)rflags_after, 0x84D); // 显示CF/SF/ZF/OF
            printf("\n");
        }
    }

    // 测试 64 位 BLSR
    {
        struct TestCase64 {
            uint64_t input;
            uint64_t expected;
            uint32_t expected_cf;
        } cases[] = {
            {0x0000000000000000, 0x0000000000000000, 1},  // 零输入
            {0x0000000000000001, 0x0000000000000000, 0},  // 最低位
            {0x8000000000000000, 0x0000000000000000, 0},  // 最高位
            {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0},  // 全1
            {0x123456789ABCDEF0, 0x123456789ABCDEE0, 0},  // 随机值
            {0x000000000000000F, 0x000000000000000E, 0},  // 低4位为1
            {0x0000000010000000, 0x0000000000000000, 0},  // 中间位测试(第28位)
            {0x0000000000000001, 0x0000000000000000, 0}   // 重复最低位测试
        };

        printf("\n64-bit BLSR tests:\n");
        for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
            uint64_t result;
            uint64_t rflags_before, rflags_after;

            // 保存RFLAGS
            asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_before));

            // 测试寄存器操作数和内存操作数
            if (i % 2 == 0) {
                BLSR64(result, cases[i].input);  // 寄存器操作数
            } else {
                uint64_t mem_input = cases[i].input;
                asm volatile ("blsrq %1, %0" : "=r"(result) : "m"(mem_input));  // 内存操作数
            }

            // 获取RFLAGS
            asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_after));

            // 计算CF标志
            uint32_t cf = (rflags_after & 1) ? 1 : 0;

            printf("Test %zu: Input=0x%016" PRIX64 "\n", i+1, cases[i].input);
            printf("  Expected: 0x%016" PRIX64 ", Result: 0x%016" PRIX64 "\n", 
                  cases[i].expected, result);
            printf("  Expected CF: %d, Actual CF: %d\n", cases[i].expected_cf, cf);

            if (result != cases[i].expected || cf != cases[i].expected_cf) {
                printf("  [FAIL] Test failed!\n");
            } else {
                printf("  [PASS] Test passed!\n");
            }

            print_eflags_cond((uint32_t)rflags_after, 0x84D); // 显示CF/SF/ZF/OF
            printf("\n");
        }
    }

    printf("\nBLSR tests completed.\n");
    return 0;
}
