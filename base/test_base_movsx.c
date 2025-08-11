#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("MOVSX instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    
    // Test byte to word
    int8_t byte_val = -5;
    int16_t word_result;
    __asm__ __volatile__ (
        "movsbw %1, %0"
        : "=r" (word_result)
        : "r" (byte_val)
    );
    if (word_result != -5) {
        printf("Error: MOVSBW expected -5, got %d\n", word_result);
        errors++;
    } else {
        printf("MOVSBW (byte to word): PASS\n");
    }
    
    // Test byte to dword
    int32_t dword_result;
    __asm__ __volatile__ (
        "movsbl %1, %0"
        : "=r" (dword_result)
        : "r" (byte_val)
    );
    if (dword_result != -5) {
        printf("Error: MOVSBL expected -5, got %d\n", dword_result);
        errors++;
    } else {
        printf("MOVSBL (byte to dword): PASS\n");
    }
    
    // Test byte to qword
    int64_t qword_result;
    __asm__ __volatile__ (
        "movsbq %1, %0"
        : "=r" (qword_result)
        : "r" (byte_val)
    );
    if (qword_result != -5) {
        printf("Error: MOVSBQ expected -5, got %ld\n", qword_result);
        errors++;
    } else {
        printf("MOVSBQ (byte to qword): PASS\n");
    }
    
    // Test word to dword
    int16_t word_val = -1000;
    __asm__ __volatile__ (
        "movswl %1, %0"
        : "=r" (dword_result)
        : "r" (word_val)
    );
    if (dword_result != -1000) {
        printf("Error: MOVSWL expected -1000, got %d\n", dword_result);
        errors++;
    } else {
        printf("MOVSWL (word to dword): PASS\n");
    }
    
    // Test word to qword
    __asm__ __volatile__ (
        "movswq %1, %0"
        : "=r" (qword_result)
        : "r" (word_val)
    );
    if (qword_result != -1000) {
        printf("Error: MOVSWQ expected -1000, got %ld\n", qword_result);
        errors++;
    } else {
        printf("MOVSWQ (word to qword): PASS\n");
    }
    
    // Test dword to qword
    int32_t dword_val = -2000000000;
    __asm__ __volatile__ (
        "movslq %1, %0"
        : "=r" (qword_result)
        : "r" (dword_val)
    );
    if (qword_result != -2000000000) {
        printf("Error: MOVSLQ expected -2000000000, got %ld\n", qword_result);
        errors++;
    } else {
        printf("MOVSLQ (dword to qword): PASS\n");
    }
    
    // Test memory operand
    int8_t mem_byte = 0x80; // -128
    int16_t mem_word_result;
    __asm__ __volatile__ (
        "movsbw %1, %0"
        : "=r" (mem_word_result)
        : "m" (mem_byte)
    );
    if (mem_word_result != -128) {
        printf("Error: Memory operand MOVSBW expected -128, got %d\n", mem_word_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test positive value
    byte_val = 0x7F; // 127
    __asm__ __volatile__ (
        "movsbq %1, %0"
        : "=r" (qword_result)
        : "r" (byte_val)
    );
    if (qword_result != 127) {
        printf("Error: Positive value expected 127, got %ld\n", qword_result);
        errors++;
    } else {
        printf("Positive value: PASS\n");
    }
    
    printf("\nMOVSX test completed\n");
    printf("====================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
