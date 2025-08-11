#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

static jmp_buf jmp_env;
static volatile sig_atomic_t segv_handled = 0;
static void *fault_addr = NULL;

static void signal_handler(int sig, siginfo_t *info, void *ucontext_unused) {
    (void)ucontext_unused;
    fault_addr = info->si_addr;
    segv_handled = 1;
    siglongjmp(jmp_env, 1);
}

static void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGTRAP, &sa, NULL);
}

static void reset_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGTRAP, &sa, NULL);
}

static void test_int_common(uint8_t vector) {
    segv_handled = 0;
    fault_addr = NULL;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        // Execute INT instruction with the specified vector
        switch(vector) {
            case 0x00: asm volatile ("int $0x00"); break;
            case 0x01: asm volatile ("int $0x01"); break;
            case 0x02: asm volatile ("int $0x02"); break;
            case 0x03: asm volatile ("int $0x03"); break;
            case 0x04: asm volatile ("int $0x04"); break;
            case 0x05: asm volatile ("int $0x05"); break;
            case 0x06: asm volatile ("int $0x06"); break;
            case 0x07: asm volatile ("int $0x07"); break;
            case 0x08: asm volatile ("int $0x08"); break;
            case 0x09: asm volatile ("int $0x09"); break;
            case 0x0A: asm volatile ("int $0x0A"); break;
            case 0x0B: asm volatile ("int $0x0B"); break;
            case 0x0C: asm volatile ("int $0x0C"); break;
            case 0x0D: asm volatile ("int $0x0D"); break;
            case 0x0E: asm volatile ("int $0x0E"); break;
            case 0x0F: asm volatile ("int $0x0F"); break;
            case 0x10: asm volatile ("int $0x10"); break;
            case 0x11: asm volatile ("int $0x11"); break;
            case 0x12: asm volatile ("int $0x12"); break;
            case 0x13: asm volatile ("int $0x13"); break;
            case 0x14: asm volatile ("int $0x14"); break;
            case 0x15: asm volatile ("int $0x15"); break;
            case 0x16: asm volatile ("int $0x16"); break;
            case 0x17: asm volatile ("int $0x17"); break;
            case 0x18: asm volatile ("int $0x18"); break;
            case 0x19: asm volatile ("int $0x19"); break;
            case 0x1A: asm volatile ("int $0x1A"); break;
            case 0x1B: asm volatile ("int $0x1B"); break;
            case 0x1C: asm volatile ("int $0x1C"); break;
            case 0x1D: asm volatile ("int $0x1D"); break;
            case 0x1E: asm volatile ("int $0x1E"); break;
            case 0x1F: asm volatile ("int $0x1F"); break;
            case 0x80: 
                // Special case for syscall interface
                asm volatile ("int $0x80");
                printf("INT 0x80: System call interface (expected behavior)\n");
                return;
            case 0x81: asm volatile ("int $0x81"); break;
            case 0xFF: asm volatile ("int $0xFF"); break;
            default: 
                printf("Unhandled vector 0x%02X\n", vector);
                return;
        }
        
        // Shouldn't reach here
        printf("INT 0x%02X: Unexpected success\n", vector);
    } else {
        if (segv_handled) {
            printf("INT 0x%02X: Segmentation fault (expected)\n", vector);
        }
    }
}

static void test_int_basic() {
    printf("Testing INT (software interrupt):\n");
    printf("--------------------------------\n");
    
    // Test a range of interrupt vectors
    for (int i = 0; i < 32; i++) {
        test_int_common(i);
    }
    
    // Test some common vectors
    test_int_common(0x80);
    test_int_common(0x81);
    test_int_common(0xFF);
    
    printf("\n");
}

static void test_int3() {
    printf("Testing INT3 (breakpoint):\n");
    printf("--------------------------\n");
    
    segv_handled = 0;
    fault_addr = NULL;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        asm volatile ("int3");
        printf("INT3: Unexpected success\n");
    } else {
        if (segv_handled) {
            printf("INT3: Segmentation fault (expected)\n");
        }
    }
    printf("\n");
}

static void test_int1() {
    printf("Testing INT1 (debug trap):\n");
    printf("--------------------------\n");
    
    segv_handled = 0;
    fault_addr = NULL;
    
    if (sigsetjmp(jmp_env, 1) == 0) {
        // Use the opcode directly since "int1" mnemonic may not be recognized
        asm volatile (".byte 0xF1");
        printf("INT1: Unexpected success\n");
    } else {
        if (segv_handled) {
            printf("INT1: Segmentation fault (expected)\n");
        }
    }
    printf("\n");
}

int main() {
    setup_signal_handlers();
    
    printf("Starting INT instruction tests\n");
    printf("=============================\n\n");
    
    test_int_basic();
    test_int3();
    test_int1();
    
    reset_signal_handlers();
    printf("All INT tests completed.\n");
    return 0;
}
