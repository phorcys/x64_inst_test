#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("NOP instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Test register unchanged
    uint64_t value = 0x123456789ABCDEF0;
    uint64_t result;
    
    __asm__ __volatile__ (
        "mov %[val], %%rax\n\t"
        "nop\n\t"
        "mov %%rax, %[res]"
        : [res] "=r" (result)
        : [val] "r" (value)
        : "rax"
    );
    
    if (result != value) {
        printf("Error: Register value changed! Expected 0x%lx, got 0x%lx\n", value, result);
        errors++;
    } else {
        printf("Register test: PASS\n");
    }
    
    // Test memory unchanged
    uint64_t mem_value = 0x13579BDF2468ACE0;
    uint64_t mem_result = mem_value;
    
    __asm__ __volatile__ (
        "nop\n\t"
        : 
        : "m" (mem_result)  // to prevent the compiler from optimizing away the memory
        : 
    );
    
    if (mem_result != mem_value) {
        printf("Error: Memory value changed! Expected 0x%lx, got 0x%lx\n", mem_value, mem_result);
        errors++;
    } else {
        printf("Memory test: PASS\n");
    }
    
    // Test multiple NOPs
    __asm__ __volatile__ (
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop"
        : 
        : 
        : 
    );
    
    // If we get here without crashing, it passed
    printf("Multiple NOPs execution: PASS\n");
    
    // Test NOP with different operand sizes (operand is ignored)
    uint32_t eax_val = 0xAABBCCDD;
    __asm__ __volatile__ (
        "nopl 0(%%eax)" : : "a" (eax_val)
    );
    printf("NOP with operand: PASS\n");
    
    printf("\nNOP test completed\n");
    printf("==================\n");
    printf("Total tests: 4\n");
    printf("Passed: %d\n", 4 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
