#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    int errors = 0;
    uint64_t saved_rbp, saved_rsp;
    uint64_t after_leave_rbp, after_leave_rsp;

    // Save current RBP and RSP
    __asm__ __volatile__ (
        "mov %%rbp, %0\n\t"
        "mov %%rsp, %1\n\t"
        : "=r" (saved_rbp), "=r" (saved_rsp)
    );

    // Set up a stack frame
    __asm__ __volatile__ (
        "push %%rbp\n\t"            // Save current base pointer
        "mov %%rsp, %%rbp\n\t"      // Set new base pointer
        "sub $0x20, %%rsp\n\t"      // Allocate 32 bytes on stack
        "leave\n\t"                 // Clean up stack frame (mov rsp, rbp; pop rbp)
        "mov %%rbp, %0\n\t"
        "mov %%rsp, %1\n\t"
        : "=r" (after_leave_rbp), "=r" (after_leave_rsp)
        : 
        : "rbp", "rsp"
    );

    printf("LEAVE instruction test\n");
    printf("======================\n\n");
    
    // Verify RBP was restored
    if (after_leave_rbp != saved_rbp) {
        printf("\nERROR: RBP not restored correctly! Difference: 0x%016lx\n", 
               after_leave_rbp - saved_rbp);
        errors++;
    } else {
        printf("\nRBP restoration: PASS\n");
    }
    
    // Verify RSP was restored
    if (after_leave_rsp != saved_rsp) {
        printf("ERROR: RSP not restored correctly! Difference: 0x%016lx\n",
               after_leave_rsp - saved_rsp);
        errors++;
    } else {
        printf("RSP restoration: PASS\n");
    }
    
    printf("\nLEAVE test completed\n");
    printf("====================\n");
    printf("Total tests: 2\n");
    printf("Passed: %d\n", 2 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
