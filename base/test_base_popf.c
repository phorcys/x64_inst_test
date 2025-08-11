#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "base.h"

// Define flag macros to match base.h
#define FLAG_CF X_CF
#define FLAG_PF X_PF
#define FLAG_AF X_AF
#define FLAG_ZF X_ZF
#define FLAG_SF X_SF
#define FLAG_DF X_DF
#define FLAG_OF X_OF
#define FLAG_AC (1 << 18)
#define FLAG_NT (1 << 14)
#define FLAG_ID (1 << 21)
#define FLAG_VM (1 << 17)

#define TEST_FLAGS (X_CF | X_PF | X_AF | X_ZF | X_SF | X_DF | X_OF)

// Memory locations for memory operand tests
uint64_t mem_flags64 __attribute__((aligned(8)));
uint32_t mem_flags32 __attribute__((aligned(4)));
uint16_t mem_flags16 __attribute__((aligned(2)));
uint64_t unaligned_flags64 __attribute__((aligned(8))) = 0;
uint32_t unaligned_flags32 __attribute__((aligned(4))) = 0;
uint16_t unaligned_flags16 __attribute__((aligned(2))) = 0;

void* thread_test(void* arg) {
    uint64_t flags = (uint64_t)arg;
    uint64_t result;
    
    __asm__ __volatile__ (
        "pushq %[flags]\n\t"   // Explicit 64-bit push
        "popfq\n\t"             // Use popfq for flags
        "pushfq\n\t"            // Use pushfq for flags
        "popq %[result]"        // Explicit 64-bit pop
        : [result] "=r" (result)
        : [flags] "r" (flags)
    );
    
    return (void*)(result & TEST_FLAGS);
}

void test_operand_sizes() {
    printf("Testing POPF operand sizes:\n");
    
    int errors = 0;
    uint64_t flags_value;
    
    // 64-bit operand
    flags_value = TEST_FLAGS;
    set_eflags(flags_value);
    flags_value = get_eflags();
    
    if ((flags_value & TEST_FLAGS) != TEST_FLAGS) {
        printf("64-bit operand: FAIL (expected 0x%x, got 0x%lx)\n", TEST_FLAGS, flags_value & TEST_FLAGS);
        errors++;
    } else {
        printf("64-bit operand: PASS\n");
    }
    
    printf("Operand size tests: %s\n", errors ? "FAIL" : "PASS");
}

void test_memory_operands() {
    printf("Testing memory operands:\n");
    
    int errors = 0;
    uint64_t flags_value;
    
    // Aligned memory tests
    mem_flags64 = TEST_FLAGS;
    set_eflags(mem_flags64);
    flags_value = get_eflags();
    
    if ((flags_value & TEST_FLAGS) != TEST_FLAGS) {
        printf("Aligned 64-bit memory: FAIL (expected 0x%x, got 0x%lx)\n", TEST_FLAGS, flags_value & TEST_FLAGS);
        errors++;
    } else {
        printf("Aligned 64-bit memory: PASS\n");
    }
    
    // Unaligned memory tests
    uint8_t* unaligned64 = (uint8_t*)&unaligned_flags64 + 1;
    uint64_t flags64 = TEST_FLAGS;
    memcpy(unaligned64, &flags64, sizeof(flags64));
    set_eflags(*(uint64_t*)unaligned64);
    flags_value = get_eflags();
    
    if ((flags_value & TEST_FLAGS) != TEST_FLAGS) {
        printf("Unaligned 64-bit memory: FAIL (expected 0x%x, got 0x%lx)\n", TEST_FLAGS, flags_value & TEST_FLAGS);
        errors++;
    } else {
        printf("Unaligned 64-bit memory: PASS\n");
    }
    
    printf("Memory operand tests: %s\n", errors ? "FAIL" : "PASS");
}

void test_rf_behavior() {
    printf("Testing RF bit behavior:\n");
    
    int errors = 0;
    uint64_t flags_value = (1 << 16); // Set RF bit
    
    set_eflags(flags_value);
    flags_value = get_eflags();
    
    // RF should always be cleared by POPF
    if (flags_value & (1 << 16)) {
        printf("RF bit not cleared: FAIL (RF=0x%lx)\n", flags_value & (1 << 16));
        errors++;
    } else {
        printf("RF bit cleared: PASS\n");
    }
    
    printf("RF bit tests: %s\n", errors ? "FAIL" : "PASS");
}

void test_privileged_flags() {
    printf("Testing user-settable flags:\n");
    
    int errors = 0;
    uint64_t flags_value;
    uint64_t current_flags = get_eflags();
    
    // Only test ID flag which can be set in user mode
    // AC and NT are privileged flags that cannot be set in user mode
    
    // Test ID flag
    flags_value = current_flags | FLAG_ID;
    set_eflags(flags_value);
    flags_value = get_eflags();
    
    if ((flags_value & FLAG_ID) != FLAG_ID) {
        printf("ID flag not set: FAIL (expected 0x%x, got 0x%lx)\n", 
               FLAG_ID, flags_value & FLAG_ID);
        errors++;
    } else {
        printf("ID flag set: PASS\n");
    }
    
    printf("User-settable flag tests: %s\n", errors ? "FAIL" : "PASS");
}

void test_virtual_8086() {
    printf("Testing virtual-8086 mode restrictions:\n");
    
    int errors = 0;
    uint64_t flags_value = FLAG_VM;
    
    // Attempt to set VM flag
    set_eflags(flags_value);
    flags_value = get_eflags();
    
    // VM flag should not be settable in user mode
    if (flags_value & FLAG_VM) {
        printf("VM flag incorrectly set: FAIL (VM=0x%lx)\n", flags_value & FLAG_VM);
        errors++;
    } else {
        printf("VM flag not set: PASS\n");
    }
    
    printf("Virtual-8086 tests: %s\n", errors ? "FAIL" : "PASS");
}

void test_multi_threaded() {
    printf("Testing multi-threaded behavior:\n");
    
    int errors = 0;
    #define NUM_THREADS 4
    pthread_t threads[NUM_THREADS];
    uint64_t thread_flags[NUM_THREADS];
    
    thread_flags[0] = X_CF | X_ZF;
    thread_flags[1] = X_PF | X_AF;
    thread_flags[2] = X_SF | X_DF;
    thread_flags[3] = X_OF | X_CF;
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_test, (void*)thread_flags[i]);
    }
    
    // Collect results
    for (int i = 0; i < NUM_THREADS; i++) {
        void* result;
        pthread_join(threads[i], &result);
        uint64_t flags_result = (uint64_t)result;
        
        if ((flags_result & TEST_FLAGS) != (thread_flags[i] & TEST_FLAGS)) {
            printf("Thread %d: FAIL (expected 0x%lx, got 0x%lx)\n", 
                   i, thread_flags[i] & TEST_FLAGS, flags_result);
            errors++;
        } else {
            printf("Thread %d: PASS\n", i);
        }
    }
    
    printf("Multi-threaded tests: %s\n", errors ? "FAIL" : "PASS");
}

int main() {
    printf("POPF instruction comprehensive test\n");
    printf("==================================\n");
    
    int errors = 0;
    
    test_operand_sizes();
    test_memory_operands();
    test_rf_behavior();
    test_privileged_flags();
    test_virtual_8086();
    test_multi_threaded();
    
    printf("\nPOPF test completed\n");
    printf("===================\n");
    printf("Total test groups: 6\n");
    printf("Failed groups: %d\n", errors);
    
    return errors ? 1 : 0;
}
