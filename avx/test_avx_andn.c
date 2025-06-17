#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// ANDN 32位指令测试函数
void test_andn32() {
    printf("=== Testing 32-bit ANDN instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint32_t src1 = 0x55555555;
        uint32_t src2 = 0xAAAAAAAA;
        uint32_t result;
        uint64_t rflags;
        
        __asm__ __volatile__ (
            "andnl %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "r" (src2)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (~src1) & src2;
        
        printf("Test 1: 32-bit basic\n");
        printf("  Input: NOT 0x%08X AND 0x%08X\n", src1, src2);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例2: 全0测试
    {
        uint32_t src1 = 0x00000000;
        uint32_t src2 = 0xFFFFFFFF;
        uint32_t result;
        uint64_t rflags;
        
        __asm__ __volatile__ (
            "andnl %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "r" (src2)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (~src1) & src2;
        
        printf("Test 2: 32-bit all zeros\n");
        printf("  Input: NOT 0x%08X AND 0x%08X\n", src1, src2);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例3: 全1测试
    {
        uint32_t src1 = 0xFFFFFFFF;
        uint32_t src2 = 0xFFFFFFFF;
        uint32_t result;
        uint64_t rflags;
        
        __asm__ __volatile__ (
            "andnl %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "r" (src2)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (~src1) & src2;
        
        printf("Test 3: 32-bit all ones\n");
        printf("  Input: NOT 0x%08X AND 0x%08X\n", src1, src2);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
    
    // 内存操作数测试
    {
        uint32_t src1 = 0x12345678;
        uint32_t src2 = 0x87654321;
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = (~src1) & src2;
        
        __asm__ __volatile__ (
            "andnl %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "m" (src2)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 4: 32-bit memory operand\n");
        printf("  Input: NOT 0x%08X AND 0x%08X\n", src1, src2);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
}

// ANDN 64位指令测试函数
void test_andn64() {
    printf("=== Testing 64-bit ANDN instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint64_t src1 = 0x5555555555555555;
        uint64_t src2 = 0xAAAAAAAAAAAAAAAA;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = (~src1) & src2;
        
        __asm__ __volatile__ (
            "andnq %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "r" (src2)
            : "cc"
        );
        
        printf("Test 1: 64-bit basic\n");
        printf("  Input: NOT 0x%016lX AND 0x%016lX\n", src1, src2);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
    
    // 测试用例2: 全0测试
    {
        uint64_t src1 = 0x0000000000000000;
        uint64_t src2 = 0xFFFFFFFFFFFFFFFF;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = (~src1) & src2;
        
        __asm__ __volatile__ (
            "andnq %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "r" (src2)
            : "cc"
        );
        
        printf("Test 2: 64-bit all zeros\n");
        printf("  Input: NOT 0x%016lX AND 0x%016lX\n", src1, src2);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
    
    // 测试用例3: 全1测试
    {
        uint64_t src1 = 0xFFFFFFFFFFFFFFFF;
        uint64_t src2 = 0xFFFFFFFFFFFFFFFF;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = (~src1) & src2;
        
        __asm__ __volatile__ (
            "andnq %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "r" (src2)
            : "cc"
        );
        
        printf("Test 3: 64-bit all ones\n");
        printf("  Input: NOT 0x%016lX AND 0x%016lX\n", src1, src2);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
    
    // 内存操作数测试
    {
        uint64_t src1 = 0x123456789ABCDEF0;
        uint64_t src2 = 0xFEDCBA9876543210;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = (~src1) & src2;
        
        __asm__ __volatile__ (
            "andnq %[src2], %[src1], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src1] "r" (src1), [src2] "m" (src2)
            : "cc"
        );
        
        printf("Test 4: 64-bit memory operand\n");
        printf("  Input: NOT 0x%016lX AND 0x%016lX\n", src1, src2);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
}

int main() {
    test_andn32();
    test_andn64();
    return 0;
}
