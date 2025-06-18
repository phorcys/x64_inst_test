#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_shlx32() {
    printf("=== Testing 32-bit SHLX instruction ===\n");
    
    // 测试用例1: 基本逻辑左移
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 4;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src << shift;
        
        printf("Test 1: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
    
    // 测试用例2: 移位0
    {
        uint32_t src = 0x87654321;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src;
        
        printf("Test 2: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
    
    // 测试用例3: 边界情况（移位31）
    {
        uint32_t src = 0x00000001;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 31;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src << shift;
        
        printf("Test 3: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
    
    // 测试用例4: 内存操作数
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 8;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "m" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src << shift;
        
        printf("Test 4: SHLX(mem operand, %d)\n", shift);
        printf("  Input:    0x%08X\n", src);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
}

void test_shlx64() {
    printf("=== Testing 64-bit SHLX instruction ===\n");
    
    // 测试用例1: 基本逻辑左移
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 8;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src << shift;
        
        printf("Test 1: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
    
    // 测试用例2: 移位0
    {
        uint64_t src = 0x87654321FEDCBA98;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src;
        
        printf("Test 2: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
    
    // 测试用例3: 边界情况（移位63）
    {
        uint64_t src = 0x0000000000000001;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 63;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src << shift;
        
        printf("Test 3: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
    
    // 测试用例4: 内存操作数
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 16;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "m" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src << shift;
        
        printf("Test 4: SHLX(mem operand, %d)\n", shift);
        printf("  Input:    0x%016lX\n", src);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags_cond(eflags, 0);
        printf("\n");
    }
}

int main() {
    test_shlx32();
    test_shlx64();
    printf("SHLX tests completed.\n");
    return 0;
}
