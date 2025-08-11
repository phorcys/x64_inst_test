#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to check ZF flag after VERR instruction
static void check_verr_result(int expected, uint64_t eflags) {
    int zf = (eflags & X_ZF) != 0;
    printf("  Expected ZF: %d, Actual ZF: %d\n", expected, zf);
    if (zf != expected) {
        printf("    ERROR: ZF expected %d, got %d\n", expected, zf);
    } else {
        printf("    OK\n");
    }
}

// Test VERR with register operand
static void test_verr_reg() {
    printf("Testing VERR with register operand:\n");
    
    uint16_t seg_reg;
    uint64_t eflags;
    
    // Test valid readable segment (CS register)
    __asm__ volatile (
        "mov %%cs, %[seg]\n\t"
        "verr %[seg]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [seg] "=r" (seg_reg), [eflags] "=r" (eflags)
        : 
        : "cc"
    );
    printf("CS register: 0x%04x\n", seg_reg);
    check_verr_result(1, eflags);
    
    // Test invalid segment (0)
    __asm__ volatile (
        "mov $0, %[seg]\n\t"
        "verr %[seg]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [seg] "=r" (seg_reg), [eflags] "=r" (eflags)
        : 
        : "cc"
    );
    printf("Null selector: 0x%04x\n", seg_reg);
    check_verr_result(0, eflags);
}

// Test VERR with memory operand
static void test_verr_mem() {
    printf("Testing VERR with memory operand:\n");
    
    uint16_t seg_val;
    uint64_t eflags;
    
    // Use CS segment which we know is valid
    __asm__ volatile ("mov %%cs, %0" : "=r" (seg_val));
    
    // Test valid readable segment (CS)
    __asm__ volatile (
        "verr %[mem]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [eflags] "=r" (eflags)
        : [mem] "m" (seg_val)
        : "cc"
    );
    printf("CS selector: 0x%04x\n", seg_val);
    check_verr_result(1, eflags);
    
    // Test invalid segment (0)
    seg_val = 0;
    __asm__ volatile (
        "verr %[mem]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [eflags] "=r" (eflags)
        : [mem] "m" (seg_val)
        : "cc"
    );
    printf("Null selector: 0x%04x\n", seg_val);
    check_verr_result(0, eflags);
    
    // Test with explicit memory location
    uint16_t mem_location = 0;
    __asm__ volatile ("mov %%cs, %0" : "=r" (mem_location));
    
    __asm__ volatile (
        "verrw %0\n\t"
        "pushfq\n\t"
        "pop %1"
        : "=m" (mem_location), "=r" (eflags)
        : 
        : "cc"
    );
    printf("Explicit memory location: 0x%04x\n", mem_location);
    check_verr_result(1, eflags);
}

int main() {
    printf("VERR instruction test\n");
    printf("=====================\n");
    
    test_verr_reg();
    test_verr_mem();
    
    printf("\nVERR test completed\n");
    printf("==================\n");
    
    return 0;
}
