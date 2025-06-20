#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_sarx32() {
    printf("=== Testing 32-bit SARX instruction ===\n");
    
    // 测试用例1: 基本算术右移
    {
        uint32_t src = 0x80000000;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 1;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (int32_t)src >> shift;
        
        printf("Test 1: SARX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
    
    // 测试用例2: 移位0
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src;
        
        printf("Test 2: SARX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
    
    // 测试用例3: 边界情况（移位31）
    {
        uint32_t src = 0x80000000;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 31;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (int32_t)src >> shift;
        
        printf("Test 3: SARX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
    
    // 测试用例4: 内存操作数
    {
        uint32_t src = 0x87654321;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 4;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxl %k[shift], %k[src], %k[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "m" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (int32_t)src >> shift; // 算术右移
        
        printf("Test 4: SARX(mem operand, %d)\n", shift);
        printf("  Input:    0x%08X\n", src);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
}

void test_sarx64() {
    printf("=== Testing 64-bit SARX instruction ===\n");
    
    // 测试用例1: 基本算术右移
    {
        uint64_t src = 0x8000000000000000;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 1;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = (int64_t)src >> shift;
        
        printf("Test 1: SARX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
    
    // 测试用例2: 移位0
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src;
        
        printf("Test 2: SARX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
    
    // 测试用例3: 边界情况（移位63）
    {
        uint64_t src = 0x8000000000000000;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 63;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = 0xFFFFFFFFFFFFFFFF; // 符号位扩展
        
        printf("Test 3: SARX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
    
    // 测试用例4: 内存操作数
    {
        uint64_t src = 0xFEDCBA9876543210;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 8;
        
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "sarxq %q[shift], %q[src], %q[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "m" (src), [shift] "r" (shift)
            : "cc"
        );
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = (int64_t)src >> shift;
        
        printf("Test 4: SARX(mem operand, %d)\n", shift);
        printf("  Input:    0x%016lX\n", src);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        if (shift) {
            print_eflags_cond(eflags, X_CF);
        } else {
            print_eflags_cond(eflags, 0);
        }
        printf("\n");
    }
}

int main() {
    test_sarx32();
    test_sarx64();
    printf("SARX tests completed.\n");
    return 0;
}
