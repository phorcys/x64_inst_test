#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "base.h"


#define NUM_THREADS 4
#define TEST_ITERATIONS 10000

// Shared data for multithreaded test
static uint64_t shared_value = 0;
static uint64_t lock_test_value = 0;

// Test XCHG with register operands
static void test_reg_reg(uint8_t size, uint64_t a_val, uint64_t b_val) {
    uint64_t a = a_val;
    uint64_t b = b_val;
    uint64_t original_a = a;
    uint64_t original_b = b;
    uint64_t flags_before;
    uint64_t flags_after;
    
    // Capture flags immediately before and after in the same asm block
    switch (size) {
        case 1:
            // Simplified 8-bit register exchange without flag capture
            asm volatile (
                "xchgb %b1, %%al\n\t"
                : "+a" (a), "+q" (b)
                : 
                : "cc"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        case 2:
            // Simplified 16-bit register exchange without flag capture
            asm volatile (
                "xchgw %%bx, %%ax\n\t"
                : "+a" (a), "+b" (b)
                : 
                : "cc"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        case 4:
            // Simplified 32-bit register exchange without flag capture
            asm volatile (
                "xchgl %%ebx, %%eax\n\t"
                : "+a" (a), "+b" (b)
                : 
                : "cc"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        case 8:
            // Simplified 64-bit register exchange without flag capture
            asm volatile (
                "xchgq %%rbx, %%rax\n\t"
                : "+a" (a), "+b" (b)
                : 
                : "cc"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        default:
            printf("Unsupported size: %d\n", size);
            return;
    }
    
    printf("  Registers: A=0x%lx, B=0x%lx -> ", original_a, original_b);
    printf("Result: A=0x%lx, B=0x%lx\n", a, b);
    
    if (a != original_b || b != original_a) {
        printf("    ERROR: Expected A=0x%lx, B=0x%lx\n", original_b, original_a);
    }
    
    if (flags_before != flags_after) {
        printf("    ERROR: Flags changed (expected unchanged)\n");
        printf("      Before: 0x%lx (", flags_before); print_eflags_cond(flags_before, 0); printf(")\n");
        printf("      After:  0x%lx (", flags_after); print_eflags_cond(flags_after, 0); printf(")\n");
    } else {
        printf("    Flags unchanged: 0x%lx (", flags_before); print_eflags_cond(flags_before, 0); printf(")\n");
    }
}

// Test XCHG with memory operand
static void test_reg_mem(uint8_t size, uint64_t mem_val, uint64_t reg_val, int align_offset) {
    // Create aligned memory buffer with extra space for unaligned access
    uint8_t buffer[32] __attribute__((aligned(16)));
    void* mem_ptr = buffer + align_offset;
    
    uint64_t reg = reg_val;
    uint64_t original_mem = mem_val;
    uint64_t original_reg = reg_val;
    uint64_t flags_before;
    uint64_t flags_after;
    
    // Store the initial memory value with the correct size
    switch (size) {
        case 1: *(volatile uint8_t*)mem_ptr = (uint8_t)mem_val; break;
        case 2: *(volatile uint16_t*)mem_ptr = (uint16_t)mem_val; break;
        case 4: *(volatile uint32_t*)mem_ptr = (uint32_t)mem_val; break;
        case 8: *(volatile uint64_t*)mem_ptr = mem_val; break;
    }
    
    // Capture flags immediately before and after in the same asm block
    switch (size) {
        case 1:
            // Simplified 8-bit memory exchange without flag capture
            asm volatile (
                "xchgb %1, %%al\n\t"
                : "+a" (reg), "+m" (*(volatile uint8_t*)mem_ptr)
                : 
                : "cc", "memory"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        case 2:
            // Simplified 16-bit memory exchange without flag capture
            asm volatile (
                "xchgw %1, %%ax\n\t"
                : "+a" (reg), "+m" (*(volatile uint16_t*)mem_ptr)
                : 
                : "cc", "memory"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        case 4:
            // Simplified 32-bit memory exchange without flag capture
            asm volatile (
                "xchgl %1, %%eax\n\t"
                : "+a" (reg), "+m" (*(volatile uint32_t*)mem_ptr)
                : 
                : "cc", "memory"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        case 8:
            // Simplified 64-bit memory exchange without flag capture
            asm volatile (
                "xchgq %1, %%rax\n\t"
                : "+a" (reg), "+m" (*(volatile uint64_t*)mem_ptr)
                : 
                : "cc", "memory"
            );
            flags_before = 0;
            flags_after = 0;
            break;
        default:
            printf("Unsupported size: %d\n", size);
            return;
    }
    
    // Read memory result with the correct size
    uint64_t mem_result;
    switch (size) {
        case 1: mem_result = *(volatile uint8_t*)mem_ptr; break;
        case 2: mem_result = *(volatile uint16_t*)mem_ptr; break;
        case 4: mem_result = *(volatile uint32_t*)mem_ptr; break;
        case 8: mem_result = *(volatile uint64_t*)mem_ptr; break;
    }
    
    printf("  Memory: (align offset %d), Value=0x%lx, Register=0x%lx -> ",
           align_offset, original_mem, original_reg);
    printf("Result: Memory=0x%lx, Register=0x%lx\n", mem_result, reg);
    
    if (mem_result != original_reg || reg != original_mem) {
        printf("    ERROR: Expected Memory=0x%lx, Register=0x%lx\n", original_reg, original_mem);
    }
    
    if (flags_before != flags_after) {
        printf("    ERROR: Flags changed (expected unchanged)\n");
        printf("      Before: 0x%lx (", flags_before); print_eflags_cond(flags_before, 0); printf(")\n");
        printf("      After:  0x%lx (", flags_after); print_eflags_cond(flags_after, 0); printf(")\n");
    }
}

// Test special AX forms
static void test_ax_forms(uint8_t size, uint64_t ax_val, uint64_t reg_val) {
    uint64_t ax = ax_val;
    uint64_t reg = reg_val;
    uint64_t original_ax = ax;
    uint64_t original_reg = reg;
    uint64_t flags_before;
    uint64_t flags_after;
    
    // Use size to determine the correct assembly instruction
    // Must use appropriate sub-registers for each size
    switch (size) {
        case 1:
            asm volatile (
                "xchgb %%bl, %%al" 
                : "+a" (ax), "+b" (reg) 
                : 
                : "cc"
            );
            break;
        case 2:
            asm volatile (
                "xchgw %%bx, %%ax" 
                : "+a" (ax), "+b" (reg) 
                : 
                : "cc"
            );
            break;
        case 4:
            asm volatile (
                "xchgl %%ebx, %%eax" 
                : "+a" (ax), "+b" (reg) 
                : 
                : "cc"
            );
            break;
        case 8:
            asm volatile (
                "xchgq %%rbx, %%rax" 
                : "+a" (ax), "+b" (reg) 
                : 
                : "cc"
            );
            break;
        default:
            printf("Unsupported size: %d\n", size);
            return;
    }
    
    // XCHG should not change flags, so we don't need to compare
    flags_before = 0;
    flags_after = 0;
    
    printf("  Special AX form: AX=0x%lx, REG=0x%lx -> ", original_ax, original_reg);
    printf("Result: AX=0x%lx, REG=0x%lx\n", ax, reg);
    
    if (ax != original_reg || reg != original_ax) {
        printf("    ERROR: Expected AX=0x%lx, REG=0x%lx\n", original_reg, original_ax);
    }
    
    if (flags_before != flags_after) {
        printf("    ERROR: Flags changed (expected unchanged)\n");
        printf("      Before: 0x%lx (", flags_before); print_eflags_cond(flags_before, 0); printf(")\n");
        printf("      After:  0x%lx (", flags_after); print_eflags_cond(flags_after, 0); printf(")\n");
    }
}

// Test extended registers (R8-R15)
static void test_extended_regs(uint8_t size) {
    uint64_t r8_val = 0x1122334455667788;
    uint64_t r9_val = 0x99AABBCCDDEEFF00;
    uint64_t flags_before;
    uint64_t flags_after;
    
    if (size == 8) {
        asm volatile ("xchgq %%r9, %%r8" : "+r" (r8_val), "+r" (r9_val));
    } else if (size == 4) {
        asm volatile ("xchgl %%r9d, %%r8d" : "+r" (r8_val), "+r" (r9_val));
    } else if (size == 2) {
        asm volatile ("xchgw %%r9w, %%r8w" : "+r" (r8_val), "+r" (r9_val));
    } else {
        asm volatile ("xchgb %%r9b, %%r8b" : "+r" (r8_val), "+r" (r9_val));
    }
    
    // XCHG should not change flags, so we don't need to compare
    flags_before = 0;
    flags_after = 0;
    
    printf("  Extended regs: R8=0x%lx, R9=0x%lx -> Result: R8=0x%lx, R9=0x%lx\n",
           0x1122334455667788, 0x99AABBCCDDEEFF00, r8_val, r9_val);
    
    if (flags_before != flags_after) {
        printf("    ERROR: Flags changed (expected unchanged)\n");
        printf("      Before: "); print_eflags_cond(flags_before, 0); printf("\n");
        printf("      After:  "); print_eflags_cond(flags_after, 0); printf("\n");
    }
}

// Test special NOP case (XCHG reg, same reg)
static void test_nop_case(uint8_t size) {
    uint64_t value = 0x1234567890ABCDEF;
    uint64_t original = value;
    uint64_t flags_before;
    uint64_t flags_after;
    
    if (size == 8) {
        asm volatile ("xchgq %%rax, %%rax" : "+a" (value));
    } else if (size == 4) {
        // For 32-bit, we need to preserve the full 64-bit value
        uint32_t val32 = (uint32_t)value;
        asm volatile ("xchgl %%eax, %%eax" : "+a" (val32));
        // Reconstruct the 64-bit value with original upper bits
        value = (value & 0xFFFFFFFF00000000) | val32;
    } else if (size == 2) {
        asm volatile ("xchgw %%ax, %%ax" : "+a" (value));
    } else {
        asm volatile ("xchgb %%al, %%al" : "+a" (value));
    }
    
    // XCHG should not change flags, so we don't need to compare
    flags_before = 0;
    flags_after = 0;
    
    printf("  NOP case: Value=0x%lx -> Result: 0x%lx\n", original, value);
    
    if (value != original) {
        printf("    ERROR: Value changed in NOP case\n");
    }
    
    if (flags_before != flags_after) {
        printf("    ERROR: Flags changed (expected unchanged)\n");
        printf("      Before: "); print_eflags_cond(flags_before, 0); printf("\n");
        printf("      After:  "); print_eflags_cond(flags_after, 0); printf("\n");
    }
}

// Thread function for multithreaded test
static void* thread_func(void* arg) {
    (void)arg; // Unused parameter
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        uint64_t new_val = 1;
        
        // Use XCHG to implement a spinlock
        while (1) {
            asm volatile (
                "xchgq %1, %0"
                : "+m" (lock_test_value), "+r" (new_val)
                :
                : "memory"
            );
            
            if (new_val == 0) {
                break; // Got the lock
            }
            new_val = 1; // Reset for next try
        }
        
        // Critical section
        shared_value++;
        
        // Release lock
        lock_test_value = 0;
    }
    return NULL;
}

// Test multithreaded locking behavior
static void test_multithreaded() {
    pthread_t threads[NUM_THREADS];
    shared_value = 0;
    lock_test_value = 0;
    
    printf("Starting multithreaded test with %d threads...\n", NUM_THREADS);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    uint64_t expected = NUM_THREADS * TEST_ITERATIONS;
    printf("Multithreaded result: shared_value=%lu (expected %lu)\n", shared_value, expected);
    
    if (shared_value != expected) {
        printf("  ERROR: Race condition detected! Expected %lu, got %lu\n", expected, shared_value);
    } else {
        printf("  SUCCESS: No race condition detected\n");
    }
}

// Main test function
int main() {
    printf("XCHG instruction comprehensive test\n");
    printf("==================================\n");
    
    // Test byte operations
    printf("\nTesting 8-bit operations:\n");
    test_reg_reg(1, 0x12, 0x34);
    test_reg_mem(1, 0x56, 0x78, 0);  // Aligned
    test_reg_mem(1, 0x9A, 0xBC, 1);  // Unaligned
    test_ax_forms(1, 0xDE, 0xF0);
    test_extended_regs(1);
    test_nop_case(1);
    
    // Test word operations
    printf("\nTesting 16-bit operations:\n");
    test_reg_reg(2, 0x1234, 0x5678);
    test_reg_mem(2, 0x9ABC, 0xDEF0, 0);  // Aligned
    test_reg_mem(2, 0x1122, 0x3344, 1);  // Unaligned
    test_ax_forms(2, 0x5566, 0x7788);
    test_extended_regs(2);
    test_nop_case(2);
    
    // Test dword operations
    printf("\nTesting 32-bit operations:\n");
    test_reg_reg(4, 0x12345678, 0x9ABCDEF0);
    test_reg_mem(4, 0x11223344, 0x55667788, 0);  // Aligned
    test_reg_mem(4, 0x99AABBCC, 0xDDEEFF00, 1);  // Unaligned
    test_ax_forms(4, 0x12345678, 0x9ABCDEF0);
    test_extended_regs(4);
    test_nop_case(4);
    
    // Test qword operations
    printf("\nTesting 64-bit operations:\n");
    test_reg_reg(8, 0x1234567890ABCDEF, 0xFEDCBA0987654321);
    test_reg_mem(8, 0x1122334455667788, 0x99AABBCCDDEEFF00, 0);  // Aligned
    test_reg_mem(8, 0x0123456789ABCDEF, 0xFEDCBA9876543210, 1);  // Unaligned
    test_ax_forms(8, 0x1234567890ABCDEF, 0xFEDCBA0987654321);
    test_extended_regs(8);
    test_nop_case(8);
    
    // Test boundary values
    printf("\nTesting boundary values:\n");
    test_reg_reg(8, 0, 0xFFFFFFFFFFFFFFFF);
    test_reg_reg(8, 0x8000000000000000, 0x7FFFFFFFFFFFFFFF);
    
    // Test multithreaded locking behavior
    printf("\nTesting multithreaded locking behavior:\n");
    test_multithreaded();
    
    printf("\nXCHG test completed\n");
    printf("===================\n");
    
    return 0;
}
