#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper functions for each test
int test_64bit_register_pop() {
    uint64_t temp_stack[16];
    uint64_t *stack_top = temp_stack + 15;
    uint64_t test_value = 0x123456789ABCDEF0;
    uint64_t popped_value;
    *stack_top = test_value;
    
    uint64_t saved_rsp;
    __asm__ __volatile__ (
        "mov %%rsp, %1\n\t"
        "mov %2, %%rsp\n\t"
        "pop %0\n\t"
        "mov %1, %%rsp"
        : "=r" (popped_value), "=&r" (saved_rsp)
        : "r" (stack_top)
        : "memory"
    );
    
    return popped_value == test_value;
}

int test_16bit_register_pop() {
    uint64_t temp_stack[16];
    uint64_t *stack_top = temp_stack + 15;
    uint16_t test_value = 0xBEEF;
    uint16_t popped_value;
    *stack_top = test_value;
    
    uint64_t saved_rsp;
    __asm__ __volatile__ (
        "mov %%rsp, %1\n\t"
        "mov %2, %%rsp\n\t"
        "popw %0\n\t"
        "mov %1, %%rsp"
        : "=r" (popped_value), "=&r" (saved_rsp)
        : "r" (stack_top)
        : "memory"
    );
    
    return popped_value == test_value;
}

int test_64bit_memory_pop() {
    uint64_t temp_stack[16];
    uint64_t *stack_top = temp_stack + 15;
    uint64_t test_value = 0xAABBCCDD11223344;
    uint64_t popped_value = 0;
    *stack_top = test_value;
    
    uint64_t saved_rsp;
    __asm__ __volatile__ (
        "mov %%rsp, %1\n\t"
        "mov %2, %%rsp\n\t"
        "pop %0\n\t"
        "mov %1, %%rsp"
        : "=m" (popped_value), "=&r" (saved_rsp)
        : "r" (stack_top)
        : "memory"
    );
    
    return popped_value == test_value;
}

int test_16bit_memory_pop() {
    uint64_t temp_stack[16];
    uint64_t *stack_top = temp_stack + 15;
    uint16_t test_value = 0x55AA;
    uint16_t popped_value = 0;
    *stack_top = test_value;
    
    uint64_t saved_rsp;
    __asm__ __volatile__ (
        "mov %%rsp, %1\n\t"
        "mov %2, %%rsp\n\t"
        "popw %0\n\t"
        "mov %1, %%rsp"
        : "=m" (popped_value), "=&r" (saved_rsp)
        : "r" (stack_top)
        : "memory"
    );
    
    return popped_value == test_value;
}

int main() {
    int errors = 0;
    
    printf("POP instruction test\n");
    printf("====================\n");
    
    // Test POP to 64-bit register
    if (test_64bit_register_pop()) {
        printf("64-bit register POP: PASS\n");
    } else {
        printf("Error: 64-bit register POP failed\n");
        errors++;
    }
    
    // Test POP to 16-bit register
    if (test_16bit_register_pop()) {
        printf("16-bit register POP: PASS\n");
    } else {
        printf("Error: 16-bit register POP failed\n");
        errors++;
    }
    
    // Test POP to 64-bit memory
    if (test_64bit_memory_pop()) {
        printf("64-bit memory POP: PASS\n");
    } else {
        printf("Error: 64-bit memory POP failed\n");
        errors++;
    }
    
    // Test POP to 16-bit memory
    if (test_16bit_memory_pop()) {
        printf("16-bit memory POP: PASS\n");
    } else {
        printf("Error: 16-bit memory POP failed\n");
        errors++;
    }
    
    // Skipping POP FS and POP GS tests due to user mode restrictions
    printf("Skipping POP FS and POP GS tests in user mode\n");
    
    printf("\nPOP test completed\n");
    printf("==================\n");
    printf("Total tests: 4\n");
    printf("Passed: %d\n", 4 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
