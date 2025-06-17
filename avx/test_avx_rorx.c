#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// RORX 指令测试函数
void test_rorx32() {
    printf("=== Testing 32-bit RORX instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint32_t src = 0x12345678;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 8; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxl $8, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (src >> shift) | (src << (32 - shift));
        
        printf("Test 1: 32-bit basic (shift=%d)\n", shift);
        printf("  Input:    0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
//        print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例2: 移位0
    {
        uint32_t src = 0x9ABCDEF0;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 0; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxl $0, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src;  // 移位0应返回原值
        
        printf("Test 2: 32-bit shift=0\n");
        printf("  Input:    0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
  //      print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例3: 移位32（应等同于移位0）
    {
        uint32_t src = 0x87654321;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 32; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxl $32, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = src;  // 移位32应等同于移位0
        
        printf("Test 3: 32-bit shift=32\n");
        printf("  Input:    0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
    //    print_eflags(eflags);
        printf("\n");
    }
    
    // 内存操作数测试
    {
        uint32_t src = 0x13579BDF;
        uint32_t result;
        uint64_t rflags;
        uint8_t shift = 16; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxl $16, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "m" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint32_t expected = (src >> shift) | (src << (32 - shift));
        
        printf("Test 4: 32-bit memory operand (shift=%d)\n", shift);
        printf("  Input:    0x%08X\n", src);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Result:   0x%08X\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
      //  print_eflags(eflags);
        printf("\n");
    }
}

// RORX 64位指令测试函数
void test_rorx64() {
    printf("=== Testing 64-bit RORX instruction ===\n");
    
    // 测试用例1: 基本测试
    {
        uint64_t src = 0x123456789ABCDEF0;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 16; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxq $16, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = (src >> shift) | (src << (64 - shift));
        
        printf("Test 1: 64-bit basic (shift=%d)\n", shift);
        printf("  Input:    0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        //print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例2: 移位0
    {
        uint64_t src = 0xFEDCBA9876543210;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 0; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxq $0, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src;  // 移位0应返回原值
        
        printf("Test 2: 64-bit shift=0\n");
        printf("  Input:    0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        // print_eflags(eflags);
        printf("\n");
    }
    
    // 测试用例3: 移位64（应等同于移位0）
    {
        uint64_t src = 0x1122334455667788;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 64; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxq $64, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "r" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = src;  // 移位64应等同于移位0
        
        printf("Test 3: 64-bit shift=64\n");
        printf("  Input:    0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
        // print_eflags(eflags);
        printf("\n");
    }
    
    // 内存操作数测试
    {
        uint64_t src = 0x2468ACE02468ACE0;
        uint64_t result;
        uint64_t rflags;
        uint8_t shift = 32; // 仅用于打印
        (void)shift; // 显式标记使用以避免警告
        
        // 清除标志寄存器
        CLEAR_FLAGS;
        __asm__ __volatile__ (
            "rorxq $32, %[src], %[result]\n\t"
            "pushfq\n\t"
            "popq %[rflags]\n\t"
            : [result] "=r" (result), [rflags] "=r" (rflags)
            : [src] "m" (src)
            : "cc"
        );
        // 清除未定义标志位
        rflags &= EFLAGS_MASK;
        uint32_t eflags = (uint32_t)rflags;
        uint64_t expected = (src >> shift) | (src << (64 - shift));
        
        printf("Test 4: 64-bit memory operand (shift=%d)\n", shift);
        printf("  Input:    0x%016lX\n", src);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Result:   0x%016lX\n", result);
        if (result != expected) {
            printf("  [ERROR] Result mismatch!\n");
        }
       // print_eflags(eflags);
        printf("\n");
    }
}

int main() {
    test_rorx32();
    test_rorx64();
    return 0;
}
