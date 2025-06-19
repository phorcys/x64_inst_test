#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_shlx32() {
    printf("=== Testing 32-bit SHLX instruction ===\n");
    
    // 测试用例1: 基本逻辑左移
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint8_t shift = 4;
        
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint32_t expected = src << shift;
        
        printf("Test 1: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例2: 移位0
    {
        uint32_t src = 0x87654321;
        uint32_t result;
        uint8_t shift = 0;
        
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint32_t expected = src;
        
        printf("Test 2: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例3: 边界情况（移位31）
    {
        uint32_t src = 0x00000001;
        uint32_t result;
        uint8_t shift = 31;
        
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint32_t expected = src << shift;
        
        printf("Test 3: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例4: 移位超过31位（应取模32）
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint8_t shift = 32;  // 32 % 32 = 0
        
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint32_t expected = src;  // 移位0
        
        printf("Test 4: SHLX(0x%08X, %d)\n", src, shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例5: 内存操作数
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint8_t shift = 8;
        
        __asm__ __volatile__ (
            "shlxl %k[shift], %k[src], %k[result]\n\t"
            : [result] "=r" (result)
            : [src] "m" (src), [shift] "r" (shift)
        );
        uint32_t expected = src << shift;
        
        printf("Test 5: SHLX(mem operand, %d)\n", shift);
        printf("  Input:    0x%08X\n", src);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例6: 移位计数来自内存
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint32_t shift = 4;
        
        // 使用寄存器传递内存地址
        void *shift_ptr = &shift;
        __asm__ __volatile__ (
            "movl (%[shift_ptr]), %%ecx\n\t"
            "shlxl %%ecx, %k[src], %k[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift_ptr] "r" (shift_ptr)
            : "ecx"
        );
        uint32_t expected = src << shift;
        
        printf("Test 6: SHLX(src, shift from memory)\n");
        printf("  Input:    0x%08X\n", src);
        printf("  Shift:    %d\n", shift);
        printf("  Result:   0x%08X\n", result);
        printf("  Expected: 0x%08X\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
}

void test_shlx64() {
    printf("=== Testing 64-bit SHLX instruction ===\n");
    
    // 测试用例1: 基本逻辑左移
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint8_t shift = 8;
        
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint64_t expected = src << shift;
        
        printf("Test 1: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例2: 移位0
    {
        uint64_t src = 0x87654321FEDCBA98;
        uint64_t result;
        uint8_t shift = 0;
        
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint64_t expected = src;
        
        printf("Test 2: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例3: 边界情况（移位63）
    {
        uint64_t src = 0x0000000000000001;
        uint64_t result;
        uint8_t shift = 63;
        
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint64_t expected = src << shift;
        
        printf("Test 3: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例4: 移位超过63位（应取模64）
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint8_t shift = 64;  // 64 % 64 = 0
        
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift] "r" (shift)
        );
        uint64_t expected = src;  // 移位0
        
        printf("Test 4: SHLX(0x%016lX, %d)\n", src, shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例5: 内存操作数
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint8_t shift = 16;
        
        __asm__ __volatile__ (
            "shlxq %q[shift], %q[src], %q[result]\n\t"
            : [result] "=r" (result)
            : [src] "m" (src), [shift] "r" (shift)
        );
        uint64_t expected = src << shift;
        
        printf("Test 5: SHLX(mem operand, %d)\n", shift);
        printf("  Input:    0x%016lX\n", src);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例6: 移位计数来自内存
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint64_t shift = 4;
        
        // 使用寄存器传递内存地址
        void *shift_ptr = &shift;
        __asm__ __volatile__ (
            "movq (%[shift_ptr]), %%rcx\n\t"
            "shlxq %%rcx, %q[src], %q[result]\n\t"
            : [result] "=r" (result)
            : [src] "r" (src), [shift_ptr] "r" (shift_ptr)
            : "rcx"
        );
        uint64_t expected = src << shift;
        
        printf("Test 6: SHLX(src, shift from memory)\n");
        printf("  Input:    0x%016lX\n", src);
        printf("  Shift:    %ld\n", shift);
        printf("  Result:   0x%016lX\n", result);
        printf("  Expected: 0x%016lX\n", expected);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
}

int main() {
    test_shlx32();
    test_shlx64();
    printf("All SHLX tests completed successfully.\n");
    return 0;
}
