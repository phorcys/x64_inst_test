#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include "base.h"

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

static void test_xgetbv() {
    uint32_t eax = 0, edx = 0;
    uint64_t xcr_value;
    uint32_t xcr_reg = 0; // XCR0 register
    
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
    
    // Read XCR0 using XGETBV
    asm volatile (
        "xgetbv"
        : "=a" (eax), "=d" (edx)
        : "c" (xcr_reg)
        : "memory"
    );
    
    // Restore signal handlers
    sigaction(SIGILL, &old_sa_ill, NULL);
    sigaction(SIGSEGV, &old_sa_segv, NULL);
    
    if (got_sigill || got_sigsegv) {
        printf("  ERROR: XGETBV caused a segmentation fault or illegal instruction\n");
        return;
    }
    
    xcr_value = ((uint64_t)edx << 32) | eax;
    
    printf("XCR0 value: 0x%016llx\n", xcr_value);
    
    // Verify that at least the SSE and AVX bits are set (if supported)
    int sse_supported = (eax & 0x02) ? 1 : 0;
    int avx_supported = (eax & 0x04) ? 1 : 0;
    
    printf("  SSE state: %s\n", sse_supported ? "Supported" : "Not supported");
    printf("  AVX state: %s\n", avx_supported ? "Supported" : "Not supported");
    
    // Instead of reading CR4, we'll infer OSXSAVE support from XCR0
    int osxsave_enabled = (eax != 0 || edx != 0);
    
    if (osxsave_enabled) {
        if (!sse_supported) {
            printf("  ERROR: XSAVE enabled but SSE state not supported!\n");
        } else {
            printf("  XGETBV basic functionality: PASS\n");
        }
    } else {
        printf("  WARNING: XSAVE not enabled, XGETBV may not function properly\n");
    }
}

int main() {
    printf("XGETBV instruction test\n");
    printf("=======================\n");
    
    // Check CPUID for XSAVE support (required for XGETBV)
    uint32_t cpuid_ecx;
    asm volatile (
        "mov $1, %%eax \n\t"
        "cpuid \n\t"
        "mov %%ecx, %0"
        : "=r" (cpuid_ecx)
        :
        : "eax", "ebx", "ecx", "edx"
    );
    
    if (cpuid_ecx & (1 << 26)) {
        printf("CPU supports XSAVE feature\n");
        test_xgetbv();
    } else {
        printf("CPU does not support XSAVE feature - XGETBV test skipped\n");
    }
    
    printf("\nXGETBV test completed\n");
    printf("=====================\n");
    
    return 0;
}
