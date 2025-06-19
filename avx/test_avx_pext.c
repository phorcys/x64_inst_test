#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "avx.h"

// PEXT 的软件实现，用于验证结果
static uint32_t pext_u32(uint32_t src, uint32_t mask) {
    uint32_t res = 0;
    uint8_t k = 0;
    for (uint8_t i = 0; i < 32; i++) {
        if (mask & (1U << i)) {
            if (src & (1U << i)) {
                res |= (1U << k);
            }
            k++;
        }
    }
    return res;
}

static uint64_t pext_u64(uint64_t src, uint64_t mask) {
    uint64_t res = 0;
    uint8_t k = 0;
    for (uint8_t i = 0; i < 64; i++) {
        if (mask & (1ULL << i)) {
            if (src & (1ULL << i)) {
                res |= (1ULL << k);
            }
            k++;
        }
    }
    return res;
}

int main() {
    printf("Testing PEXT instruction\n");
    printf("========================\n");
    
    // 32 位 PEXT 测试
    {
        struct TestCase32 {
            uint32_t src;
            uint32_t mask;
            const char* name;
        } test_cases[] = {
            {0, 0, "src=0, mask=0"},
            {0xFFFFFFFF, 0xFFFFFFFF, "src=0xFFFFFFFF, mask=0xFFFFFFFF"},
            {0x12345678, 0x55555555, "src=0x12345678, mask=0x55555555"},
            {0x9ABCDEF0, 0x0F0F0F0F, "src=0x9ABCDEF0, mask=0x0F0F0F0F"},
            {0x12345678, 0x0000FFFF, "src=0x12345678, mask=0x0000FFFF"},
            {0x12345678, 0xFFFF0000, "src=0x12345678, mask=0xFFFF0000"},
            {0x80000000, 0x80000000, "src=0x80000000, mask=0x80000000"},
            {0x00000001, 0x00000001, "src=0x00000001, mask=0x00000001"},
            {0x12345678, 0xAAAAAAAA, "src=0x12345678, mask=0xAAAAAAAA"},
            {0x12345678, 0x00000001, "src=0x12345678, mask=0x00000001"},
        };
        
        printf("\n32-bit PEXT tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint32_t src = test_cases[i].src;
            uint32_t mask = test_cases[i].mask;
            uint32_t result;
            uint32_t expected = pext_u32(src, mask);
            
            // 执行 PEXT 指令
            asm volatile(
                "pext %2, %1, %0"
                : "=r" (result)
                : "r" (src), "r" (mask)
            );
            
            // 获取标志寄存器
            uint64_t flags;
            asm volatile (
                "pushfq\n\t"
                "pop %0"
                : "=r" (flags)
                : 
                : "cc"
            );
            
            printf("Test %zu: Input=0x%08X, Mask=0x%08X\n", i+1, src, mask);
            printf("  Expected: 0x%08X, Result: 0x%08X\n", expected, result);
            
            if (result != expected) {
                printf("  [FAIL] Test failed!\n");
            } else {
                printf("  [PASS] Test passed!\n");
            }
            
            print_eflags_cond((uint32_t)flags, 0x84D); // 显示CF/SF/ZF/OF
            printf("\n");
        }
    }
    
    // 64 位 PEXT 测试
    {
        struct TestCase64 {
            uint64_t src;
            uint64_t mask;
            const char* name;
        } test_cases[] = {
            {0, 0, "src=0, mask=0"},
            {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, "src=0xFFFFFFFFFFFFFFFF, mask=0xFFFFFFFFFFFFFFFF"},
            {0x123456789ABCDEF0, 0x5555555555555555, "src=0x123456789ABCDEF0, mask=0x5555555555555555"},
            {0x123456789ABCDEF0, 0x0F0F0F0F0F0F0F0F, "src=0x123456789ABCDEF0, mask=0x0F0F0F0F0F0F0F0F"},
            {0x123456789ABCDEF0, 0x00000000FFFFFFFF, "src=0x123456789ABCDEF0, mask=0x00000000FFFFFFFF"},
            {0x123456789ABCDEF0, 0xFFFFFFFF00000000, "src=0x123456789ABCDEF0, mask=0xFFFFFFFF00000000"},
            {0x8000000000000000, 0x8000000000000000, "src=0x8000000000000000, mask=0x8000000000000000"},
            {0x0000000000000001, 0x0000000000000001, "src=0x0000000000000001, mask=0x0000000000000001"},
            {0x123456789ABCDEF0, 0xAAAAAAAAAAAAAAAA, "src=0x123456789ABCDEF0, mask=0xAAAAAAAAAAAAAAAA"},
            {0x123456789ABCDEF0, 0x0000000000000001, "src=0x123456789ABCDEF0, mask=0x0000000000000001"},
        };
        
        printf("\n64-bit PEXT tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint64_t src = test_cases[i].src;
            uint64_t mask = test_cases[i].mask;
            uint64_t result;
            uint64_t expected = pext_u64(src, mask);
            
            // 执行 PEXT 指令
            asm volatile(
                "pext %2, %1, %0"
                : "=r" (result)
                : "r" (src), "r" (mask)
            );
            
            // 获取标志寄存器
            uint64_t flags;
            asm volatile (
                "pushfq\n\t"
                "pop %0"
                : "=r" (flags)
                : 
                : "cc"
            );
            
            printf("Test %zu: Input=0x%016" PRIX64 ", Mask=0x%016" PRIX64 "\n", i+1, src, mask);
            printf("  Expected: 0x%016" PRIX64 ", Result: 0x%016" PRIX64 "\n", expected, result);
            
            if (result != expected) {
                printf("  [FAIL] Test failed!\n");
            } else {
                printf("  [PASS] Test passed!\n");
            }
            
            print_eflags_cond((uint32_t)flags, 0x84D); // 显示CF/SF/ZF/OF
            printf("\n");
        }
    }
    
    printf("\nPEXT tests completed.\n");
    return 0;
}
