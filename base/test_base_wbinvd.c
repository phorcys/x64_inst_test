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
    printf("WBINVD instruction test\n");
    printf("=======================\n");
    
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
        // Try to execute WBINVD
        printf("Attempting to execute WBINVD...\n");
        sigill_received = 0;
        
        // Use inline assembly to execute WBINVD
        __asm__ volatile ("wbinvd");
        
        if (!sigill_received) {
            printf("WBINVD executed without generating SIGILL\n");
            printf("This may indicate emulation support\n");
        }
    }
    
    // Restore original signal handlers
    sigaction(SIGILL, &sa_old_ill, NULL);
    sigaction(SIGSEGV, &sa_old_segv, NULL);
    
    printf("\nWBINVD test completed\n");
    printf("====================\n");
    
    return 0;
}
