#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("MOVZX instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    
    // Test byte to word
    uint8_t byte_val = 0x80;
    uint16_t word_result;
    __asm__ __volatile__ (
        "movzbw %1, %0"
        : "=r" (word_result)
        : "r" (byte_val)
    );
    if (word_result != 0x0080) {
        printf("Error: MOVZBW expected 0x0080, got 0x%x\n", word_result);
        errors++;
    } else {
        printf("MOVZBW (byte to word): PASS\n");
    }
    
    // Test byte to dword
    uint32_t dword_result;
    __asm__ __volatile__ (
        "movzbl %1, %0"
        : "=r" (dword_result)
        : "r" (byte_val)
    );
    if (dword_result != 0x00000080) {
        printf("Error: MOVZBL expected 0x00000080, got 0x%x\n", dword_result);
        errors++;
    } else {
        printf("MOVZBL (byte to dword): PASS\n");
    }
    
    // Test byte to qword
    uint64_t qword_result;
    __asm__ __volatile__ (
        "movzbq %1, %0"
        : "=r" (qword_result)
        : "r" (byte_val)
    );
    if (qword_result != 0x0000000000000080) {
        printf("Error: MOVZBQ expected 0x0000000000000080, got 0x%lx\n", qword_result);
        errors++;
    } else {
        printf("MOVZBQ (byte to qword): PASS\n");
    }
    
    // Test word to dword
    uint16_t word_val = 0x8000;
    __asm__ __volatile__ (
        "movzwl %1, %0"
        : "=r" (dword_result)
        : "r" (word_val)
    );
    if (dword_result != 0x00008000) {
        printf("Error: MOVZWL expected 0x00008000, got 0x%x\n", dword_result);
        errors++;
    } else {
        printf("MOVZWL (word to dword): PASS\n");
    }
    
    // Test word to qword
    __asm__ __volatile__ (
        "movzwq %1, %0"
        : "=r" (qword_result)
        : "r" (word_val)
    );
    if (qword_result != 0x0000000000008000) {
        printf("Error: MOVZWQ expected 0x0000000000008000, got 0x%lx\n", qword_result);
        errors++;
    } else {
        printf("MOVZWQ (word to qword): PASS\n");
    }
    
    // Test memory operand
    uint8_t mem_byte = 0xFF;
    uint32_t mem_dword_result;
    __asm__ __volatile__ (
        "movzbl %1, %0"
        : "=r" (mem_dword_result)
        : "m" (mem_byte)
    );
    if (mem_dword_result != 0x000000FF) {
        printf("Error: Memory operand MOVZBL expected 0x000000FF, got 0x%x\n", mem_dword_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test positive value
    byte_val = 0x7F;
    __asm__ __volatile__ (
        "movzbq %1, %0"
        : "=r" (qword_result)
        : "r" (byte_val)
    );
    if (qword_result != 0x000000000000007F) {
        printf("Error: Positive value expected 0x000000000000007F, got 0x%lx\n", qword_result);
        errors++;
    } else {
        printf("Positive value: PASS\n");
    }
    
    // Test zero value
    byte_val = 0x00;
    __asm__ __volatile__ (
        "movzbq %1, %0"
        : "=r" (qword_result)
        : "r" (byte_val)
    );
    if (qword_result != 0) {
        printf("Error: Zero value expected 0, got 0x%lx\n", qword_result);
        errors++;
    } else {
        printf("Zero value: PASS\n");
    }
    
    printf("\nMOVZX test completed\n");
    printf("====================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
