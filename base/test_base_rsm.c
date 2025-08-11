#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>
#include "base.h"

// Global jump buffer for signal handling
static sigjmp_buf env;

// Signal handler for illegal instruction
void handle_sigill(int sig) {
    printf("SIGILL caught - RSM instruction trapped as expected\n");
    siglongjmp(env, 1);
}

int main() {
    printf("RSM instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Set up signal handler for SIGILL
    struct sigaction sa;
    sa.sa_handler = handle_sigill;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGILL, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    
    // Test 1: Attempt to execute RSM
    printf("Attempting to execute RSM instruction...\n");
    
    if (sigsetjmp(env, 1) == 0) {
        // This is the initial setup - try to execute RSM
        __asm__ __volatile__("rsm");
        
        // If we get here, RSM didn't cause SIGILL - this is unexpected
        printf("Error: RSM executed without triggering SIGILL\n");
        errors++;
    } else {
        // SIGILL was caught - this is expected behavior
        printf("RSM execution trapped successfully: PASS\n");
    }
    
    // Test 2: Verify system state after attempted RSM
    // We'll check if we can still access memory and registers
    uint64_t test_value = 0x123456789ABCDEF0;
    uint64_t read_value;
    
    __asm__ __volatile__(
        "mov %1, %%rax\n\t"
        "mov %%rax, %0"
        : "=r" (read_value)
        : "r" (test_value)
        : "rax"
    );
    
    if (read_value != test_value) {
        printf("Error: Register access failed after RSM attempt. Expected 0x%lx, got 0x%lx\n",
               test_value, read_value);
        errors++;
    } else {
        printf("Register access after RSM attempt: PASS\n");
    }
    
    // Test 3: Memory access after attempted RSM
    uint64_t mem_buffer[2] = {0xDEADBEEF, 0xCAFEBABE};
    uint64_t mem_value;
    
    __asm__ __volatile__(
        "movq (%1), %0"
        : "=r" (mem_value)
        : "r" (mem_buffer)
    );
    
    if (mem_value != mem_buffer[0]) {
        printf("Error: Memory access failed after RSM attempt. Expected 0x%lx, got 0x%lx\n",
               mem_buffer[0], mem_value);
        errors++;
    } else {
        printf("Memory access after RSM attempt: PASS\n");
    }
    
    // Test 4: Control flow after attempted RSM
    int control_flow_test = 0;
    for (int i = 0; i < 5; i++) {
        control_flow_test++;
    }
    
    if (control_flow_test != 5) {
        printf("Error: Control flow broken after RSM attempt. Expected 5, got %d\n", control_flow_test);
        errors++;
    } else {
        printf("Control flow after RSM attempt: PASS\n");
    }
    
    // Test 5: System call after attempted RSM
    printf("Attempting system call after RSM...\n");
    int syscall_result = getpid();
    if (syscall_result <= 0) {
        printf("Error: System call failed after RSM attempt\n");
        errors++;
    } else {
        printf("System call after RSM attempt: PASS \n");
    }
    
    printf("\nRSM test completed\n");
    printf("==================\n");
    printf("Total tests: 5\n");
    printf("Passed: %d\n", 5 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
