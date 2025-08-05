#include "base.h"
#include <stdint.h>
#include <stdio.h>

void test_bsf_reg_reg() {
    printf("\nTesting BSF reg, reg\n");
    
    // 16-bit tests
    uint16_t src16, dest16;
    uint32_t eflags;
    
    uint16_t test_cases16[] = {
        0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 
        0x0010, 0x0020, 0x0040, 0x0080, 0x0100,
        0x0200, 0x0400, 0x0800, 0x1000, 0x2000,
        0x4000, 0x8000, 0xAAAA, 0x5555, 0x00FF
    };
    
    for (size_t i = 0; i < sizeof(test_cases16)/sizeof(test_cases16[0]); i++) {
        src16 = test_cases16[i];
        
        CLEAR_FLAGS;
        asm volatile (
            "movw %[src_val], %%ax\n\t"
            "bsfw %%ax, %%bx\n\t"
            "pushfq\n\t"
            "popq %%rcx"
            : "=b" (dest16), "=c" (eflags)
            : [src_val] "r" (src16)
            : "ax", "cc"
        );

        if (src16 == 0) {
            printf("BSF src(0x%04x) = undefined, ZF=%d\n", src16, (eflags & X_ZF) ? 1 : 0);
        } else {
            printf("BSF src(0x%04x) = %d, ZF=%d\n", src16, dest16, (eflags & X_ZF) ? 1 : 0);
        }
    }

    // 32-bit tests
    uint32_t src32, dest32;
    uint32_t test_cases32[] = {
        0x00000000, 0x00000001, 0x80000000, 0x00010000,
        0x00008000, 0x40000000, 0x00000010, 0x00000020,
        0x00000040, 0x00000080, 0x00000100, 0x00000200,
        0x00000400, 0x00000800, 0x00001000, 0x00002000,
        0x00004000, 0x00008000, 0x00100000, 0x00200000,
        0x00400000, 0x00800000, 0x01000000, 0x02000000,
        0x04000000, 0x08000000, 0x10000000, 0x20000000,
        0x40000000, 0x80000000, 0xAAAAAAAA, 0x55555555
    };
    
    for (size_t i = 0; i < sizeof(test_cases32)/sizeof(test_cases32[0]); i++) {
        src32 = test_cases32[i];
        
        CLEAR_FLAGS;
        asm volatile (
            "movl %[src_val], %%eax\n\t"
            "bsfl %%eax, %%ebx\n\t"
            "pushfq\n\t"
            "popq %%rcx"
            : "=b" (dest32), "=c" (eflags)
            : [src_val] "r" (src32)
            : "ax", "cc"
        );

        if (src32 == 0) {
            printf("BSF src(0x%08x) = undefined, ZF=%d\n", src32, (eflags & X_ZF) ? 1 : 0);
        } else {
            printf("BSF src(0x%08x) = %d, ZF=%d\n", src32, dest32, (eflags & X_ZF) ? 1 : 0);
        }
    }

    // 64-bit tests
    uint64_t src64, dest64;
    uint64_t test_cases64[] = {
        0x0000000000000000, 0x0000000000000001, 0x8000000000000000,
        0x0000000000001000, 0x0000000100000000, 0x1000000000000000,
        0x0000000000000002, 0x0000000000000004, 0x0000000000000008,
        0x0000000000000010, 0x0000000000000020, 0x0000000000000040,
        0x0000000000000080, 0x0000000000000100, 0x0000000000000200,
        0x0000000000000400, 0x0000000000000800, 0x0000000000001000,
        0x0000000000002000, 0x0000000000004000, 0x0000000000008000,
        0x0000000000010000, 0x0000000000020000, 0x0000000000040000,
        0x0000000000080000, 0x0000000000100000, 0x0000000000200000,
        0x0000000000400000, 0x0000000000800000, 0x0000000001000000,
        0x0000000002000000, 0x0000000004000000, 0x0000000008000000,
        0x0000000010000000, 0x0000000020000000, 0x0000000040000000,
        0x0000000080000000, 0x0000000100000000, 0x0000000200000000,
        0x0000000400000000, 0x0000000800000000, 0x0000001000000000,
        0x0000002000000000, 0x0000004000000000, 0x0000008000000000,
        0x0000010000000000, 0x0000020000000000, 0x0000040000000000,
        0x0000080000000000, 0x0000100000000000, 0x0000200000000000,
        0x0000400000000000, 0x0000800000000000, 0x0001000000000000,
        0x0002000000000000, 0x0004000000000000, 0x0008000000000000,
        0x0010000000000000, 0x0020000000000000, 0x0040000000000000,
        0x0080000000000000, 0x0100000000000000, 0x0200000000000000,
        0x0400000000000000, 0x0800000000000000, 0x1000000000000000,
        0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
        0xAAAAAAAAAAAAAAAA, 0x5555555555555555
    };
    
    for (size_t i = 0; i < sizeof(test_cases64)/sizeof(test_cases64[0]); i++) {
        src64 = test_cases64[i];
        
        CLEAR_FLAGS;
        asm volatile (
            "movq %[src_val], %%rax\n\t"
            "bsfq %%rax, %%rbx\n\t"
            "pushfq\n\t"
            "popq %%rcx"
            : "=b" (dest64), "=c" (eflags)
            : [src_val] "r" (src64)
            : "ax", "cc"
        );

        if (src64 == 0) {
            printf("BSF src(0x%016lx) = undefined, ZF=%d\n", src64, (eflags & X_ZF) ? 1 : 0);
        } else {
            printf("BSF src(0x%016lx) = %ld, ZF=%d\n", src64, dest64, (eflags & X_ZF) ? 1 : 0);
        }
    }
}

void test_bsf_reg_mem() {
    printf("\nTesting BSF reg, mem\n");
    
    uint64_t src_values[] = {
        0x00000000, 0x00000001, 0x80000000, 0x00010000,
        0x0000000000000001, 0x8000000000000000, 0x0000000100000000
    };
    
    for (size_t i = 0; i < sizeof(src_values)/sizeof(src_values[0]); i++) {
        uint64_t src_val = src_values[i];
        uint64_t dest;
        uint32_t eflags;
        
        CLEAR_FLAGS;
        asm volatile (
            "movq %[src_val], %%rax\n\t"
            "movq %%rax, %[mem]\n\t"
            "bsfq %[mem], %%rbx\n\t"
            "pushfq\n\t"
            "popq %%rcx"
            : "=b" (dest), "=c" (eflags), [mem] "=m" (src_val)
            : [src_val] "r" (src_val)
            : "rax", "cc"
        );

        if (src_val == 0) {
            printf("BSF mem(0x%016lx) = undefined, ZF=%d\n", src_val, (eflags & X_ZF) ? 1 : 0);
        } else {
            printf("BSF mem(0x%016lx) = %ld, ZF=%d\n", src_val, dest, (eflags & X_ZF) ? 1 : 0);
        }
    }
}

int main() {
    test_bsf_reg_reg();
    test_bsf_reg_mem();
    
    printf("\nBSF tests completed.\n");
    return 0;
}
