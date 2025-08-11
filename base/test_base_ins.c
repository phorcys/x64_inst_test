#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

static jmp_buf jmp_env;
static volatile sig_atomic_t segv_handled = 0;
static void *fault_addr = NULL;

static void segv_handler(int sig, siginfo_t *info, void *ucontext_unused) {
    (void)sig;
    (void)ucontext_unused;
    fault_addr = info->si_addr;
    segv_handled = 1;
    siglongjmp(jmp_env, 1);
}

static void setup_sigsegv_handler() {
    struct sigaction sa;
    sa.sa_sigaction = segv_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
}

static void reset_sigsegv_handler() {
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);
}

static void test_insb() {
    printf("Testing INSB (byte input from port to string):\n");
    printf("---------------------------------------------\n");
    
    uint16_t ports[] = {0, 1, 0x7F, 0x80, 0xFFFF};
    uint8_t buffer[8] = {0};
    
    for (size_t i = 0; i < sizeof(ports)/sizeof(ports[0]); i++) {
        uint16_t port = ports[i];
        segv_handled = 0;
        fault_addr = NULL;
        
        if (sigsetjmp(jmp_env, 1) == 0) {
            // Set up ES:DI (using ES segment)
            asm volatile (
                "mov %0, %%dx\n\t"        // Set port
                "mov %1, %%rdi\n\t"       // Set buffer address
                "mov %%ds, %%ax\n\t"      // Copy DS to AX
                "mov %%ax, %%es\n\t"      // Set ES to same as DS
                "cld\n\t"                 // Clear direction flag (forward)
                "insb"                    // Execute INSB
                :
                : "r" (port), "r" (buffer)
                : "dx", "rdi", "ax", "memory", "cc"
            );
            
            // Shouldn't reach here
            printf("INSB (port 0x%04X): Unexpected success\n", port);
            continue;
        }
        
        if (segv_handled) {
            printf("INSB (port 0x%04X): Segmentation fault (expected) at %p\n", port, fault_addr);
        }
    }
    printf("\n");
}

static void test_insw() {
    printf("Testing INSW (word input from port to string):\n");
    printf("---------------------------------------------\n");
    
    uint16_t ports[] = {0, 1, 0x7F, 0x80, 0xFFFF};
    uint16_t buffer[8] = {0};
    
    for (size_t i = 0; i < sizeof(ports)/sizeof(ports[0]); i++) {
        uint16_t port = ports[i];
        segv_handled = 0;
        fault_addr = NULL;
        
        if (sigsetjmp(jmp_env, 1) == 0) {
            // Set up ES:DI (using ES segment)
            asm volatile (
                "mov %0, %%dx\n\t"        // Set port
                "mov %1, %%rdi\n\t"       // Set buffer address
                "mov %%ds, %%ax\n\t"      // Copy DS to AX
                "mov %%ax, %%es\n\t"      // Set ES to same as DS
                "cld\n\t"                 // Clear direction flag (forward)
                "insw"                    // Execute INSW
                :
                : "r" (port), "r" (buffer)
                : "dx", "rdi", "ax", "memory", "cc"
            );
            
            // Shouldn't reach here
            printf("INSW (port 0x%04X): Unexpected success\n", port);
            continue;
        }
        
        if (segv_handled) {
            printf("INSW (port 0x%04X): Segmentation fault (expected) at %p\n", port, fault_addr);
        }
    }
    printf("\n");
}

static void test_insd() {
    printf("Testing INSD (dword input from port to string):\n");
    printf("----------------------------------------------\n");
    
    uint16_t ports[] = {0, 1, 0x7F, 0x80, 0xFFFF};
    uint32_t buffer[8] = {0};
    
    for (size_t i = 0; i < sizeof(ports)/sizeof(ports[0]); i++) {
        uint16_t port = ports[i];
        segv_handled = 0;
        fault_addr = NULL;
        
        if (sigsetjmp(jmp_env, 1) == 0) {
            // Set up ES:DI (using ES segment)
            asm volatile (
                "mov %0, %%dx\n\t"        // Set port
                "mov %1, %%rdi\n\t"       // Set buffer address
                "mov %%ds, %%ax\n\t"      // Copy DS to AX
                "mov %%ax, %%es\n\t"      // Set ES to same as DS
                "cld\n\t"                 // Clear direction flag (forward)
                "insl"                    // Execute INSD (insl is the mnemonic for dword)
                :
                : "r" (port), "r" (buffer)
                : "dx", "rdi", "ax", "memory", "cc"
            );
            
            // Shouldn't reach here
            printf("INSD (port 0x%04X): Unexpected success\n", port);
            continue;
        }
        
        if (segv_handled) {
            printf("INSD (port 0x%04X): Segmentation fault (expected) at %p\n", port, fault_addr);
        }
    }
    printf("\n");
}

static void test_ins_direction() {
    printf("Testing INS with direction flag set:\n");
    printf("-----------------------------------\n");
    
    uint16_t port = 0x80;
    uint8_t buffer[8] = {0};
    
    // Test backward direction (DF=1)
    segv_handled = 0;
    fault_addr = NULL;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        // Set up ES:DI (using ES segment)
        asm volatile (
            "mov %0, %%dx\n\t"        // Set port
            "mov %1, %%rdi\n\t"       // Set buffer address
            "add $7, %%rdi\n\t"       // Point to end of buffer
            "mov %%ds, %%ax\n\t"      // Copy DS to AX
            "mov %%ax, %%es\n\t"      // Set ES to same as DS
            "std\n\t"                 // Set direction flag (backward)
            "insb"                    // Execute INSB
            :
            : "r" (port), "r" (buffer)
            : "dx", "rdi", "ax", "memory", "cc"
        );
        
        // Shouldn't reach here
        printf("INSB with DF=1 (port 0x%04X): Unexpected success\n", port);
    } else {
        if (segv_handled) {
            printf("INSB with DF=1 (port 0x%04X): Segmentation fault (expected) at %p\n", port, fault_addr);
        }
    }
    
    // Test forward direction (DF=0)
    segv_handled = 0;
    fault_addr = NULL;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        // Set up ES:DI (using ES segment)
        asm volatile (
            "mov %0, %%dx\n\t"        // Set port
            "mov %1, %%rdi\n\t"       // Set buffer address
            "mov %%ds, %%ax\n\t"      // Copy DS to AX
            "mov %%ax, %%es\n\t"      // Set ES to same as DS
            "cld\n\t"                 // Clear direction flag (forward)
            "insb"                    // Execute INSB
            :
            : "r" (port), "r" (buffer)
            : "dx", "rdi", "ax", "memory", "cc"
        );
        
        // Shouldn't reach here
        printf("INSB with DF=0 (port 0x%04X): Unexpected success\n", port);
    } else {
        if (segv_handled) {
            printf("INSB with DF=0 (port 0x%04X): Segmentation fault (expected) at %p\n", port, fault_addr);
        }
    }
    printf("\n");
}

int main() {
    setup_sigsegv_handler();
    
    printf("Starting INS instruction tests\n");
    printf("=============================\n\n");
    
    test_insb();
    test_insw();
    test_insd();
    test_ins_direction();
    
    reset_sigsegv_handler();
    printf("All INS tests completed.\n");
    return 0;
}
