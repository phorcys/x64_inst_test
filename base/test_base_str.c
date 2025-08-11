#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

static jmp_buf env;

void handle_sigill(int sig) {
    (void)sig;  // Mark parameter as unused to avoid warning
    longjmp(env, 1);
}

void handle_sigsegv(int sig) {
    (void)sig;  // Mark parameter as unused to avoid warning
    longjmp(env, 1);
}

int main() {
    printf("STR instruction comprehensive test\n");
    printf("=================================\n");
    
    struct sigaction sa_ill, sa_segv, old_sa_ill, old_sa_segv;
    int errors = 0;
    uint16_t tr_value_mem;
    uint16_t tr_value_reg16;
    uint32_t tr_value_reg32;
    uint64_t tr_value_reg64;

    // Set up signal handlers to catch privilege violations
    sa_ill.sa_handler = handle_sigill;
    sigemptyset(&sa_ill.sa_mask);
    sa_ill.sa_flags = 0;
    sigaction(SIGILL, &sa_ill, &old_sa_ill);

    sa_segv.sa_handler = handle_sigsegv;
    sigemptyset(&sa_segv.sa_mask);
    sa_segv.sa_flags = 0;
    sigaction(SIGSEGV, &sa_segv, &old_sa_segv);

    if (setjmp(env) == 0) {
        // Test storing TR to memory
        __asm__ __volatile__("str %0" : "=m" (tr_value_mem));
        printf("STR to memory: 0x%04x\n", tr_value_mem);
    } else {
        printf("STR to memory: #GP encountered (privilege violation)\n");
        errors++;
    }

    if (setjmp(env) == 0) {
        // Test storing TR to 16-bit register
        __asm__ __volatile__("str %w0" : "=r" (tr_value_reg16));
        printf("STR to 16-bit register: 0x%04x\n", tr_value_reg16);
    } else {
        printf("STR to 16-bit register: #GP encountered (privilege violation)\n");
        errors++;
    }

    if (setjmp(env) == 0) {
        // Test storing TR to 32-bit register
        __asm__ __volatile__("str %0" : "=r" (tr_value_reg32));
        printf("STR to 32-bit register: 0x%08x\n", tr_value_reg32);
        
        // Check that upper 16 bits are cleared
        if ((tr_value_reg32 >> 16) != 0) {
            printf("  Error: Upper 16 bits not cleared (got 0x%08x)\n", tr_value_reg32);
            errors++;
        }
    } else {
        printf("STR to 32-bit register: #GP encountered (privilege violation)\n");
        errors++;
    }

    if (setjmp(env) == 0) {
        // Test storing TR to 64-bit register
        __asm__ __volatile__("str %0" : "=r" (tr_value_reg64));
        printf("STR to 64-bit register: 0x%016lx\n", tr_value_reg64);
        
        // Check that upper 48 bits are cleared
        if ((tr_value_reg64 >> 16) != 0) {
            printf("  Error: Upper 48 bits not cleared (got 0x%016lx)\n", tr_value_reg64);
            errors++;
        }
    } else {
        printf("STR to 64-bit register: #GP encountered (privilege violation)\n");
        errors++;
    }

    // Restore original signal handlers
    sigaction(SIGILL, &old_sa_ill, NULL);
    sigaction(SIGSEGV, &old_sa_segv, NULL);

    printf("\nSTR test completed\n");
    printf("==================\n");
    printf("Total tests: 4\n");
    printf("Failed tests: %d\n", errors);
    
    return errors ? 1 : 0;
}
