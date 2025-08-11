#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "base.h"

// SWAPGS test requires kernel-level execution
// We'll test it by attempting to execute and handling any faults

static jmp_buf jmp_env;

static void handle_sigsegv(int sig __attribute__((unused))) {
    siglongjmp(jmp_env, 1);
}

int main() {
    printf("SWAPGS instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    
    // Set up signal handler to catch privilege violations
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigsegv;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGSEGV, &sa, NULL);
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        // Attempt SWAPGS - should cause #GP in user mode
        __asm__ volatile ("swapgs");
        printf("  ERROR: SWAPGS executed without privilege violation\n");
        errors++;
    } else {
        printf("  Privilege violation correctly detected\n");
    }
    
    // Note: Actual GS base swap would require kernel mode testing
    // This is beyond the scope of a user-space test
    
    printf("\nSWAPGS test completed\n");
    printf("====================\n");
    printf("Total tests: 1\n");
    printf("Failed tests: %d\n", errors);
    
    return errors ? 1 : 0;
}
