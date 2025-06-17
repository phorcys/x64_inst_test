#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "avx.h"

// 内联汇编宏定义
#define BZHI32(dst, src, idx) \
    asm volatile ("bzhi %2, %1, %0" : "=r" (dst) : "r" (src), "r" (idx) : "cc")

#define BZHI64(dst, src, idx) \
    asm volatile ("bzhi %2, %1, %0" : "=r" (dst) : "r" (src), "r" (idx) : "cc")

int main() {
    printf("Testing BZHI instruction\n");
    printf("========================\n");

    // 测试 32 位 BZHI
    {
        struct TestCase32 {
            uint32_t src;
            uint32_t idx;
            uint32_t expected;
            const char* name;
        } test_cases[] = {
            {0xFFFFFFFF, 0, 0, "0xFFFFFFFF, idx=0"},
            {0xFFFFFFFF, 1, 0x1, "0xFFFFFFFF, idx=1"},
            {0xFFFFFFFF, 8, 0xFF, "0xFFFFFFFF, idx=8"},
            {0xFFFFFFFF, 16, 0xFFFF, "0xFFFFFFFF, idx=16"},
            {0xFFFFFFFF, 31, 0x7FFFFFFF, "0xFFFFFFFF, idx=31"},
            {0xFFFFFFFF, 32, 0xFFFFFFFF, "0xFFFFFFFF, idx=32"},
            {0x12345678, 4, 0x8, "0x12345678, idx=4"},
            {0x12345678, 12, 0x678, "0x12345678, idx=12"},
            {0x12345678, 24, 0x345678, "0x12345678, idx=24"},
            {0x12345678, 32, 0x12345678, "0x12345678, idx=32"},
            {0x80000000, 31, 0x0, "0x80000000, idx=31"},
            {0x80000000, 32, 0x80000000, "0x80000000, idx=32"}
        };
        
        printf("\n32-bit BZHI tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint32_t src = test_cases[i].src;
            uint32_t idx = test_cases[i].idx;
            uint32_t result;
            uint32_t expected = test_cases[i].expected;
            
            // 清除标志寄存器
            uint64_t flags;
            CLEAR_FLAGS;
            
            asm volatile (
                "bzhi %[idx], %[src], %[result]\n\t"
                "pushfq\n\t"
                "pop %[flags]"
                : [result] "=r" (result),
                  [flags] "=r" (flags)
                : [src] "r" (src),
                  [idx] "r" (idx)
                : "cc"
            );
            
            // 忽略第1位（保留位，总是为1）
            flags &= ~(1UL << 1);
            
            printf("Test %zu: BZHI(%s) => ", i+1, test_cases[i].name);
            printf("Result: 0x%08" PRIx32 ", Expected: 0x%08" PRIx32 " ", result, expected);
            
            if (result == expected) {
                printf("[PASS]");
            } else {
                printf("[FAIL]");
            }
            printf(" | Flags: 0x%04" PRIx16 "\n", (uint16_t)flags);
            // 调整输出格式为16位十六进制（低16位）
            // 参考文件使用16位格式，所以只输出低16位
            // 修改为：printf(" | Flags: 0x%04" PRIx16 "\n", (uint16_t)flags);
        }
    }

    // 测试 64 位 BZHI
    {
        struct TestCase64 {
            uint64_t src;
            uint64_t idx;
            uint64_t expected;
            const char* name;
        } test_cases[] = {
            {0xFFFFFFFFFFFFFFFF, 0, 0, "0xFFFFFFFFFFFFFFFF, idx=0"},
            {0xFFFFFFFFFFFFFFFF, 1, 0x1, "0xFFFFFFFFFFFFFFFF, idx=1"},
            {0xFFFFFFFFFFFFFFFF, 8, 0xFF, "0xFFFFFFFFFFFFFFFF, idx=8"},
            {0xFFFFFFFFFFFFFFFF, 16, 0xFFFF, "0xFFFFFFFFFFFFFFFF, idx=16"},
            {0xFFFFFFFFFFFFFFFF, 32, 0xFFFFFFFF, "0xFFFFFFFFFFFFFFFF, idx=32"},
            {0xFFFFFFFFFFFFFFFF, 48, 0xFFFFFFFFFFFF, "0xFFFFFFFFFFFFFFFF, idx=48"},
            {0xFFFFFFFFFFFFFFFF, 63, 0x7FFFFFFFFFFFFFFF, "0xFFFFFFFFFFFFFFFF, idx=63"},
            {0xFFFFFFFFFFFFFFFF, 64, 0xFFFFFFFFFFFFFFFF, "0xFFFFFFFFFFFFFFFF, idx=64"},
            {0x123456789ABCDEF0, 4, 0x0, "0x123456789ABCDEF0, idx=4"}, // 正确：保留低4位 (0xF0 & 0xF = 0x0)
            {0x123456789ABCDEF0, 16, 0xDEF0, "0x123456789ABCDEF0, idx=16"}, // 保留低16位
            {0x123456789ABCDEF0, 32, 0x9ABCDEF0, "0x123456789ABCDEF0, idx=32"},
            {0x123456789ABCDEF0, 48, 0x56789ABCDEF0, "0x123456789ABCDEF0, idx=48"},
            {0x123456789ABCDEF0, 64, 0x123456789ABCDEF0, "0x123456789ABCDEF0, idx=64"},
            {0x8000000000000000, 63, 0x0, "0x8000000000000000, idx=63"},
            {0x8000000000000000, 64, 0x8000000000000000, "0x8000000000000000, idx=64"}
        };
        
        printf("\n64-bit BZHI tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint64_t src = test_cases[i].src;
            uint64_t idx = test_cases[i].idx;
            uint64_t result;
            uint64_t expected = test_cases[i].expected;
            
            // 清除标志寄存器
            uint64_t flags;
            CLEAR_FLAGS;
            
            asm volatile (
                "bzhi %[idx], %[src], %[result]\n\t"
                "pushfq\n\t"
                "pop %[flags]"
                : [result] "=r" (result),
                  [flags] "=r" (flags)
                : [src] "r" (src),
                  [idx] "r" (idx)
                : "cc"
            );
            
            // 忽略第1位（保留位，总是为1）
            flags &= ~(1UL << 1);
            
            printf("Test %zu: BZHI(%s) => ", i+1, test_cases[i].name);
            printf("Result: 0x%016" PRIx64 ", Expected: 0x%016" PRIx64 " ", result, expected);
            
            if (result == expected) {
                printf("[PASS]");
            } else {
                printf("[FAIL]");
            }
            printf(" | Flags: 0x%04" PRIx16 "\n", (uint16_t)flags);
            // 调整输出格式为16位十六进制（低16位）
            // 参考文件使用16位格式，所以只输出低16位
            // 修改为：printf(" | Flags: 0x%04" PRIx16 "\n", (uint16_t)flags);
        }
    }

    printf("\nBZHI tests completed.\n");
    return 0;
}
