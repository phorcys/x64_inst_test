#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <signal.h>
#include "base.h"

static jmp_buf jmp_env;
static volatile sig_atomic_t segv_handled = 0;
static volatile sig_atomic_t ill_handled = 0;

static void signal_handler(int sig, siginfo_t *info, void *ucontext_unused) {
    (void)ucontext_unused;
    (void)info;
    if (sig == SIGSEGV) segv_handled = 1;
    if (sig == SIGILL) ill_handled = 1;
    siglongjmp(jmp_env, 1);
}

static void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
}

static void reset_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
}

// Helper function to print flags in a readable format
static void print_flags(const char* name, uint64_t flags) {
    printf("%s: CF=%d PF=%d AF=%d ZF=%d SF=%d TF=%d IF=%d DF=%d OF=%d\n",
           name,
           (flags & 1) ? 1 : 0,      // CF
           (flags & 4) ? 1 : 0,      // PF
           (flags & 16) ? 1 : 0,     // AF
           (flags & 64) ? 1 : 0,     // ZF
           (flags & 128) ? 1 : 0,    // SF
           (flags & 256) ? 1 : 0,    // TF
           (flags & 512) ? 1 : 0,    // IF
           (flags & 1024) ? 1 : 0,   // DF
           (flags & 2048) ? 1 : 0);  // OF
}

static void test_iret_basic() {
    printf("Testing basic IRET functionality:\n");
    printf("--------------------------------\n");
    
    uint64_t original_flags, after_flags;
    volatile int iret_success = 0;
    segv_handled = 0;
    ill_handled = 0;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        asm volatile (
            // Save current flags
            "pushfq\n"
            "popq %0\n"
            : "=r" (original_flags)
        );
        
        uint64_t modified_flags = original_flags ^ 0xCD5; // Toggle flags: CF, PF, AF, ZF, SF, DF, OF
        
        asm volatile (
            // Build IRET frame
            "leaq 1f(%%rip), %%rax\n"    // Load address of label 1
            "pushq %%rax\n"              // RIP
            "movq %%cs, %%rax\n"
            "pushq %%rax\n"              // CS
            "pushq %2\n"                 // RFLAGS (modified)
            
            "iretq\n"                    // Execute IRET
            
            // This code should not be executed
            "ud2\n"                      // Generate illegal instruction if IRET fails
            
            // Label we return to via IRET
            "1:\n"
            // Save flags after IRET
            "pushfq\n"
            "popq %0\n"                  // Save to after_flags
            "movl $1, %1\n"              // Set success flag
            : "=r" (after_flags), "+m" (iret_success)
            : "r" (modified_flags)
            : "rax", "memory", "cc"
        );
    }
    
    if (segv_handled || ill_handled || !iret_success) {
        printf("IRET execution failed\n");
        if (segv_handled) printf("  - Segmentation fault occurred\n");
        if (ill_handled) printf("  - Illegal instruction occurred\n");
        if (!iret_success) printf("  - IRET did not reach target label\n");
    } else {
        print_flags("Original flags", original_flags);
        print_flags("Modified flags", original_flags ^ 0xCD5);
        print_flags("Flags after IRET", after_flags);
        
        // Verify flag restoration
        uint64_t flag_mask = 0xCD5; // Mask for CF, PF, AF, ZF, SF, DF, OF
        uint64_t expected_flags = (original_flags & ~flag_mask) | (original_flags ^ flag_mask);
        
        if ((after_flags & flag_mask) == (expected_flags & flag_mask)) {
            printf("Flag restoration successful\n");
        } else {
            printf("Flag restoration failed\n");
            printf("Expected: 0x%04lx\n", expected_flags & flag_mask);
            printf("Actual:   0x%04lx\n", after_flags & flag_mask);
        }
    }
    printf("\n");
}

static void test_iret_with_stack_switch() {
    printf("Testing IRET with stack switch:\n");
    printf("------------------------------\n");
    
    volatile int iret_success = 0;
    segv_handled = 0;
    ill_handled = 0;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        // Allocate a new stack
        uint8_t new_stack[4096] __attribute__((aligned(16)));
        uintptr_t new_stack_top = (uintptr_t)&new_stack[sizeof(new_stack) - 16];
        
        asm volatile (
            // Save current RSP
            "movq %%rsp, %%r15\n"
            
            // Build IRET frame on new stack
            "movq %1, %%rsp\n"           // Switch to new stack
            
            "leaq 1f(%%rip), %%rax\n"    // Load address of label 1
            "pushq %%rax\n"              // RIP
            "movq %%cs, %%rax\n"
            "pushq %%rax\n"              // CS
            "pushfq\n"                   // Current RFLAGS
            "popq %%rax\n"
            "pushq %%rax\n"              // RFLAGS
            
            "iretq\n"                    // Execute IRET
            
            // This code should not be executed
            "ud2\n"                      // Generate illegal instruction if IRET fails
            
            // Label we return to via IRET
            "1:\n"
            // Restore original stack
            "movq %%r15, %%rsp\n"
            "movl $1, %0\n"              // Set success flag
            : "+m" (iret_success)
            : "r" (new_stack_top)
            : "rax", "r15", "memory", "cc"
        );
    }
    
    if (segv_handled || ill_handled || !iret_success) {
        printf("Stack switch test failed\n");
        if (segv_handled) printf("  - Segmentation fault occurred\n");
        if (ill_handled) printf("  - Illegal instruction occurred\n");
        if (!iret_success) printf("  - IRET did not reach target label\n");
    } else {
        printf("Stack switch successful\n");
    }
    printf("\n");
}

int main() {
    setup_signal_handlers();
    
    printf("Starting IRET instruction tests\n");
    printf("==============================\n\n");
    
    test_iret_basic();
    test_iret_with_stack_switch();
    
    reset_signal_handlers();
    printf("All IRET tests completed.\n");
    return 0;
}
