#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include "base.h"

// Signal handler for SIGSEGV
void handle_sigsegv(int sig) {
    (void)sig; // Suppress unused parameter warning
    printf("Caught SIGSEGV (Segmentation Fault)\n");
    _exit(0); // Exit immediately after handling the signal
}

// Test HLT instruction
int main() {
    printf("Testing HLT:\n");
    printf("============\n");
    
    // Register signal handler
    signal(SIGSEGV, handle_sigsegv);
    
    printf("Attempting to execute HLT instruction...\n");
    fflush(stdout);
    
    // Try to execute HLT
    asm volatile (
        "hlt\n\t"
        : 
        : 
        : 
    );
    
    // This line will only execute if HLT didn't trigger SIGILL
    printf("Error: HLT executed without triggering SIGILL\n");
    return 1;
}
