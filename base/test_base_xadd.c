#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "base.h"

// Helper function to print flags for XADD
static void print_xadd_flags(uint64_t eflags) {
    printf("  Flags: ");
    if (eflags & X_CF) printf("CF ");
    if (eflags & X_PF) printf("PF ");
    if (eflags & X_AF) printf("AF ");
    if (eflags & X_ZF) printf("ZF ");
    if (eflags & X_SF) printf("SF ");
    if (eflags & X_OF) printf("OF");
    printf("\n");
}

// Test XADD with different operand sizes
static void test_xadd_size(uint8_t size) {
    const char *size_str = (size == 1) ? "byte" : (size == 2) ? "word" : (size == 4) ? "dword" : "qword";
    printf("Testing XADD %s:\n", size_str);
    
    uint64_t eflags;
    bool success = true;
    
    // Test with register operands
    for (int i = 0; i < 10; i++) {
        uint64_t a = (i * 10) + 5;
        uint64_t b = (i * 5) + 3;
        uint64_t expected_sum = a + b;
        
        // Save original value of a for comparison
        uint64_t original_a = a;
        
        if (size == 1) {
            __asm__ volatile (
                "xaddb %%bl, %%al\n\t"
                "pushfq\n\t"
                "pop %[eflags]"
                : "+a" (a), "+b" (b), [eflags] "=r" (eflags)
                : 
                : "cc"
            );
        } else if (size == 2) {
            __asm__ volatile (
                "xaddw %%bx, %%ax\n\t"
                "pushfq\n\t"
                "pop %[eflags]"
                : "+a" (a), "+b" (b), [eflags] "=r" (eflags)
                : 
                : "cc"
            );
        } else if (size == 4) {
            __asm__ volatile (
                "xaddl %%ebx, %%eax\n\t"
                "pushfq\n\t"
                "pop %[eflags]"
                : "+a" (a), "+b" (b), [eflags] "=r" (eflags)
                : 
                : "cc"
            );
        } else {
            __asm__ volatile (
                "xaddq %%rbx, %%rax\n\t"
                "pushfq\n\t"
                "pop %[eflags]"
                : "+a" (a), "+b" (b), [eflags] "=r" (eflags)
                : 
                : "cc"
            );
        }
        
        printf("  Registers: RAX=0x%lx, RBX=0x%lx -> ", a, b);
        printf("Result: RAX=0x%lx, RBX=0x%lx\n", a, b);
        
        if (a != expected_sum || b != original_a) {
            printf("    ERROR: Expected RAX=0x%lx (sum), RBX=0x%lx (original RAX)\n", expected_sum, original_a);
            success = false;
        }
        print_xadd_flags(eflags);
    }
    
    // Test with memory operand using appropriately sized variables
    char buffer[16] __attribute__((aligned(8)));
    
    // Test aligned memory
    printf("  Aligned Memory:\n");
    if (size == 1) {
        uint8_t *mem_aligned = (uint8_t*)&buffer[0];
        *mem_aligned = 0x12;
        uint8_t reg_byte = 0x34;
        uint8_t expected_sum = *mem_aligned + reg_byte;
        uint8_t original_mem = *mem_aligned;
        
        __asm__ volatile (
            "xaddb %[reg_byte], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_aligned), [reg_byte] "+q" (reg_byte), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%x, Register=0x%x -> ", original_mem, 0x34);
        printf("Result: Memory=0x%x, Register=0x%x\n", *mem_aligned, reg_byte);
        
        if (*mem_aligned != expected_sum || reg_byte != original_mem) {
            printf("      ERROR: Expected Memory=0x%x (sum), Register=0x%x (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    } else if (size == 2) {
        uint16_t *mem_aligned = (uint16_t*)&buffer[0];
        *mem_aligned = 0x1234;
        uint16_t reg_word = 0x5678;
        uint16_t expected_sum = *mem_aligned + reg_word;
        uint16_t original_mem = *mem_aligned;
        
        __asm__ volatile (
            "xaddw %[reg_word], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_aligned), [reg_word] "+r" (reg_word), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%x, Register=0x%x -> ", original_mem, 0x5678);
        printf("Result: Memory=0x%x, Register=0x%x\n", *mem_aligned, reg_word);
        
        if (*mem_aligned != expected_sum || reg_word != original_mem) {
            printf("      ERROR: Expected Memory=0x%x (sum), Register=0x%x (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    } else if (size == 4) {
        uint32_t *mem_aligned = (uint32_t*)&buffer[0];
        *mem_aligned = 0x12345678;
        uint32_t reg_dword = 0x9ABCDEF0;
        uint32_t expected_sum = *mem_aligned + reg_dword;
        uint32_t original_mem = *mem_aligned;
        
        __asm__ volatile (
            "xaddl %[reg_dword], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_aligned), [reg_dword] "+r" (reg_dword), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%x, Register=0x%x -> ", original_mem, 0x9ABCDEF0);
        printf("Result: Memory=0x%x, Register=0x%x\n", *mem_aligned, reg_dword);
        
        if (*mem_aligned != expected_sum || reg_dword != original_mem) {
            printf("      ERROR: Expected Memory=0x%x (sum), Register=0x%x (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    } else {
        uint64_t *mem_aligned = (uint64_t*)&buffer[0];
        *mem_aligned = 0x123456789ABCDEF0;
        uint64_t reg_qword = 0xF0F0F0F0F0F0F0F0;
        uint64_t expected_sum = *mem_aligned + reg_qword;
        uint64_t original_mem = *mem_aligned;
        
        __asm__ volatile (
            "xaddq %[reg_qword], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_aligned), [reg_qword] "+r" (reg_qword), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%lx, Register=0x%lx -> ", original_mem, 0xF0F0F0F0F0F0F0F0);
        printf("Result: Memory=0x%lx, Register=0x%lx\n", *mem_aligned, reg_qword);
        
        if (*mem_aligned != expected_sum || reg_qword != original_mem) {
            printf("      ERROR: Expected Memory=0x%lx (sum), Register=0x%lx (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    }
    
    // Test unaligned memory
    printf("  Unaligned Memory:\n");
    if (size == 1) {
        uint8_t *mem_unaligned = (uint8_t*)&buffer[1];
        *mem_unaligned = 0x56;
        uint8_t reg_byte = 0x78;
        uint8_t expected_sum = *mem_unaligned + reg_byte;
        uint8_t original_mem = *mem_unaligned;
        
        __asm__ volatile (
            "xaddb %[reg_byte], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_unaligned), [reg_byte] "+q" (reg_byte), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%x, Register=0x%x -> ", original_mem, 0x78);
        printf("Result: Memory=0x%x, Register=0x%x\n", *mem_unaligned, reg_byte);
        
        if (*mem_unaligned != expected_sum || reg_byte != original_mem) {
            printf("      ERROR: Expected Memory=0x%x (sum), Register=0x%x (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    } else if (size == 2) {
        uint16_t *mem_unaligned = (uint16_t*)&buffer[1];
        *mem_unaligned = 0x9ABC;
        uint16_t reg_word = 0xDEF0;
        uint16_t expected_sum = *mem_unaligned + reg_word;
        uint16_t original_mem = *mem_unaligned;
        
        __asm__ volatile (
            "xaddw %[reg_word], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_unaligned), [reg_word] "+r" (reg_word), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%x, Register=0x%x -> ", original_mem, 0xDEF0);
        printf("Result: Memory=0x%x, Register=0x%x\n", *mem_unaligned, reg_word);
        
        if (*mem_unaligned != expected_sum || reg_word != original_mem) {
            printf("      ERROR: Expected Memory=0x%x (sum), Register=0x%x (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    } else if (size == 4) {
        uint32_t *mem_unaligned = (uint32_t*)&buffer[1];
        *mem_unaligned = 0x11223344;
        uint32_t reg_dword = 0x55667788;
        uint32_t expected_sum = *mem_unaligned + reg_dword;
        uint32_t original_mem = *mem_unaligned;
        
        __asm__ volatile (
            "xaddl %[reg_dword], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_unaligned), [reg_dword] "+r" (reg_dword), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%x, Register=0x%x -> ", original_mem, 0x55667788);
        printf("Result: Memory=0x%x, Register=0x%x\n", *mem_unaligned, reg_dword);
        
        if (*mem_unaligned != expected_sum || reg_dword != original_mem) {
            printf("      ERROR: Expected Memory=0x%x (sum), Register=0x%x (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    } else {
        uint64_t *mem_unaligned = (uint64_t*)&buffer[1];
        *mem_unaligned = 0x1122334455667788;
        uint64_t reg_qword = 0x99AABBCCDDEEFF00;
        uint64_t expected_sum = *mem_unaligned + reg_qword;
        uint64_t original_mem = *mem_unaligned;
        
        __asm__ volatile (
            "xaddq %[reg_qword], %[mem]\n\t"
            "pushfq\n\t"
            "pop %[eflags]"
            : [mem] "+m" (*mem_unaligned), [reg_qword] "+r" (reg_qword), [eflags] "=r" (eflags)
            : 
            : "cc"
        );
        
        printf("    Value=0x%lx, Register=0x%lx -> ", original_mem, 0x99AABBCCDDEEFF00);
        printf("Result: Memory=0x%lx, Register=0x%lx\n", *mem_unaligned, reg_qword);
        
        if (*mem_unaligned != expected_sum || reg_qword != original_mem) {
            printf("      ERROR: Expected Memory=0x%lx (sum), Register=0x%lx (original memory)\n", expected_sum, original_mem);
            success = false;
        }
        print_xadd_flags(eflags);
    }
    
    if (success) {
        printf("All %s tests passed!\n", size_str);
    } else {
        printf("Some %s tests failed!\n", size_str);
    }
    printf("--------------------------------\n");
}

int main() {
    printf("XADD instruction test\n");
    printf("=====================\n");
    
    test_xadd_size(1);  // byte
    test_xadd_size(2);  // word
    test_xadd_size(4);  // dword
    test_xadd_size(8);  // qword
    
    printf("\nXADD test completed\n");
    printf("===================\n");
    
    return 0;
}
