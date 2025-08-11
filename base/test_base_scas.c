#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("SCAS instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    uint8_t byte_val, byte_arr[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint16_t word_val, word_arr[4] = {0xAABB, 0xCCDD, 0xEEFF, 0x1122};
    uint32_t dword_val, dword_arr[4] = {0xAABBCCDD, 0x11223344, 0x55667788, 0x99AABBCC};
    uint64_t qword_val, qword_arr[4] = {0xAABBCCDD11223344, 0x5566778899AABBCC, 0x1122334455667788, 0x99AABBCCDDEEFF00};
    uint64_t flags;
    uint64_t rdi_val;
    
    // Test 1: SCASB - byte comparison (forward)
    byte_val = 0xCC;
    uint8_t *byte_ptr = byte_arr + 2;  // Point to third element (0xCC)
    __asm__ __volatile__ (
        "cld\n\t"                   // Clear direction flag (forward)
        "mov %[ptr], %%rdi\n\t"     // Load address of third element
        "scasb\n\t"                 // Compare AL with byte at [RDI]
        "pushf\n\t"
        "pop %[flags]\n\t"
        "mov %%rdi, %[rdi_val]"     // Get new RDI value
        : [flags] "=r" (flags), [rdi_val] "=r" (rdi_val)
        : [ptr] "r" (byte_ptr), "a" (byte_val)
        : "rdi", "cc"
    );
    
    // Should find at index 2, RDI should point to next byte
    if (rdi_val != (uint64_t)(byte_arr + 3)) {
        printf("Error: SCASB forward RDI expected %p, got %p\n", byte_arr + 3, (void*)rdi_val);
        errors++;
    } else if (!(flags & (1 << 6))) { // ZF=1 (found)
        printf("Error: SCASB forward ZF not set correctly\n");
        errors++;
    } else {
        printf("SCASB forward: PASS\n");
    }
    
    // Test 2: SCASB - byte comparison (backward)
    byte_val = 0xBB;
    __asm__ __volatile__ (
        "std\n\t"                   // Set direction flag (backward)
        "lea 1+%[arr], %%rdi\n\t"   // Start at second element
        "scasb\n\t"                 // Compare AL with byte at [RDI]
        "pushf\n\t"
        "pop %[flags]\n\t"
        "mov %%rdi, %[rdi_val]\n\t"
        "cld"                       // Clear direction flag
        : [flags] "=r" (flags), [rdi_val] "=r" (rdi_val)
        : [arr] "m" (byte_arr), "a" (byte_val)
        : "rdi", "cc"
    );
    
    // Should find at index 1, RDI should point to previous byte
    if (rdi_val != (uint64_t)(byte_arr)) {
        printf("Error: SCASB backward RDI expected %p, got %p\n", byte_arr, (void*)rdi_val);
        errors++;
    } else if (!(flags & (1 << 6))) { // ZF=0 (not found)
        printf("Error: SCASB backward ZF not set correctly\n");
        errors++;
    } else {
        printf("SCASB backward: PASS\n");
    }
    
    // Test 3: SCASW - word comparison
    word_val = 0xCCDD;
    uint16_t *word_ptr = word_arr + 1;  // Point to second element (0xCCDD)
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %[ptr], %%rdi\n\t"     // Load address of second element
        "scasw\n\t"
        "pushf\n\t"
        "pop %[flags]\n\t"
        "mov %%rdi, %[rdi_val]"
        : [flags] "=r" (flags), [rdi_val] "=r" (rdi_val)
        : [ptr] "r" (word_ptr), "a" (word_val)
        : "rdi", "cc"
    );
    
    // Should find at index 1, RDI should point to next word
    if (rdi_val != (uint64_t)(word_arr + 2)) {
        printf("Error: SCASW RDI expected %p, got %p\n", word_arr + 2, (void*)rdi_val);
        errors++;
    } else if (!(flags & (1 << 6))) { // ZF=1 (found)
        printf("Error: SCASW ZF not set correctly\n");
        errors++;
    } else {
        printf("SCASW: PASS\n");
    }
    
    // Test 4: SCASD - dword comparison
    dword_val = 0x11223344;
    uint32_t *dword_ptr = dword_arr + 1;  // Point to second element (0x11223344)
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %[ptr], %%rdi\n\t"     // Load address of second element
        "scasl\n\t"
        "pushf\n\t"
        "pop %[flags]\n\t"
        "mov %%rdi, %[rdi_val]"
        : [flags] "=r" (flags), [rdi_val] "=r" (rdi_val)
        : [ptr] "r" (dword_ptr), "a" (dword_val)
        : "rdi", "cc"
    );
    
    // Should find at index 1, RDI should point to next dword
    if (rdi_val != (uint64_t)(dword_arr + 2)) {
        printf("Error: SCASD RDI expected %p, got %p\n", dword_arr + 2, (void*)rdi_val);
        errors++;
    } else if (!(flags & (1 << 6))) { // ZF=1 (found)
        printf("Error: SCASD ZF not set correctly\n");
        errors++;
    } else {
        printf("SCASD: PASS\n");
    }
    
    // Test 5: SCASQ - qword comparison (64-bit only)
    qword_val = 0x5566778899AABBCC;
    uint64_t *qword_ptr = qword_arr + 1;  // Point to second element (0x5566778899AABBCC)
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %[ptr], %%rdi\n\t"     // Load address of second element
        "scasq\n\t"
        "pushf\n\t"
        "pop %[flags]\n\t"
        "mov %%rdi, %[rdi_val]"
        : [flags] "=r" (flags), [rdi_val] "=r" (rdi_val)
        : [ptr] "r" (qword_ptr), "a" (qword_val)
        : "rdi", "cc"
    );
    
    // Should find at index 1, RDI should point to next qword
    if (rdi_val != (uint64_t)(qword_arr + 2)) {
        printf("Error: SCASQ RDI expected %p, got %p\n", qword_arr + 2, (void*)rdi_val);
        errors++;
    } else if (!(flags & (1 << 6))) { // ZF=1 (found)
        printf("Error: SCASQ ZF not set correctly\n");
        errors++;
    } else {
        printf("SCASQ: PASS\n");
    }
    
    // Test 6: Flags check (ZF when not found)
    byte_val = 0x11;
    __asm__ __volatile__ (
        "cld\n\t"
        "lea %[arr], %%rdi\n\t"
        "scasb\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (flags)
        : [arr] "m" (byte_arr), "a" (byte_val)
        : "rdi", "cc"
    );
    
    // Should not find 0x11, ZF=0
    if (flags & (1 << 6)) { // ZF=1 (found)
        printf("Error: SCASB not found ZF set incorrectly\n");
        errors++;
    } else {
        printf("SCASB not found: PASS\n");
    }
    
    printf("\nSCAS test completed\n");
    printf("===================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
