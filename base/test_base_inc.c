#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Function to test INC on memory operands
static void test_mem_inc() {
    uint8_t byte_val = 0;
    uint16_t word_val = 0;
    uint32_t dword_val = 0;
    uint64_t qword_val = 0;
    uint64_t eflags;
    
    printf("Testing INC on memory operands:\n");
    printf("-------------------------------\n");
    
    // Test byte
    byte_val = 0xFE;
    asm volatile (
        "incb %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+m" (byte_val)
        :
        : "cc"
    );
    printf("INC byte (0xFE -> 0xFF): 0x%02X\n", byte_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    asm volatile (
        "incb %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+m" (byte_val)
        :
        : "cc"
    );
    printf("INC byte (0xFF -> 0x00): 0x%02X\n", byte_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    // Test word
    word_val = 0x7FFF;
    asm volatile (
        "incw %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+m" (word_val)
        :
        : "cc"
    );
    printf("INC word (0x7FFF -> 0x8000): 0x%04X\n", word_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    // Test dword
    dword_val = 0xFFFFFFFF;
    asm volatile (
        "incl %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+m" (dword_val)
        :
        : "cc"
    );
    printf("INC dword (0xFFFFFFFF -> 0x00000000): 0x%08X\n", dword_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    // Test qword
    qword_val = 0x7FFFFFFFFFFFFFFF;
    asm volatile (
        "incq %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+m" (qword_val)
        :
        : "cc"
    );
    printf("INC qword (0x7FFFFFFFFFFFFFFF -> 0x8000000000000000): 0x%016lX\n", qword_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    printf("\n");
}

// Function to test INC on register operands
static void test_reg_inc() {
    uint8_t byte_val = 0;
    uint16_t word_val = 0;
    uint32_t dword_val = 0;
    uint64_t qword_val = 0;
    uint64_t eflags;
    
    printf("Testing INC on register operands:\n");
    printf("--------------------------------\n");
    
    // Test byte
    byte_val = 0x41;
    asm volatile (
        "incb %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+r" (byte_val)
        :
        : "cc"
    );
    printf("INC AL (0x41 -> 0x42): 0x%02X\n", byte_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    // Test word
    word_val = 0xFFFF;
    asm volatile (
        "incw %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+r" (word_val)
        :
        : "cc"
    );
    printf("INC AX (0xFFFF -> 0x0000): 0x%04X\n", word_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    // Test dword
    dword_val = 0x80000000;
    asm volatile (
        "incl %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+r" (dword_val)
        :
        : "cc"
    );
    printf("INC EAX (0x80000000 -> 0x80000001): 0x%08X\n", dword_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    
    // Test qword
    qword_val = 0;
    asm volatile (
        "incq %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (eflags), "+r" (qword_val)
        :
        : "cc"
    );
    printf("INC RAX (0x0 -> 0x1): 0x%016lX\n", qword_val);
    printf("Flags:\n");
    print_eflags_cond(eflags, X_OF | X_SF | X_ZF | X_AF | X_PF);
    printf("\n");
}

int main() {
    printf("Starting INC instruction tests\n");
    printf("=============================\n\n");
    
    test_mem_inc();
    test_reg_inc();
    
    printf("All INC tests completed.\n");
    return 0;
}
