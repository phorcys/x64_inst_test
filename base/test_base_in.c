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

static void test_in_imm8() {
    printf("Testing IN with immediate port addresses:\n");
    printf("----------------------------------------\n");
    
    // Test specific constant port addresses
    uint8_t ports[] = {0, 1, 0x7F, 0x80, 0xFF};
    for (size_t i = 0; i < sizeof(ports)/sizeof(ports[0]); i++) {
        uint8_t port = ports[i];
        uint8_t result = 0;
        segv_handled = 0;
        fault_addr = NULL;
        
        if (sigsetjmp(jmp_env, 1) == 0) {
            // Use constant values with switch since 'N' constraint requires constant
            switch (port) {
                case 0:
                    asm volatile ("inb $0, %%al" : "=a" (result));
                    break;
                case 1:
                    asm volatile ("inb $1, %%al" : "=a" (result));
                    break;
                case 0x7F:
                    asm volatile ("inb $0x7F, %%al" : "=a" (result));
                    break;
                case 0x80:
                    asm volatile ("inb $0x80, %%al" : "=a" (result));
                    break;
                case 0xFF:
                    asm volatile ("inb $0xFF, %%al" : "=a" (result));
                    break;
            }
        }
        
        if (segv_handled) {
            printf("IN AL, 0x%02X: Segmentation fault (expected) at %p\n", port, fault_addr);
        } else {
            printf("IN AL, 0x%02X: Unexpected success, result=0x%02X\n", port, result);
        }
    }
    printf("\n");
}

static void test_in_dx() {
    printf("Testing IN with DX register:\n");
    printf("----------------------------\n");
    
    uint16_t ports[] = {0, 1, 0x7FFF, 0x8000, 0xFFFF};
    for (size_t i = 0; i < sizeof(ports)/sizeof(ports[0]); i++) {
        uint16_t port = ports[i];
        uint8_t result8 = 0;
        segv_handled = 0;
        fault_addr = NULL;
        
        if (sigsetjmp(jmp_env, 1) == 0) {
            // Set DX to port address
            asm volatile (
                "mov %0, %%dx"
                :
                : "r" (port)
                : "dx"
            );
            
            // 8-bit IN with DX
            asm volatile (
                "inb %%dx, %%al"
                : "=a" (result8)
                :
                : "dx"
            );
            
            // Shouldn't reach here
            printf("IN AL, DX (0x%04X): Unexpected success, result8=0x%02X\n", port, result8);
            continue;
        }
        
        if (segv_handled) {
            printf("IN AL, DX (0x%04X): Segmentation fault (expected) at %p\n", port, fault_addr);
        }
    }
    printf("\n");
}

int main() {
    setup_sigsegv_handler();
    
    printf("Starting IN instruction tests\n");
    printf("============================\n\n");
    
    test_in_imm8();
    test_in_dx();
    
    reset_sigsegv_handler();
    printf("All IN tests completed.\n");
    return 0;
}
