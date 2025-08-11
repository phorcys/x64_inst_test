#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("MOV instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Test register to register moves
    uint64_t rax_val = 0x123456789ABCDEF0;
    uint64_t rbx_val = 0;
    __asm__ __volatile__ (
        "mov %1, %%rax\n\t"
        "mov %%rax, %0"
        : "=r" (rbx_val)
        : "r" (rax_val)
        : "rax"
    );
    if (rax_val != rbx_val) {
        printf("Error: Register-register move expected 0x%lx, got 0x%lx\n", rax_val, rbx_val);
        errors++;
    } else {
        printf("Register-register move: PASS\n");
    }
    
    // Test memory to register
    uint64_t mem_val = 0x13579BDF2468ACE0;
    uint64_t reg_val;
    __asm__ __volatile__ (
        "mov %1, %0"
        : "=r" (reg_val)
        : "m" (mem_val)
    );
    if (reg_val != mem_val) {
        printf("Error: Memory-register move expected 0x%lx, got 0x%lx\n", mem_val, reg_val);
        errors++;
    } else {
        printf("Memory-register move: PASS\n");
    }
    
    // Test register to memory
    uint64_t new_mem = 0;
    uint64_t new_val = 0x2468ACE013579BDF;
    __asm__ __volatile__ (
        "mov %1, %0"
        : "=m" (new_mem)
        : "r" (new_val)
    );
    if (new_mem != new_val) {
        printf("Error: Register-memory move expected 0x%lx, got 0x%lx\n", new_val, new_mem);
        errors++;
    } else {
        printf("Register-memory move: PASS\n");
    }
    
    // Test immediate to register
    uint64_t imm_val;
    __asm__ __volatile__ (
        "mov $0x1122334455667788, %0"
        : "=r" (imm_val)
    );
    if (imm_val != 0x1122334455667788) {
        printf("Error: Immediate-register move expected 0x1122334455667788, got 0x%lx\n", imm_val);
        errors++;
    } else {
        printf("Immediate-register move: PASS\n");
    }
    
    // Test different widths
    uint32_t eax_val;
    uint16_t ax_val;
    uint8_t al_val;
    
    __asm__ __volatile__ (
        "mov $0xAABBCCDD, %%eax\n\t"
        "mov %%ax, %0\n\t"
        "mov %%al, %1"
        : "=r" (ax_val), "=r" (al_val)
        : 
        : "eax"
    );
    
    if (ax_val != 0xCCDD) {
        printf("Error: 16-bit move expected 0xCCDD, got 0x%x\n", ax_val);
        errors++;
    }
    if (al_val != 0xDD) {
        printf("Error: 8-bit move expected 0xDD, got 0x%x\n", al_val);
        errors++;
    }
    if (ax_val == 0xCCDD && al_val == 0xDD) {
        printf("Different width moves: PASS\n");
    }
    
    printf("\nMOV test completed\n");
    printf("==================\n");
    printf("Total tests: 5\n");
    printf("Passed: %d\n", 5 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
