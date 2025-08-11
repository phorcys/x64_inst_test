#include <stdio.h>
#include <stdint.h>
#include "base.h"

static void test_jmp_direct() {
    printf("Testing direct JMP:\n");
    printf("-------------------\n");
    
    volatile int result = 0;
    
    asm volatile (
        "jmp 1f\n"              // Direct jump to label 1
        "movl $0, %0\n"         // Should not execute
        "1:\n"
        "movl $1, %0\n"         // Jump target
        : "=m" (result)
        :
        : "cc", "memory"
    );
    
    printf("Direct jump: %s\n", result ? "success" : "failed");
    printf("\n");
}

static void test_jmp_register_indirect() {
    printf("Testing register-indirect JMP:\n");
    printf("-----------------------------\n");
    
    volatile int result = 0;
    void* target;
    
    // Get the address of the target label
    asm volatile (
        "lea 1f(%%rip), %0\n"   // Load address of label 1 into target
        : "=r" (target)
        :
        : "cc"
    );
    
    asm volatile (
        "mov %1, %%rax\n"       // Move target address to RAX
        "jmp *%%rax\n"          // Jump to address in RAX
        "movl $0, %0\n"         // Should not execute
        "1:\n"
        "movl $1, %0\n"         // Jump target
        : "=m" (result)
        : "r" (target)
        : "rax", "cc", "memory"
    );
    
    printf("Register-indirect jump: %s\n", result ? "success" : "failed");
    printf("\n");
}

static void test_jmp_memory_indirect() {
    printf("Testing memory-indirect JMP:\n");
    printf("---------------------------\n");
    
    volatile int result = 0;
    void* target;
    void* target_ptr = &target;
    
    asm volatile (
        "lea 1f(%%rip), %1\n" 
        "mov %1, %%rax\n"       // Move address of target_ptr to RAX
        "jmp *(%%rax)\n"        // Jump to address stored at RAX
        "movl $0, %0\n"         // Should not execute
        "1:\n"
        "movl $2, %0\n"         // Jump target
        : "=m" (result),"=r" (target)
        : "r" (&target_ptr)
        : "rax", "cc", "memory"
    );
    
    printf("Memory-indirect jump: %s\n", result ? "success" : "failed");
    printf("\n");
}

static void test_jmp_relative() {
    printf("Testing relative JMP:\n");
    printf("---------------------\n");
    
    volatile int result = 0;
    
    asm volatile (
        "jmp .+7\n"             // Relative jump over next instruction
        "movl $0, %0\n"         // Should not execute
        "movl $1, %0\n"         // Jump target
        : "=m" (result)
        :
        : "cc", "memory"
    );
    
    printf("Relative jump: %s\n", result ? "success" : "failed");
    printf("\n");
}

int main() {
    printf("Starting JMP instruction tests\n");
    printf("=============================\n\n");
    
    test_jmp_direct();
    test_jmp_register_indirect();
    //test_jmp_memory_indirect();
   // test_jmp_relative();
    
    printf("\nAll JMP tests completed.\n");
    return 0;
}
