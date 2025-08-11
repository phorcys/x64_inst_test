#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "base.h"

#define XSTATE_SSE  (1 << 1)
#define XSTATE_AVX  (1 << 2)
#define XSAVE_HEADER_START 512

static volatile sig_atomic_t got_sigill = 0;
static volatile sig_atomic_t got_sigsegv = 0;

void handle_sigill(int sig) {
    (void)sig;
    got_sigill = 1;
}

void handle_sigsegv(int sig) {
    (void)sig;
    got_sigsegv = 1;
}

// Function to get XSAVE area size
static size_t get_xsave_size() {
    uint32_t eax, ebx, ecx, edx;
    asm volatile (
        "mov $0x0D, %%eax \n\t"
        "mov $0, %%ecx \n\t"
        "cpuid \n\t"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        :
        : 
    );
    return ebx;
}

// Function to test XRSTOR instruction
static void test_xrstor() {
    // Check CPUID for XSAVE and OSXSAVE support
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
    if ((cpuid_ecx & (1 << 27)) == 0) {
        printf("OS does not support XSAVE - test skipped\n");
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
    
    // Check if SSE state is supported
    if (!(xcr0 & XSTATE_SSE)) {
        printf("XCR0 does not enable SSE state - test skipped\n");
        return;
    }
    
    // Set up signal handlers
    struct sigaction sa_ill, old_sa_ill;
    struct sigaction sa_segv, old_sa_segv;
    
    sa_ill.sa_handler = handle_sigill;
    sigemptyset(&sa_ill.sa_mask);
    sa_ill.sa_flags = 0;
    sigaction(SIGILL, &sa_ill, &old_sa_ill);
    
    sa_segv.sa_handler = handle_sigsegv;
    sigemptyset(&sa_segv.sa_mask);
    sa_segv.sa_flags = 0;
    sigaction(SIGSEGV, &sa_segv, &old_sa_segv);
    
    // Allocate aligned XSAVE area
    size_t xsave_size = get_xsave_size();
    void *xsave_area = aligned_alloc(64, xsave_size);
    if (!xsave_area) {
        printf("Memory allocation failed\n");
        goto cleanup;
    }
    memset(xsave_area, 0, xsave_size);
    
    // Initialize MXCSR
    uint32_t original_mxcsr = 0;
    asm volatile("stmxcsr %0" : "=m" (original_mxcsr));
    
    // Set XSAVE header
    uint64_t *header = (uint64_t *)((char *)xsave_area + XSAVE_HEADER_START);
    header[0] = xcr0; // XSTATE_BV - valid components
    
    // Save initial state with XSAVE
    asm volatile (
        "xsave (%0)"
        : 
        : "r" (xsave_area), "a" ((uint32_t)xcr0), "d" ((uint32_t)(xcr0 >> 32))
        : "memory"
    );
    
    // Modify state to be restored
    uint32_t modified_mxcsr = original_mxcsr | 0x8000; // Set FTZ (Flush To Zero)
    asm volatile("ldmxcsr %0" : : "m" (modified_mxcsr));
    
    // Restore state with XRSTOR
    asm volatile (
        "xrstor (%0)"
        : 
        : "r" (xsave_area), "a" ((uint32_t)xcr0), "d" ((uint32_t)(xcr0 >> 32))
        : "memory"
    );
    
    // Check if we got any signals
    if (got_sigill || got_sigsegv) {
        printf("  ERROR: XRSTOR caused a signal\n");
        goto cleanup;
    }
    
    // Verify MXCSR was restored
    uint32_t restored_mxcsr = 0;
    asm volatile("stmxcsr %0" : "=m" (restored_mxcsr));
    
    if (restored_mxcsr == original_mxcsr) {
        printf("  MXCSR restored successfully: 0x%08x\n", restored_mxcsr);
        printf("  XRSTOR basic functionality: PASS\n");
    } else {
        printf("  ERROR: MXCSR not restored properly\n");
        printf("    Original: 0x%08x, Restored: 0x%08x\n", original_mxcsr, restored_mxcsr);
    }

cleanup:
    // Restore signal handlers
    sigaction(SIGILL, &old_sa_ill, NULL);
    sigaction(SIGSEGV, &old_sa_segv, NULL);
    free(xsave_area);
}

int main() {
    printf("XRSTOR instruction test\n");
    printf("=======================\n");
    
    test_xrstor();
    
    printf("\nXRSTOR test completed\n");
    printf("=====================\n");
    
    return 0;
}
