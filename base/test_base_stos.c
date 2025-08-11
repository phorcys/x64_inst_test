#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

#define BUF_SIZE 32

int main() {
    printf("STOS instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    uint8_t buffer[BUF_SIZE];
    
    // Test STOSB (byte)
    memset(buffer, 0, sizeof(buffer));
    uint8_t al_val = 0xAA;
    uint8_t *ptr = buffer + 8;
    
    __asm__ __volatile__ (
        "mov %1, %%rdi\n\t"
        "mov %2, %%al\n\t"
        "stosb"
        : "+D" (ptr)
        : "r" (ptr), "r" (al_val)
        : "al", "memory"
    );
    
    if (buffer[8] != al_val) {
        printf("Error: STOSB failed (expected 0x%02X, got 0x%02X)\n", al_val, buffer[8]);
        errors++;
    }
    if (ptr != buffer + 9) {
        printf("Error: STOSB pointer not incremented (expected %p, got %p)\n", buffer+9, ptr);
        errors++;
    }
    
    // Test STOSW (word)
    memset(buffer, 0, sizeof(buffer));
    uint16_t ax_val = 0xBBCC;
    ptr = buffer + 10;
    
    __asm__ __volatile__ (
        "mov %1, %%rdi\n\t"
        "mov %2, %%ax\n\t"
        "stosw"
        : "+D" (ptr)
        : "r" (ptr), "r" (ax_val)
        : "ax", "memory"
    );
    
    if (*(uint16_t*)(buffer+10) != ax_val) {
        printf("Error: STOSW failed (expected 0x%04X, got 0x%04X)\n", ax_val, *(uint16_t*)(buffer+10));
        errors++;
    }
    if (ptr != buffer + 12) {
        printf("Error: STOSW pointer not incremented (expected %p, got %p)\n", buffer+12, ptr);
        errors++;
    }
    
    // Test STOSD (doubleword)
    memset(buffer, 0, sizeof(buffer));
    uint32_t eax_val = 0xDDEEFF00;
    ptr = buffer + 12;
    
    __asm__ __volatile__ (
        "mov %1, %%rdi\n\t"
        "mov %2, %%eax\n\t"
        "stosl"
        : "+D" (ptr)
        : "r" (ptr), "r" (eax_val)
        : "eax", "memory"
    );
    
    if (*(uint32_t*)(buffer+12) != eax_val) {
        printf("Error: STOSD failed (expected 0x%08X, got 0x%08X)\n", eax_val, *(uint32_t*)(buffer+12));
        errors++;
    }
    if (ptr != buffer + 16) {
        printf("Error: STOSD pointer not incremented (expected %p, got %p)\n", buffer+16, ptr);
        errors++;
    }
    
    // Test STOSQ (quadword)
    memset(buffer, 0, sizeof(buffer));
    uint64_t rax_val = 0x1122334455667788;
    ptr = buffer + 16;
    
    __asm__ __volatile__ (
        "mov %1, %%rdi\n\t"
        "mov %2, %%rax\n\t"
        "stosq"
        : "+D" (ptr)
        : "r" (ptr), "r" (rax_val)
        : "rax", "memory"
    );
    
    if (*(uint64_t*)(buffer+16) != rax_val) {
        printf("Error: STOSQ failed (expected 0x%016lX, got 0x%016lX)\n", rax_val, *(uint64_t*)(buffer+16));
        errors++;
    }
    if (ptr != buffer + 24) {
        printf("Error: STOSQ pointer not incremented (expected %p, got %p)\n", buffer+24, ptr);
        errors++;
    }
    
    // Test with direction flag set (reverse direction)
    memset(buffer, 0, sizeof(buffer));
    ptr = buffer + 20;
    al_val = 0x55;
    
    __asm__ __volatile__ (
        "std\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%al\n\t"
        "stosb\n\t"
        "cld"
        : "+D" (ptr)
        : "r" (ptr), "r" (al_val)
        : "al", "memory", "cc"
    );
    
    if (buffer[20] != al_val) {
        printf("Error: STOSB with STD failed (expected 0x%02X, got 0x%02X)\n", al_val, buffer[20]);
        errors++;
    }
    if (ptr != buffer + 19) {
        printf("Error: STOSB with STD pointer not decremented (expected %p, got %p)\n", buffer+19, ptr);
        errors++;
    }
    
    printf("\nSTOS test completed\n");
    printf("===================\n");
    printf("Total tests: 5\n");
    printf("Passed: %d\n", 5 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
