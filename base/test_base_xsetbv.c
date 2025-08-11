#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>
#include "base.h"

static sigjmp_buf env;
static volatile sig_atomic_t got_sigsegv = 0;
static volatile sig_atomic_t got_sigill = 0;

void handle_sig(int sig) {
    if (sig == SIGSEGV) {
        got_sigsegv = 1;
    } else if (sig == SIGILL) {
        got_sigill = 1;
    }
    siglongjmp(env, 1);
}

// Function to test XSETBV instruction
static void test_xsetbv() {
    // Check CPUID for XSAVE support
    uint32_t cpuid_ecx;
    asm volatile (
        "mov $1, %%eax \n\t"
        "cpuid \n\t"
        "mov %%ecx, %0"
        : "=r" (cpuid_ecx)
        :
        : "eax", "ebx", "ecx", "edx"
    );
    
    if ((cpuid_ecx & (1 << 26)) == 0) {
        printf("CPU does not support XSAVE - test skipped\n");
        return;
    }
    
    // Get XCR0 value
    uint32_t eax_xcr0, edx_xcr0;
    uint64_t xcr0;
    asm volatile (
        "xgetbv"
        : "=a" (eax_xcr0), "=d" (edx_xcr0)
        : "c" (0)
    );
    xcr0 = ((uint64_t)edx_xcr0 << 32) | eax_xcr0;
    
    // Set up signal handlers
    struct sigaction sa, old_sa_segv, old_sa_ill;
    
    sa.sa_handler = handle_sig;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGSEGV, &sa, &old_sa_segv) == -1) {
        perror("sigaction(SIGSEGV) failed");
        return;
    }
    if (sigaction(SIGILL, &sa, &old_sa_ill) == -1) {
        perror("sigaction(SIGILL) failed");
        return;
    }
    
    int no_fault = 0;
    if (sigsetjmp(env, 1) == 0) {
        // Attempt to execute XSETBV - should cause #GP fault
        uint32_t eax = (uint32_t)xcr0;
        uint32_t edx = (uint32_t)(xcr0 >> 32);
        asm volatile (
            "xsetbv"
            : 
            : "c" (0), "a" (eax), "d" (edx)
        );
        no_fault = 1;  // If we get here, no fault occurred
    }
    
    // Check results
    if (no_fault) {
        printf("  ERROR: XSETBV executed without fault in user mode\n");
    } else if (got_sigsegv) {
        printf("  Got expected SIGSEGV (GP fault) - PASS\n");
        printf("  XSETBV test: PASS\n");
    } else if (got_sigill) {
        printf("  ERROR: Got unexpected SIGILL\n");
    } else {
        printf("  ERROR: No signal was raised\n");
    }
    
    // Restore signal handlers
    sigaction(SIGSEGV, &old_sa_segv, NULL);
    sigaction(SIGILL, &old_sa_ill, NULL);
}

int main() {
    printf("XSETBV instruction test\n");
    printf("=======================\n");
    
    test_xsetbv();
    
    printf("\nXSETBV test completed\n");
    printf("=====================\n");
    
    return 0;
}
