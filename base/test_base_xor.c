#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test XOR operations with different operand combinations and widths
static void test_xor_combinations() {
    uint8_t u8_result, u8_expected;
    uint16_t u16_result, u16_expected;
    uint32_t u32_result, u32_expected;
    uint64_t u64_result, u64_expected;
    uint64_t flags;
    
    printf("\nTesting 8-bit XOR operations:\n");
    // Register-Register
    asm volatile (
        "movb $0xAA, %%al \n\t"
        "movb $0x55, %%bl \n\t"
        "xorb %%bl, %%al \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : "=a" (u8_result), "=d" (flags)
        :
        : "bl", "cc", "memory"
    );
    u8_expected = 0xFF;
    printf("  XOR AL, BL: Result=0x%02x, Expected=0x%02x - %s\n", 
           u8_result, u8_expected, u8_result == u8_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    // Memory-Register
    uint8_t mem8 = 0xF0;
    asm volatile (
        "movb $0x0F, %%al \n\t"
        "xorb %[mem_val], %%al \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u8_result), "=d" (flags)
        : [mem_val] "m" (mem8)
        : "cc", "memory"
    );
    u8_expected = 0xFF;
    printf("  XOR AL, mem8: Result=0x%02x, Expected=0x%02x - %s\n", 
           u8_result, u8_expected, u8_result == u8_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    // Immediate-Register
    asm volatile (
        "movb $0xFF, %%al \n\t"
        "xorb $0xFF, %%al \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : "=a" (u8_result), "=d" (flags)
        :
        : "cc", "memory"
    );
    u8_expected = 0x00;
    printf("  XOR AL, imm8: Result=0x%02x, Expected=0x%02x - %s\n", 
           u8_result, u8_expected, u8_result == u8_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    printf("\nTesting 16-bit XOR operations:\n");
    // Register-Memory
    uint16_t mem16 = 0xAAAA;
    asm volatile (
        "movw $0x5555, %%ax \n\t"
        "xorw %[mem_val], %%ax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u16_result), "=d" (flags)
        : [mem_val] "m" (mem16)
        : "cc", "memory"
    );
    u16_expected = 0xFFFF;
    printf("  XOR AX, mem16: Result=0x%04x, Expected=0x%04x - %s\n", 
           u16_result, u16_expected, u16_result == u16_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    // Immediate-Memory
    asm volatile (
        "movw $0xFFFF, %%ax \n\t"
        "xorw $0xFFFF, %%ax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u16_result), "=d" (flags)
        :
        : "cc", "memory"
    );
    u16_expected = 0x0000;
    printf("  XOR AX, imm16: Result=0x%04x, Expected=0x%04x - %s\n", 
           u16_result, u16_expected, u16_result == u16_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    printf("\nTesting 32-bit XOR operations:\n");
    // Register-Register
    asm volatile (
        "movl $0xAAAAAAAA, %%eax \n\t"
        "movl $0x55555555, %%ebx \n\t"
        "xorl %%ebx, %%eax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u32_result), "=d" (flags)
        :
        : "ebx", "cc", "memory"
    );
    u32_expected = 0xFFFFFFFF;
    printf("  XOR EAX, EBX: Result=0x%08x, Expected=0x%08x - %s\n", 
           u32_result, u32_expected, u32_result == u32_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    // Memory-Immediate
    uint32_t mem32 = 0x12345678;
    asm volatile (
        "movl $0x12345678, %%eax \n\t"
        "xorl $0x12345678, %%eax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u32_result), "=d" (flags)
        :
        : "cc", "memory"
    );
    u32_expected = 0x00000000;
    printf("  XOR EAX, imm32: Result=0x%08x, Expected=0x%08x - %s\n", 
           u32_result, u32_expected, u32_result == u32_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    printf("\nTesting 64-bit XOR operations:\n");
    // Register-Register
    asm volatile (
        "movq $0xAAAAAAAAAAAAAAAA, %%rax \n\t"
        "movq $0x5555555555555555, %%rbx \n\t"
        "xorq %%rbx, %%rax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u64_result), "=d" (flags)
        :
        : "rbx", "cc", "memory"
    );
    u64_expected = 0xFFFFFFFFFFFFFFFF;
    printf("  XOR RAX, RBX: Result=0x%016lx, Expected=0x%016lx - %s\n", 
           u64_result, u64_expected, u64_result == u64_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    // Memory-Register
    uint64_t mem64 = 0x0123456789ABCDEF;
    asm volatile (
        "movq $0xFEDCBA9876543210, %%rax \n\t"
        "xorq %[mem_val], %%rax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u64_result), "=d" (flags)
        : [mem_val] "m" (mem64)
        : "cc", "memory"
    );
    u64_expected = 0xFEDCBA9876543210 ^ 0x0123456789ABCDEF;
    printf("  XOR RAX, mem64: Result=0x%016lx, Expected=0x%016lx - %s\n", 
           u64_result, u64_expected, u64_result == u64_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
    
    // Same register XOR (should clear register and set ZF)
    asm volatile (
        "movq $0x1234567890ABCDEF, %%rax \n\t"
        "xorq %%rax, %%rax \n\t"
        "pushfq\n\t"
        "popq %%rdx"
        : [result] "=a" (u64_result), "=d" (flags)
        :
        : "cc", "memory"
    );
    u64_expected = 0x0000000000000000;
    printf("  XOR RAX, RAX: Result=0x%016lx, Expected=0x%016lx - %s\n", 
           u64_result, u64_expected, u64_result == u64_expected ? "PASS" : "FAIL");
    print_eflags_cond((uint32_t)flags, X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF);
}

int main() {
    printf("XOR instruction comprehensive test\n");
    printf("=================================\n");
    
    test_xor_combinations();
    
    printf("\nXOR test completed\n");
    printf("==================\n");
    
    return 0;
}
