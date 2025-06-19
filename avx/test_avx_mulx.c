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
            {0xFFFFFFFF, 1, 0xFFFFFFFF, "EDX=0xFFFFFFFF, src=1"},
        };
        
        printf("\n32-bit MULX tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint32_t edx = test_cases[i].edx;
            uint32_t src = test_cases[i].src;
            uint32_t low, high;
            uint64_t expected = test_cases[i].expected;
            uint64_t eflags_before, eflags_after;
            
            // 保存执行前的RFLAGS
            asm volatile ("pushfq; pop %0" : "=r" (eflags_before));
            
            asm volatile(
                "mov %2, %%edx\n\t"  // 设置EDX
                "mulx %3, %0, %1" 
                : "=r" (low), "=r" (high) 
                : "r" (edx), "r" (src)
                : "rdx");
            
            // 保存执行后的EFLAGS
            asm volatile ("pushfq; pop %0" : "=r" (eflags_after));
            
            uint64_t result = ((uint64_t)high << 32) | low;
            
            printf("Test %zu: MULX(%s) => ", i+1, test_cases[i].name);
            printf("Result: 0x%016" PRIx64 ", Expected: 0x%016" PRIx64 " ", result, expected);
            
            int pass_result = (result == expected);
            // 比较RFLAGS的低32位（实际标志位）
            int pass_flags = ((eflags_before & 0xFFFFFFFF) == (eflags_after & 0xFFFFFFFF));
            
            if (pass_result && pass_flags) {
                printf("[PASS]\n");
            } else {
                printf("[FAIL]");
                if (!pass_result) printf(" (Result mismatch)");
                if (!pass_flags) {
                    printf(" (Flags changed: 0x%08X -> 0x%08X)", 
                           (uint32_t)(eflags_before & 0xFFFFFFFF), 
                           (uint32_t)(eflags_after & 0xFFFFFFFF));
                    printf("\n  Before flags: ");
                    print_eflags_cond(eflags_before, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
                    printf("  After flags:  ");
                    print_eflags_cond(eflags_after, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
                }
                printf("\n");
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
            {0xFFFFFFFFFFFFFFFF, 1, 0xFFFFFFFFFFFFFFFF, "RDX=0xFFFFFFFFFFFFFFFF, src=1"},
        };
        
        printf("\n64-bit MULX tests:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
            uint64_t rdx = test_cases[i].rdx;
            uint64_t src = test_cases[i].src;
            uint64_t low, high;
            __uint128_t expected = test_cases[i].expected;
            uint64_t eflags_before, eflags_after;
            
            // 保存执行前的RFLAGS
            asm volatile ("pushfq; pop %0" : "=r" (eflags_before));
            
            asm volatile(
                "mov %2, %%rdx\n\t"  // 设置RDX
                "mulx %3, %0, %1" 
                : "=r" (low), "=r" (high) 
                : "r" (rdx), "r" (src)
                : "rdx");
            
            // 保存执行后的RFLAGS
            asm volatile ("pushfq; pop %0" : "=r" (eflags_after));
            
            __uint128_t result = ((__uint128_t)high << 64) | low;
            
            printf("Test %zu: MULX(%s) => ", i+1, test_cases[i].name);
            printf("Result: 0x%016" PRIx64 "%016" PRIx64 ", Expected: 0x%016" PRIx64 "%016" PRIx64 " ", 
                   (uint64_t)(result >> 64), (uint64_t)result,
                   (uint64_t)(expected >> 64), (uint64_t)expected);
            
            int pass_result = (result == expected);
            // 比较RFLAGS的低32位（实际标志位）
            int pass_flags = ((eflags_before & 0xFFFFFFFF) == (eflags_after & 0xFFFFFFFF));
            
            if (pass_result && pass_flags) {
                printf("[PASS]\n");
            } else {
                printf("[FAIL]");
                if (!pass_result) printf(" (Result mismatch)");
                if (!pass_flags) {
                    printf(" (Flags changed: 0x%08X -> 0x%08X)", 
                           (uint32_t)(eflags_before & 0xFFFFFFFF), 
                           (uint32_t)(eflags_after & 0xFFFFFFFF));
                    printf("\n  Before flags: ");
                    print_eflags_cond(eflags_before, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
                    printf("  After flags:  ");
                    print_eflags_cond(eflags_after, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
                }
                printf("\n");
            }
        }
    }

    // 内存操作数测试
    {
        printf("\nMemory operand tests:\n");
        
        // 对齐内存测试
        uint64_t aligned_mem64[2] = {0x123456789ABCDEF0, 0xFEDCBA9876543210};
        uint32_t aligned_mem32[2] = {0x12345678, 0x9ABCDEF0};
        
        // 32位内存操作数测试
        {
            uint32_t edx_val = 0x12345678;
            uint32_t src_val = aligned_mem32[1];
            uint32_t low, high;
            uint64_t expected = (uint64_t)edx_val * src_val;
            uint64_t eflags_before, eflags_after;
            
            asm volatile ("pushfq; pop %0" : "=r" (eflags_before));
            asm volatile(
                "mov %[edx], %%edx\n\t"
                "mulx %[src], %[low], %[high]"
                : [low] "=r" (low), [high] "=r" (high)
                : [edx] "r" (edx_val), [src] "m" (src_val)
                : "rdx");
            asm volatile ("pushfq; pop %0" : "=r" (eflags_after));
            
            uint64_t result = ((uint64_t)high << 32) | low;
            printf("32-bit memory operand (aligned): ");
            printf("Result: 0x%016" PRIx64 ", Expected: 0x%016" PRIx64 " %s\n", 
                   result, expected, 
                   (result == expected && (eflags_before & 0xFFFFFFFF) == (eflags_after & 0xFFFFFFFF)) ? 
                   "[PASS]" : "[FAIL]");
        }
        
        // 64位内存操作数测试
        {
            uint64_t rdx_val = 0x123456789ABCDEF0;
            uint64_t src_val = aligned_mem64[1];
            uint64_t low, high;
            __uint128_t expected = (__uint128_t)rdx_val * src_val;
            uint64_t eflags_before, eflags_after;
            
            asm volatile ("pushfq; pop %0" : "=r" (eflags_before));
            asm volatile(
                "mov %[rdx], %%rdx\n\t"
                "mulx %[src], %[low], %[high]"
                : [low] "=r" (low), [high] "=r" (high)
                : [rdx] "r" (rdx_val), [src] "m" (src_val)
                : "rdx");
            asm volatile ("pushfq; pop %0" : "=r" (eflags_after));
            
            __uint128_t result = ((__uint128_t)high << 64) | low;
            printf("64-bit memory operand (aligned): ");
            printf("Result: 0x%016" PRIx64 "%016" PRIx64 ", Expected: 0x%016" PRIx64 "%016" PRIx64 " %s\n", 
                   (uint64_t)(result >> 64), (uint64_t)result,
                   (uint64_t)(expected >> 64), (uint64_t)expected,
                   (result == expected && (eflags_before & 0xFFFFFFFF) == (eflags_after & 0xFFFFFFFF)) ? 
                   "[PASS]" : "[FAIL]");
        }
        
        // 非对齐内存测试
        {
            uint8_t buffer[32] = {0};
            uint32_t* unaligned_mem32 = (uint32_t*)(buffer + 1);
            *unaligned_mem32 = 0x9ABCDEF0;
            
            uint32_t edx_val = 0x12345678;
            uint32_t src_val = *unaligned_mem32;
            uint32_t low, high;
            uint64_t expected = (uint64_t)edx_val * src_val;
            uint64_t eflags_before, eflags_after;
            
            asm volatile ("pushfq; pop %0" : "=r" (eflags_before));
            asm volatile(
                "mov %[edx], %%edx\n\t"
                "mulx %[src], %[low], %[high]"
                : [low] "=r" (low), [high] "=r" (high)
                : [edx] "r" (edx_val), [src] "m" (src_val)
                : "rdx");
            asm volatile ("pushfq; pop %0" : "=r" (eflags_after));
            
            uint64_t result = ((uint64_t)high << 32) | low;
            printf("32-bit memory operand (unaligned): ");
            printf("Result: 0x%016" PRIx64 ", Expected: 0x%016" PRIx64 " %s\n", 
                   result, expected, 
                   (result == expected && (eflags_before & 0xFFFFFFFF) == (eflags_after & 0xFFFFFFFF)) ? 
                   "[PASS]" : "[FAIL]");
        }
        
        // 64位非对齐内存测试
        {
            uint8_t buffer[32] = {0};
            uint64_t* unaligned_mem64 = (uint64_t*)(buffer + 1);
            *unaligned_mem64 = 0xFEDCBA9876543210;
            
            uint64_t rdx_val = 0x123456789ABCDEF0;
            uint64_t src_val = *unaligned_mem64;
            uint64_t low, high;
            __uint128_t expected = (__uint128_t)rdx_val * src_val;
            uint64_t eflags_before, eflags_after;
            
            asm volatile ("pushfq; pop %0" : "=r" (eflags_before));
            asm volatile(
                "mov %[rdx], %%rdx\n\t"
                "mulx %[src], %[low], %[high]"
                : [low] "=r" (low), [high] "=r" (high)
                : [rdx] "r" (rdx_val), [src] "m" (src_val)
                : "rdx");
            asm volatile ("pushfq; pop %0" : "=r" (eflags_after));
            
            __uint128_t result = ((__uint128_t)high << 64) | low;
            printf("64-bit memory operand (unaligned): ");
            printf("Result: 0x%016" PRIx64 "%016" PRIx64 ", Expected: 0x%016" PRIx64 "%016" PRIx64 " %s\n", 
                   (uint64_t)(result >> 64), (uint64_t)result,
                   (uint64_t)(expected >> 64), (uint64_t)expected,
                   (result == expected && (eflags_before & 0xFFFFFFFF) == (eflags_after & 0xFFFFFFFF)) ? 
                   "[PASS]" : "[FAIL]");
        }
    }
    
    printf("\nMULX tests completed.\n");
    return 0;
}
