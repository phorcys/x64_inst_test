#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "avx.h"

// 内联汇编宏定义
#define BLSR32(dst, src) \
    asm volatile ("blsr %1, %0" : "=r" (dst) : "r" (src))

#define BLSR64(dst, src) \
    asm volatile ("blsr %1, %0" : "=r" (dst) : "r" (src))

int main() {
    printf("Testing BLSR instruction\n");
    printf("=======================\n");

    // 测试 32 位 BLSR
    {
        uint32_t test_cases[] = {0, 1, 0x80000000, 0xFFFFFFFF, 0x12345678, 0x0000000F};
        const char* names[] = {"0", "1", "0x80000000", "0xFFFFFFFF", "0x12345678", "0x0000000F"};
        
        printf("\n32-bit BLSR tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint32_t src = test_cases[i];
            uint32_t result;
            uint32_t expected = src & (src - 1);  // BLSR 操作: x & (x-1)
            
            // 清除标志寄存器
            CLEAR_FLAGS;
            
            BLSR32(result, src);
            
            printf("Test %zu: BLSR(%s) => ", i+1, names[i]);
            printf("Result: 0x%08" PRIx32 ", Expected: 0x%08" PRIx32 " ", result, expected);
            
            if (result == expected) {
                printf("[PASS]");
            } else {
                printf("[FAIL]");
            }
            
            // 输出标志寄存器状态
            uint64_t flags;
            asm volatile ("pushfq\n\tpop %0" : "=r"(flags));
            printf(" | Flags: 0x%016" PRIx64 "\n", flags);
        }
    }

    // 测试 64 位 BLSR
    {
        uint64_t test_cases[] = {0, 1, 0x8000000000000000, 
                               0xFFFFFFFFFFFFFFFF, 0x123456789ABCDEF0, 0x000000000000000F};
        const char* names[] = {"0", "1", "0x8000000000000000", 
                             "0xFFFFFFFFFFFFFFFF", "0x123456789ABCDEF0", "0x000000000000000F"};
        
        printf("\n64-bit BLSR tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint64_t src = test_cases[i];
            uint64_t result;
            uint64_t expected = src & (src - 1);  // BLSR 操作: x & (x-1)
            
            // 清除标志寄存器
            CLEAR_FLAGS;
            
            BLSR64(result, src);
            
            printf("Test %zu: BLSR(%s) => ", i+1, names[i]);
            printf("Result: 0x%016" PRIx64 ", Expected: 0x%016" PRIx64 " ", result, expected);
            
            if (result == expected) {
                printf("[PASS]");
            } else {
                printf("[FAIL]");
            }
            
            // 输出标志寄存器状态
            uint64_t flags;
            asm volatile ("pushfq\n\tpop %0" : "=r"(flags));
            printf(" | Flags: 0x%016" PRIx64 "\n", flags);
        }
    }

    printf("\nBLSR tests completed.\n");
    return 0;
}
