#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test CMPXCHG8B with memory operands
static void test_cmpxchg8b() {
    printf("Testing CMPXCHG8B instruction:\n");
    printf("=============================\n");
    
    // Aligned memory (16-byte aligned)
    uint64_t aligned_mem[2] __attribute__((aligned(16)));
    uint64_t unaligned_mem[2];
    uint64_t *unaligned_ptr = (uint64_t*)((char*)&unaligned_mem + 4);
    
    uint32_t eax_val, edx_val, ecx_val, ebx_val;
    uint64_t flags;
    
    // Test aligned memory (success)
    aligned_mem[0] = 0x123456789ABCDEF0;
    eax_val = 0x9ABCDEF0;  // low part
    edx_val = 0x12345678;  // high part
    ebx_val = 0x11223344;  // new low
    ecx_val = 0x55667788;  // new high
    CLEAR_FLAGS;
    asm volatile (
        "mov %[eax], %%eax\n\t"
        "mov %[edx], %%edx\n\t"
        "mov %[ebx], %%ebx\n\t"
        "mov %[ecx], %%ecx\n\t"
        "cmpxchg8b %[mem]\n\t"
        "pushfq\n\t"
        "pop %q[flags]\n\t"
        : [mem] "+m" (aligned_mem[0]), [flags] "=&r" (flags)
        : [eax] "r" (eax_val), [edx] "r" (edx_val), 
          [ebx] "r" (ebx_val), [ecx] "r" (ecx_val)
        : "eax", "edx", "ebx", "ecx", "cc"
    );
    printf("Aligned memory (success):\n");
    printf("  Memory before: 0x%016lX, after: 0x%016lX\n", 
           0x123456789ABCDEF0UL, aligned_mem[0]);
    printf("  EDX:EAX before: 0x%08X%08X, after: 0x%08X%08X\n", 
           edx_val, eax_val, edx_val, eax_val);
    print_eflags_cond(flags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
    
    // Test aligned memory (failure)
    aligned_mem[0] = 0x1122334455667788;
    eax_val = 0x9ABCDEF0;  // low part
    edx_val = 0x12345678;  // high part
    ebx_val = 0x99AABBCC;  // new low
    ecx_val = 0xDDEEFF00;  // new high
    CLEAR_FLAGS;
    asm volatile (
        "mov %[eax], %%eax\n\t"
        "mov %[edx], %%edx\n\t"
        "mov %[ebx], %%ebx\n\t"
        "mov %[ecx], %%ecx\n\t"
        "cmpxchg8b %[mem]\n\t"
        "pushfq\n\t"
        "pop %q[flags]\n\t"
        : [mem] "+m" (aligned_mem[0]), [flags] "=&r" (flags)
        : [eax] "r" (eax_val), [edx] "r" (edx_val), 
          [ebx] "r" (ebx_val), [ecx] "r" (ecx_val)
        : "eax", "edx", "ebx", "ecx", "cc"
    );
    printf("Aligned memory (failure):\n");
    printf("  Memory before: 0x%016lX, after: 0x%016lX\n", 
           0x1122334455667788UL, aligned_mem[0]);
    printf("  EDX:EAX before: 0x%08X%08X, after: 0x%08X%08X\n", 
           edx_val, eax_val, (uint32_t)(aligned_mem[0] >> 32), (uint32_t)aligned_mem[0]);
    print_eflags_cond(flags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
    
    // Test unaligned memory (success)
    *unaligned_ptr = 0x123456789ABCDEF0;
    eax_val = 0x9ABCDEF0;  // low part
    edx_val = 0x12345678;  // high part
    ebx_val = 0x11223344;  // new low
    ecx_val = 0x55667788;  // new high
    CLEAR_FLAGS;
    asm volatile (
        "mov %[eax], %%eax\n\t"
        "mov %[edx], %%edx\n\t"
        "mov %[ebx], %%ebx\n\t"
        "mov %[ecx], %%ecx\n\t"
        "cmpxchg8b %[mem]\n\t"
        "pushfq\n\t"
        "pop %q[flags]\n\t"
        : [mem] "+m" (*unaligned_ptr), [flags] "=&r" (flags)
        : [eax] "r" (eax_val), [edx] "r" (edx_val), 
          [ebx] "r" (ebx_val), [ecx] "r" (ecx_val)
        : "eax", "edx", "ebx", "ecx", "cc"
    );
    printf("Unaligned memory (success):\n");
    printf("  Memory before: 0x%016lX, after: 0x%016lX\n", 
           0x123456789ABCDEF0UL, *unaligned_ptr);
    printf("  EDX:EAX before: 0x%08X%08X, after: 0x%08X%08X\n", 
           edx_val, eax_val, edx_val, eax_val);
    print_eflags_cond(flags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
    
    // Test unaligned memory (failure)
    *unaligned_ptr = 0x1122334455667788;
    eax_val = 0x9ABCDEF0;  // low part
    edx_val = 0x12345678;  // high part
    ebx_val = 0x99AABBCC;  // new low
    ecx_val = 0xDDEEFF00;  // new high
    CLEAR_FLAGS;
    asm volatile (
        "mov %[eax], %%eax\n\t"
        "mov %[edx], %%edx\n\t"
        "mov %[ebx], %%ebx\n\t"
        "mov %[ecx], %%ecx\n\t"
        "cmpxchg8b %[mem]\n\t"
        "pushfq\n\t"
        "pop %q[flags]\n\t"
        : [mem] "+m" (*unaligned_ptr), [flags] "=&r" (flags)
        : [eax] "r" (eax_val), [edx] "r" (edx_val), 
          [ebx] "r" (ebx_val), [ecx] "r" (ecx_val)
        : "eax", "edx", "ebx", "ecx", "cc"
    );
    printf("Unaligned memory (failure):\n");
    printf("  Memory before: 0x%016lX, after: 0x%016lX\n", 
           0x1122334455667788UL, *unaligned_ptr);
    printf("  EDX:EAX before: 0x%08X%08X, after: 0x%08X%08X\n", 
           edx_val, eax_val, (uint32_t)(*unaligned_ptr >> 32), (uint32_t)*unaligned_ptr);
    print_eflags_cond(flags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
}

// Test LOCK prefix with CMPXCHG8B
static void test_lock_cmpxchg8b() {
    printf("\nTesting LOCK CMPXCHG8B:\n");
    printf("=======================\n");
    
    uint64_t shared_mem = 0;
    uint32_t eax_val, edx_val, ecx_val, ebx_val;
    uint64_t flags;
    
    // Test with LOCK prefix (success)
    eax_val = 0;        // low part
    edx_val = 0;        // high part
    ebx_val = 0x1234;   // new low
    ecx_val = 0x5678;   // new high
    CLEAR_FLAGS;
    asm volatile (
        "mov %[eax], %%eax\n\t"
        "mov %[edx], %%edx\n\t"
        "mov %[ebx], %%ebx\n\t"
        "mov %[ecx], %%ecx\n\t"
        "lock cmpxchg8b %[mem]\n\t"
        "pushfq\n\t"
        "pop %q[flags]\n\t"
        : [mem] "+m" (shared_mem), [flags] "=&r"(flags)
        : [eax] "r" (eax_val), [edx] "r" (edx_val), 
          [ebx] "r" (ebx_val), [ecx] "r" (ecx_val)
        : "eax", "edx", "ebx", "ecx", "cc"
    );
    printf("LOCK CMPXCHG8B (success):\n");
    printf("  Memory before: 0x%016lX, after: 0x%016lX\n", 0UL, shared_mem);
    printf("  EDX:EAX before: 0x%08X%08X, after: 0x%08X%08X\n", 
           edx_val, eax_val, edx_val, eax_val);
    print_eflags_cond(flags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
    
    // Test with LOCK prefix (failure)
    shared_mem = 0x1122334455667788;
    eax_val = 0;        // low part
    edx_val = 0;        // high part
    ebx_val = 0x99AA;   // new low
    ecx_val = 0xBBCC;   // new high
    CLEAR_FLAGS;
    asm volatile (
        "mov %[eax], %%eax\n\t"
        "mov %[edx], %%edx\n\t"
        "mov %[ebx], %%ebx\n\t"
        "mov %[ecx], %%ecx\n\t"
        "lock cmpxchg8b %[mem]\n\t"
        "pushfq\n\t"
        "pop %q[flags]\n\t"
        : [mem] "+m" (shared_mem), [flags] "=&r"(flags)
        : [eax] "r" (eax_val), [edx] "r" (edx_val), 
          [ebx] "r" (ebx_val), [ecx] "r" (ecx_val)
        : "eax", "edx", "ebx", "ecx", "cc"
    );
    printf("LOCK CMPXCHG8B (failure):\n");
    printf("  Memory before: 0x%016lX, after: 0x%016lX\n", 0x1122334455667788UL, shared_mem);
    printf("  EDX:EAX before: 0x%08X%08X, after: 0x%08X%08X\n", 
           edx_val, eax_val, (uint32_t)(shared_mem >> 32), (uint32_t)shared_mem);
    print_eflags_cond(flags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
}

int main() {
    test_cmpxchg8b();
    test_lock_cmpxchg8b();
    printf("\nCMPXCHG8B tests completed.\n");
    return 0;
}
