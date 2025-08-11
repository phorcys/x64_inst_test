#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

static sigjmp_buf jmpbuf;
static volatile int sigill_received = 0;

static void handle_sigill(int sig) {
    (void)sig;
    sigill_received = 1;
    siglongjmp(jmpbuf, 1);
}

int main() {
    printf("WRMSR instruction test\n");
    printf("======================\n");
    
    // Set up signal handlers for SIGILL and SIGSEGV
    struct sigaction sa_old_ill, sa_old_segv, sa_new;
    sa_new.sa_handler = handle_sigill;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = 0;
    
    if (sigaction(SIGILL, &sa_new, &sa_old_ill) != 0) {
        perror("sigaction for SIGILL");
        return 1;
    }
    if (sigaction(SIGSEGV, &sa_new, &sa_old_segv) != 0) {
        perror("sigaction for SIGSEGV");
        return 1;
    }
    
    if (sigsetjmp(jmpbuf, 1)) {
        // We come here after signal
        if (sigill_received) {
            printf("Received SIGILL (expected in user mode)\n");
        } else {
            printf("Received SIGSEGV (expected in user mode)\n");
        }
    } else {
        // Try to execute WRMSR
        printf("Attempting to execute WRMSR...\n");
        sigill_received = 0;
        
        // Use inline assembly to execute WRMSR
        // Using MSR 0x1B (APIC_BASE) as a safe test
        __asm__ volatile (
            "mov $0x1B, %%ecx\n\t"
            "xor %%edx, %%edx\n\t"
            "xor %%eax, %%eax\n\t"
            "wrmsr"
            : : : "ecx", "edx", "eax", "cc"
        );
        
        if (!sigill_received) {
            printf("WRMSR executed without generating signal\n");
            printf("This may indicate emulation support\n");
        }
    }
    
    // Restore original signal handlers
    sigaction(SIGILL, &sa_old_ill, NULL);
    sigaction(SIGSEGV, &sa_old_segv, NULL);
    
    printf("\nWRMSR test completed\n");
    printf("===================\n");
    
    return 0;
}
