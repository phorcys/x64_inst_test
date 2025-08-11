#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Test GS segment prefix with various memory operations
int main() {
    printf("Testing GS segment prefix:\n");
    printf("==========================\n");
    
    uint64_t buffer[2] = {0x123456789ABCDEF0, 0x0FEDCBA987654321};
    uint64_t gs_value;
    
    // Test GS prefix with MOV
    asm volatile (
        "movq $0x1122334455667788, %%rax\n\t"
        "movq %%rax, %[buffer]\n\t"
        "gs movq %[buffer], %%rbx\n\t"
        "movq %%rbx, %[gs_value]"
        : [gs_value] "=m" (gs_value)
        : [buffer] "r" (buffer)
        : "rax", "rbx", "memory"
    );
    
    printf("GS MOV result: 0x%016lX\n", gs_value);
    printf("Original value: 0x%016lX\n", buffer[0]);
    
    // Test GS prefix with ADD
    uint64_t add_result;
    asm volatile (
        "movq $100, %%rax\n\t"
        "gs addq $50, %%rax\n\t"
        "movq %%rax, %[result]"
        : [result] "=m" (add_result)
        :
        : "rax"
    );
    printf("GS ADD result: %lu\n", add_result);
    
    // Test GS prefix with CMP
    uint64_t cmp_flags;
    asm volatile (
        "movq $200, %%rax\n\t"
        "gs cmpq $200, %%rax\n\t"
        "pushfq\n\t"
        "popq %[flags]"
        : [flags] "=r" (cmp_flags)
        :
        : "rax", "cc"
    );
    printf("GS CMP flags: 0x%016lX\n", cmp_flags & 0x8D5);
    
    printf("\nGS tests completed.\n");
    return 0;
}
