#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// BLSI 32位指令测试函数
void test_blsi32() {
    printf("=== Testing 32-bit BLSI instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint32_t src = 0x12345678; // 0b00010010001101000101011001111000
        uint32_t result;
        uint64_t rflags;
        // 预期结果: 提取最低位的1 (0x00000008)
        uint32_t expected = src & -src;
        
        __asm__ __volatile__ (
            "blsil %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 1: 32-bit basic\n");
        printf("  Input: 0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0x84D);
        printf("\n");
    }
    
    // 测试用例2: 零输入测试
    {
        uint32_t src = 0x00000000;
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = 0;
        
        __asm__ __volatile__ (
            "blsil %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 2: 32-bit zero input\n");
        printf("  Input: 0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0x84D);
        printf("\n");
    }
    
    // 测试用例3: 最高位测试
    {
        uint32_t src = 0x80000000;
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = src; // 结果应等于输入值
        
        __asm__ __volatile__ (
            "blsil %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 3: 32-bit highest bit\n");
        printf("  Input: 0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0x84D);
        printf("\n");
    }
    
    // 测试用例4: 全1测试
    {
        uint32_t src = 0xFFFFFFFF;
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = 1; // 最低位是1
        
        __asm__ __volatile__ (
            "blsil %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 4: 32-bit all ones\n");
        printf("  Input: 0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0x84D);
        printf("\n");
    }
}

// BLSI 64位指令测试函数
void test_blsi64() {
    printf("=== Testing 64-bit BLSI instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = src & -src;
        
        __asm__ __volatile__ (
            "blsiq %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        
        printf("Test 1: 64-bit basic\n");
        printf("  Input: 0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond((uint32_t)rflags, 0x84D);
        printf("\n");
    }
    
    // 测试用例2: 零输入测试
    {
        uint64_t src = 0x0000000000000000;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = 0;
        
        __asm__ __volatile__ (
            "blsiq %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        
        printf("Test 2: 64-bit zero input\n");
        printf("  Input: 0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond((uint32_t)rflags, 0x84D);
        printf("\n");
    }
    
    // 测试用例3: 最高位测试
    {
        uint64_t src = 0x8000000000000000;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = src;
        
        __asm__ __volatile__ (
            "blsiq %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        
        printf("Test 3: 64-bit highest bit\n");
        printf("  Input: 0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond((uint32_t)rflags, 0x84D);
        printf("\n");
    }
    
    // 测试用例4: 全1测试
    {
        uint64_t src = 0xFFFFFFFFFFFFFFFF;
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = 1;
        
        __asm__ __volatile__ (
            "blsiq %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        
        printf("Test 4: 64-bit all ones\n");
        printf("  Input: 0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond((uint32_t)rflags, 0x84D);
        printf("\n");
    }
}

int main() {
    test_blsi32();
    test_blsi64();
    return 0;
}
