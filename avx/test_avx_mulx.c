#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "avx.h"

int main() {
    printf("Testing MULX instruction\n");
    printf("========================\n");
    
    // 32位 MULX 测试
    {
        struct TestCase32 {
            uint32_t edx;
            uint32_t src;
            uint64_t expected;
            const char* name;
        } test_cases[] = {
            {0, 0, 0, "EDX=0, src=0"},
            {0, 1, 0, "EDX=0, src=1"},
            {1, 0, 0, "EDX=1, src=0"},
            {1, 1, 1, "EDX=1, src=1"},
            {0xFFFFFFFF, 0xFFFFFFFF, (uint64_t)0xFFFFFFFF * 0xFFFFFFFF, "EDX=0xFFFFFFFF, src=0xFFFFFFFF"},
            {0x12345678, 0x9ABCDEF0, (uint64_t)0x12345678 * 0x9ABCDEF0, "EDX=0x12345678, src=0x9ABCDEF0"},
            {0x80000000, 0x80000000, (uint64_t)0x80000000 * 0x80000000, "EDX=0x80000000, src=0x80000000"},
            {0xFFFFFFFF, 1, 0xFFFFFFFF, "EDX=0xFFFFFFFF, src=1"}
        };
        
        printf("\n32-bit MULX tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint32_t edx = test_cases[i].edx;
            uint32_t src = test_cases[i].src;
            uint32_t low, high;
            uint64_t expected = test_cases[i].expected;
            
            // 执行MULX指令（直接设置EDX）
            asm volatile(
                "mov %2, %%edx\n\t"  // 设置EDX
                "mulx %3, %0, %1" 
                : "=r" (low), "=r" (high) 
                : "r" (edx), "r" (src)
                : "%edx", "cc");
            
            uint64_t result = ((uint64_t)high << 32) | low;
            
            printf("Test %zu: MULX(%s) => ", i+1, test_cases[i].name);
            printf("Result: 0x%016" PRIx64 ", Expected: 0x%016" PRIx64 " ", result, expected);
            
            if (result == expected) {
                printf("[PASS]\n");
            } else {
                printf("[FAIL]\n");
            }
        }
    }
    
    // 64位 MULX 测试
    {
        struct TestCase64 {
            uint64_t rdx;
            uint64_t src;
            __uint128_t expected;
            const char* name;
        } test_cases[] = {
            {0, 0, 0, "RDX=0, src=0"},
            {0, 1, 0, "RDX=0, src=1"},
            {1, 0, 0, "RDX=1, src=0"},
            {1, 1, 1, "RDX=1, src=1"},
            {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, (__uint128_t)0xFFFFFFFFFFFFFFFF * 0xFFFFFFFFFFFFFFFF, "RDX=0xFFFFFFFFFFFFFFFF, src=0xFFFFFFFFFFFFFFFF"},
            {0x123456789ABCDEF0, 0xFEDCBA9876543210, (__uint128_t)0x123456789ABCDEF0 * 0xFEDCBA9876543210, "RDX=0x123456789ABCDEF0, src=0xFEDCBA9876543210"},
            {0x8000000000000000, 0x8000000000000000, (__uint128_t)0x8000000000000000 * 0x8000000000000000, "RDX=0x8000000000000000, src=0x8000000000000000"},
            {0xFFFFFFFFFFFFFFFF, 1, 0xFFFFFFFFFFFFFFFF, "RDX=0xFFFFFFFFFFFFFFFF, src=1"}
        };
        
        printf("\n64-bit MULX tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint64_t rdx = test_cases[i].rdx;
            uint64_t src = test_cases[i].src;
            uint64_t low, high;
            __uint128_t expected = test_cases[i].expected;
            
            // 执行MULX指令（直接设置RDX）
            asm volatile(
                "mov %2, %%rdx\n\t"  // 设置RDX
                "mulx %3, %0, %1" 
                : "=r" (low), "=r" (high) 
                : "r" (rdx), "r" (src)
                : "%rdx", "cc");
            
            __uint128_t result = ((__uint128_t)high << 64) | low;
            
            printf("Test %zu: MULX(%s) => ", i+1, test_cases[i].name);
            printf("Result: 0x%016" PRIx64 "%016" PRIx64 ", Expected: 0x%016" PRIx64 "%016" PRIx64 " ", 
                   (uint64_t)(result >> 64), (uint64_t)result,
                   (uint64_t)(expected >> 64), (uint64_t)expected);
            
            if (result == expected) {
                printf("[PASS]\n");
            } else {
                printf("[FAIL]\n");
            }
        }
    }
    
    printf("\nMULX tests completed.\n");
    return 0;
}
