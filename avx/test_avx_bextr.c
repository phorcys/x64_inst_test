#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// BEXTR 32位指令测试函数
void test_bextr32() {
    printf("=== Testing 32-bit BEXTR instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint32_t src = 0x12345678;
        uint32_t control = (8) | (16 << 8); // 修正：低8位=起始位(8)，高8位=位数(16)
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = (src >> 8) & 0xFFFF; // 提取16位，从第8位开始
        
        __asm__ __volatile__ (
            "bextrl %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 1: 32-bit basic\n");
        printf("  Input: 0x%08X, Control: 0x%08X (start=%d, len=%d)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例2: 零长度测试
    {
        uint32_t src = 0xFFFFFFFF;
        uint32_t control = (0) | (0 << 8); // 修正：低8位=起始位(0)，高8位=位数(0)
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = 0;
        
        __asm__ __volatile__ (
            "bextrl %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 2: 32-bit zero length\n");
        printf("  Input: 0x%08X, Control: 0x%08X (start=%d, len=%d)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例3: 全长度测试
    {
        uint32_t src = 0x89ABCDEF;
        uint32_t control = (0) | (32 << 8); // 修正：低8位=起始位(0)，高8位=位数(32)
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = src;
        
        __asm__ __volatile__ (
            "bextrl %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 3: 32-bit full length\n");
        printf("  Input: 0x%08X, Control: 0x%08X (start=%d, len=%d)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例4: 边界测试
    {
        uint32_t src = 0x87654321;
        uint32_t control = (24) | (8 << 8); // 修正：低8位=起始位(24)，高8位=位数(8)
        uint32_t result;
        uint64_t rflags;
        uint32_t expected = (src >> 24) & 0xFF;
        
        __asm__ __volatile__ (
            "bextrl %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        uint32_t eflags = (uint32_t)rflags;
        
        printf("Test 4: 32-bit boundary\n");
        printf("  Input: 0x%08X, Control: 0x%08X (start=%d, len=%d)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags(eflags);
        printf("\n");
    }
}

// BEXTR 64位指令测试函数
void test_bextr64() {
    printf("=== Testing 64-bit BEXTR instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t control = (16) | (32 << 8); // 修正：低8位=起始位(16)，高8位=位数(32)
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = (src >> 16) & 0xFFFFFFFF;
        
        __asm__ __volatile__ (
            "bextrq %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        
        printf("Test 1: 64-bit basic\n");
        printf("  Input: 0x%016lX, Control: 0x%016lX (start=%ld, len=%ld)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
    
    // 测试用例2: 零长度测试
    {
        uint64_t src = 0xFFFFFFFFFFFFFFFF;
        uint64_t control = (0) | (0 << 8); // 修正：低8位=起始位(0)，高8位=位数(0)
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = 0;
        
        __asm__ __volatile__ (
            "bextrq %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        
        printf("Test 2: 64-bit zero length\n");
        printf("  Input: 0x%016lX, Control: 0x%016lX (start=%ld, len=%ld)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
    
    // 测试用例3: 全长度测试
    {
        uint64_t src = 0x89ABCDEF01234567;
        uint64_t control = (0) | (64 << 8); // 修正：低8位=起始位(0)，高8位=位数(64)
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = src;
        
        __asm__ __volatile__ (
            "bextrq %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        
        printf("Test 3: 64-bit full length\n");
        printf("  Input: 0x%016lX, Control: 0x%016lX (start=%ld, len=%ld)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        print_eflags((uint32_t)rflags);
        printf("\n");
    }
    
    // 测试用例4: 边界测试
    {
        uint64_t src = 0xFEDCBA9876543210;
        uint64_t control = (56) | (8 << 8); // 修正：低8位=起始位(56)，高8位=位数(8)
        uint64_t result;
        uint64_t rflags;
        uint64_t expected = (src >> 56) & 0xFF;
        
        __asm__ __volatile__ (
            "bextrq %[control], %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src), [control] "r" (control)
            : "cc"
        );
        
        printf("Test 4: 64-bit boundary\n");
        printf("  Input: 0x%016lX, Control: 0x%016lX (start=%ld, len=%ld)\n", 
               src, control, control & 0xFF, (control >> 8) & 0xFF);
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
    test_bextr32();
    test_bextr64();
    return 0;
}
