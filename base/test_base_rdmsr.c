#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

static jmp_buf env;
static volatile int segv_occurred = 0;

void handle_sigsegv(int sig) {
    segv_occurred = 1;
    siglongjmp(env, 1);
}

int main() {
    printf("RDMSR instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    uint32_t msr_index;
    uint32_t low, high;
    
    // Setup signal handler for SIGSEGV
    struct sigaction sa;
    sa.sa_handler = handle_sigsegv;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);
    
    // Test reading IA32_TIME_STAMP_COUNTER (MSR 0x10) - may be accessible in some contexts
    msr_index = 0x10;
    segv_occurred = 0;
    
    if (sigsetjmp(env, 1) == 0) {
        __asm__ __volatile__ (
            "mov %[msr], %%ecx\n\t"
            "rdmsr"
            : "=a" (low), "=d" (high)
            : [msr] "r" (msr_index)
            : "ecx"
        );
        
        if (segv_occurred) {
            printf("RDMSR for MSR 0x%x caused SIGSEGV (expected in user mode)\n", msr_index);
        } else {
            printf("RDMSR for MSR 0x%x returned low: 0x%x, high: 0x%x\n", msr_index, low, high);
        }
    } else {
        printf("RDMSR for MSR 0x%x caused SIGSEGV (expected in user mode)\n", msr_index);
    }
    
    // Test reading IA32_APIC_BASE (MSR 0x1B) - typically requires privilege
    msr_index = 0x1B;
    segv_occurred = 0;
    
    if (sigsetjmp(env, 1) == 0) {
        __asm__ __volatile__ (
            "mov %[msr], %%ecx\n\t"
            "rdmsr"
            : "=a" (low), "=d" (high)
            : [msr] "r" (msr_index)
            : "ecx"
        );
        
        if (segv_occurred) {
            printf("RDMSR for MSR 0x%x caused SIGSEGV (expected in user mode)\n", msr_index);
        } else {
            printf("RDMSR for MSR 0x%x returned low: 0x%x, high: 0x%x\n", msr_index, low, high);
            // Check if APIC base is valid
            if (low & 0x800) { // Check enable bit
                printf("APIC enabled\n");
            }
        }
    } else {
        printf("RDMSR for MSR 0x%x caused SIGSEGV (expected in user mode)\n", msr_index);
    }
    
    // Test invalid MSR index (0xFFFFFFFF)
    msr_index = 0xFFFFFFFF;
    segv_occurred = 0;
    
    if (sigsetjmp(env, 1) == 0) {
        __asm__ __volatile__ (
            "mov %[msr], %%ecx\n\t"
            "rdmsr"
            : "=a" (low), "=d" (high)
            : [msr] "r" (msr_index)
            : "ecx"
        );
        
        if (segv_occurred) {
            printf("RDMSR for invalid MSR 0x%x caused SIGSEGV (expected)\n", msr_index);
        } else {
            printf("RDMSR for invalid MSR 0x%x returned low: 0x%x, high: 0x%x\n", msr_index, low, high);
            errors++;
        }
    } else {
        printf("RDMSR for invalid MSR 0x%x caused SIGSEGV (expected)\n", msr_index);
    }
    
    // Test with ECX=0 (IA32_P5_MC_ADDR)
    msr_index = 0x0;
    segv_occurred = 0;
    
    if (sigsetjmp(env, 1) == 0) {
        __asm__ __volatile__ (
            "mov %[msr], %%ecx\n\t"
            "rdmsr"
            : "=a" (low), "=d" (high)
            : [msr] "r" (msr_index)
            : "ecx"
        );
        
        if (segv_occurred) {
            printf("RDMSR for MSR 0x%x caused SIGSEGV (expected in user mode)\n", msr_index);
        } else {
            printf("RDMSR for MSR 0x%x returned low: 0x%x, high: 0x%x\n", msr_index, low, high);
        }
    } else {
        printf("RDMSR for MSR 0x%x caused SIGSEGV (expected in user mode)\n", msr_index);
    }
    
    printf("\nRDMSR test completed\n");
    printf("====================\n");
    printf("Note: RDMSR is a privileged instruction and typically causes SIGSEGV in user mode\n");
    printf("Total tests: 4\n");
    printf("Errors: %d\n", errors);
    
    return errors ? 1 : 0;
}
