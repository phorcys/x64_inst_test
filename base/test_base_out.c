#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/signal.h>
#include "base.h"

// Global jump buffer for signal handling
static sigjmp_buf g_env;

// Signal handler for segmentation faults
void sigsegv_handler(int sig) {
    (void)sig; // Avoid unused parameter warning
    // Re-establish the signal handler for the next test
    signal(SIGSEGV, sigsegv_handler);
    // Jump back to the saved context
    siglongjmp(g_env, 1);
}

int main() {
    printf("OUT instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Setup signal handler using simple signal() function
    signal(SIGSEGV, sigsegv_handler);
    
    // Test 8-bit OUT (should cause SIGSEGV)
    if (sigsetjmp(g_env, 1) == 0) {
        signal(SIGSEGV, sigsegv_handler);
        uint8_t value = 0x42;
        __asm__ __volatile__ (
            "mov %[val], %%al\n\t"
            "out %%al, $0x80"
            : 
            : [val] "r" (value)
            : "al"
        );
        // If we get here, no SIGSEGV occurred (error)
        printf("Error: 8-bit OUT did not cause segmentation fault\n");
        errors++;
    } else {
        printf("8-bit OUT: PASS (segmentation fault as expected)\n");
    }
    
    // Test 16-bit OUT (should cause SIGSEGV)
    if (sigsetjmp(g_env, 1) == 0) {
        signal(SIGSEGV, sigsegv_handler);
        uint16_t value = 0x1234;
        __asm__ __volatile__ (
            "mov %[val], %%ax\n\t"
            "out %%ax, $0x80"
            : 
            : [val] "r" (value)
            : "ax"
        );
        printf("Error: 16-bit OUT did not cause segmentation fault\n");
        errors++;
    } else {
        printf("16-bit OUT: PASS (segmentation fault as expected)\n");
    }
    
    // Test 32-bit OUT (should cause SIGSEGV)
    if (sigsetjmp(g_env, 1) == 0) {
        signal(SIGSEGV, sigsegv_handler);
        uint32_t value = 0xDEADBEEF;
        __asm__ __volatile__ (
            "mov %[val], %%eax\n\t"
            "out %%eax, $0x80"
            : 
            : [val] "r" (value)
            : "eax"
        );
        printf("Error: 32-bit OUT did not cause segmentation fault\n");
        errors++;
    } else {
        printf("32-bit OUT: PASS (segmentation fault as expected)\n");
    }
    
    // Test 8-bit DX addressing
    if (sigsetjmp(g_env, 1) == 0) {
        signal(SIGSEGV, sigsegv_handler);
        uint8_t value = 0x55;
        uint16_t port = 0x80;
        __asm__ __volatile__ (
            "mov %[port], %%dx\n\t"
            "mov %[val], %%al\n\t"
            "out %%al, %%dx"
            : 
            : [val] "r" (value), [port] "r" (port)
            : "dx", "al"
        );
        printf("Error: 8-bit DX OUT did not cause segmentation fault\n");
        errors++;
    } else {
        printf("8-bit DX OUT: PASS (segmentation fault as expected)\n");
    }
    
    // Test 16-bit DX addressing
    if (sigsetjmp(g_env, 1) == 0) {
        signal(SIGSEGV, sigsegv_handler);
        uint16_t value = 0x1234;
        uint16_t port = 0x81;
        __asm__ __volatile__ (
            "mov %[port], %%dx\n\t"
            "mov %[val], %%ax\n\t"
            "out %%ax, %%dx"
            : 
            : [val] "r" (value), [port] "r" (port)
            : "dx", "ax"
        );
        printf("Error: 16-bit DX OUT did not cause segmentation fault\n");
        errors++;
    } else {
        printf("16-bit DX OUT: PASS (segmentation fault as expected)\n");
    }
    
    // Test 32-bit DX addressing
    if (sigsetjmp(g_env, 1) == 0) {
        signal(SIGSEGV, sigsegv_handler);
        uint32_t value = 0xDEADBEEF;
        uint16_t port = 0x82;
        __asm__ __volatile__ (
            "mov %[port], %%dx\n\t"
            "mov %[val], %%eax\n\t"
            "out %%eax, %%dx"
            : 
            : [val] "r" (value), [port] "r" (port)
            : "dx", "eax"
        );
        printf("Error: 32-bit DX OUT did not cause segmentation fault\n");
        errors++;
    } else {
        printf("32-bit DX OUT: PASS (segmentation fault as expected)\n");
    }
    
    // Restore default signal handler
    signal(SIGSEGV, SIG_DFL);
    
    printf("\nOUT test completed\n");
    printf("==================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
