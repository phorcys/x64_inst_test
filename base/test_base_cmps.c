#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test CMPS with different data sizes and direction flag settings
static void test_cmps() {
    printf("Testing CMPS instruction:\n");
    printf("=========================\n");
    
    // Buffers for source and destination
    uint8_t src_buf[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                           0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t dst_buf[16] = {0x01, 0x02, 0x03, 0x04, 0x55, 0x66, 0x77, 0x88,
                           0x11, 0x22, 0x33, 0x44, 0x05, 0x06, 0x07, 0x08};
    
    // Test different sizes: byte (1), word (2), dword (4), qword (8)
    for (int size = 1; size <= 8; size *= 2) {
        printf("\nTesting CMPS with %d-byte elements:\n", size);
        printf("----------------------------------\n");
        
        // Test both directions: forward (DF=0) and backward (DF=1)
        for (int dir = 0; dir < 2; dir++) {
            const char* dir_str = dir ? "backward (DF=1)" : "forward (DF=0)";
            printf("  Direction: %s\n", dir_str);
            
            // Set up pointers to middle of buffers
            uint8_t* src_ptr = src_buf + 4;
            uint8_t* dst_ptr = dst_buf + 4;
            
            // Set known initial state for EFLAGS
            asm volatile (
                "push $0x202\n\t"  // Set bit 1 (reserved) and clear all other flags
                "popf\n\t"
            );
            if (dir) {
                asm volatile ("std");
            } else {
                asm volatile ("cld");
            }
            uint64_t flags_before = get_eflags();
            uint64_t rsi_before, rdi_before, rsi_after, rdi_after;
            
            // Perform comparison
            asm volatile (
                "mov %1, %%rsi\n\t"
                "mov %2, %%rdi\n\t"
                "mov %3, %%rcx\n\t"
                : "=S"(rsi_before), "=D"(rdi_before)
                : "r"(src_ptr), "r"(dst_ptr), "c"(1)
                : "memory"
            );
            
            // Execute CMPS instruction based on size
            switch (size) {
                case 1:
                    asm volatile ("cmpsb" : : "S"(rsi_before), "D"(rdi_before));
                    break;
                case 2:
                    asm volatile ("cmpsw" : : "S"(rsi_before), "D"(rdi_before));
                    break;
                case 4:
                    asm volatile ("cmpsl" : : "S"(rsi_before), "D"(rdi_before));
                    break;
                case 8:
                    asm volatile ("cmpsq" : : "S"(rsi_before), "D"(rdi_before));
                    break;
            }
            
            asm volatile (
                "mov %%rsi, %0\n\t"
                "mov %%rdi, %1\n\t"
                : "=r"(rsi_after), "=r"(rdi_after)
                : 
                : "memory"
            );
            
            uint64_t flags_after = get_eflags();
            
            // Calculate pointer differences
            intptr_t src_diff = (intptr_t)rsi_after - (intptr_t)rsi_before;
            intptr_t dst_diff = (intptr_t)rdi_after - (intptr_t)rdi_before;
            
            printf("    Pointers moved: src %+ld, dst %+ld\n", src_diff, dst_diff);
        // Only check relevant flags: CF, PF, AF, ZF, SF, OF (0x8D5)
        //printf("    Flags before: 0x%03lX, after: 0x%03lX\n", 
          //     flags_before & 0x8D5, flags_after & 0x8D5);
            
            // Show data being compared
            printf("    Data compared: src=0x");
            for (int i = 0; i < size; i++) {
                printf("%02X", src_ptr[i]);
            }
            printf(", dst=0x");
            for (int i = 0; i < size; i++) {
                printf("%02X", dst_ptr[i]);
            }
            printf("\n");
        }
    }
}

// Test repeated CMPS operations
static void test_rep_cmps() {
    printf("\nTesting REP CMPS:\n");
    printf("=================\n");
    
    // Buffers for source and destination (ensure proper alignment)
    __attribute__((aligned(16))) uint8_t src_buf[16] = {0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22, 0x33, 0x44,
                           0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
    __attribute__((aligned(16))) uint8_t dst_buf[16] = {0xAA, 0xBB, 0xCC, 0xDD, 0x55, 0x66, 0x77, 0x88,
                           0x11, 0x22, 0x33, 0x44, 0x99, 0xAA, 0xBB, 0xCC};
    
    // Test both REPE (REPZ) and REPNE (REPNZ)
    for (int rep_type = 0; rep_type < 2; rep_type++) {
        const char* rep_str = rep_type ? "REPNE" : "REPE";
        printf("  %s CMPSB test:\n", rep_str);
        
        // Set up pointers to aligned memory
        uint8_t* src_ptr = src_buf;
        uint8_t* dst_ptr = dst_buf;
        size_t count = 16;
        
        // Set known initial state for EFLAGS
        asm volatile (
            "push $0x202\n\t"  // Set bit 1 (reserved) and clear all other flags
            "popf\n\t"
        );
        asm volatile ("cld"); // Always forward for repeated ops
        uint64_t flags_before = get_eflags();
        uint64_t rsi_before = (uint64_t)src_ptr;
        uint64_t rdi_before = (uint64_t)dst_ptr;
        uint64_t rcx_before = count;
        uint64_t rsi_after, rdi_after, rcx_after;
        
        // Execute REP CMPS in a single asm block
        if (rep_type) {
            asm volatile (
                "mov %[rsi_before], %%rsi\n\t"
                "mov %[rdi_before], %%rdi\n\t"
                "mov %[rcx_before], %%rcx\n\t"
                "repne cmpsb\n\t"
                "mov %%rsi, %[rsi_after]\n\t"
                "mov %%rdi, %[rdi_after]\n\t"
                "mov %%rcx, %[rcx_after]\n\t"
                : [rsi_after] "=r" (rsi_after),
                  [rdi_after] "=r" (rdi_after),
                  [rcx_after] "=r" (rcx_after)
                : [rsi_before] "r" (rsi_before),
                  [rdi_before] "r" (rdi_before),
                  [rcx_before] "r" (rcx_before)
                : "rsi", "rdi", "rcx", "memory", "cc"
            );
        } else {
            asm volatile (
                "mov %[rsi_before], %%rsi\n\t"
                "mov %[rdi_before], %%rdi\n\t"
                "mov %[rcx_before], %%rcx\n\t"
                "repe cmpsb\n\t"
                "mov %%rsi, %[rsi_after]\n\t"
                "mov %%rdi, %[rdi_after]\n\t"
                "mov %%rcx, %[rcx_after]\n\t"
                : [rsi_after] "=r" (rsi_after),
                  [rdi_after] "=r" (rdi_after),
                  [rcx_after] "=r" (rcx_after)
                : [rsi_before] "r" (rsi_before),
                  [rdi_before] "r" (rdi_before),
                  [rcx_before] "r" (rcx_before)
                : "rsi", "rdi", "rcx", "memory", "cc"
            );
        }
        
        uint64_t flags_after = get_eflags();
        
        // Calculate comparisons done
        size_t comparisons = count - rcx_after;
        printf("    Comparisons: %zu, remaining RCX: %lu\n", comparisons, rcx_after);
            printf("    Pointers moved: src %+ld, dst %+ld\n",
                   (int64_t)(rsi_after - rsi_before),
                   (int64_t)(rdi_after - rdi_before));
        // Only check relevant flags: CF, PF, AF, ZF, SF, OF (0x8D5)
       // printf("    Flags before: 0x%03lX, after: 0x%03lX\n", 
           //    flags_before & 0x8D5, flags_after & 0x8D5);
        
        // Show mismatch location if any
        if (rcx_after > 0 && rcx_after < count) {
            size_t mismatch_index = count - rcx_after - 1;
            printf("    First mismatch at index %zu: src=0x%02X, dst=0x%02X\n",
                   mismatch_index, src_buf[mismatch_index], dst_buf[mismatch_index]);
        }
    }
}

int main() {
    test_cmps();
    test_rep_cmps();
    printf("\nCMPS tests completed.\n");
    return 0;
}
