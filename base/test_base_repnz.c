#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("REPNZ prefix instruction test\n");
    printf("=============================\n");
    
    int errors = 0;
    
    // Test 1: REPNZ SCASB (Byte scan for non-match)
    char data[20] = "abcXdefXghi";
    char target = 'X';
    size_t position;
    size_t count = strlen(data);
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%al\n\t"
        "mov %3, %%rcx\n\t"
        "repnz scasb\n\t"
        "jnz not_found\n\t"
        "dec %%rdi\n\t"   // Point back to found char
        "mov %%rdi, %0\n\t"
        "jmp done\n\t"
        "not_found:\n\t"
        "mov $0, %0\n\t"
        "done:\n\t"
        : "=r" (position)
        : "r" (data), "r" (target), "r" (count)
        : "rdi", "rax", "rcx", "cc"
    );
    
    if (position == 0 || *(char*)position != 'X') {
        printf("Error: REPNZ SCASB failed to find 'X'. Position: %p\n", (void*)position);
        errors++;
    } else {
        printf("REPNZ SCASB: PASS (found at %ld)\n", position - (size_t)data);
    }
    
    // Test 2: REPNZ CMPSB (Byte compare until mismatch)
    char str1[20] = "match1234";
    char str2[20] = "match5678";
    int result;
    count = 5;  // Compare first 5 bytes (should match)
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rsi\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rcx\n\t"
        "repnz cmpsb\n\t"
        "setz %%al"
        : "=a" (result)
        : "r" (str1), "r" (str2), "r" (count)
        : "rsi", "rdi", "rcx", "cc"
    );
    
    if (!result) {
        printf("Error: REPNZ CMPSB failed. Expected match for first 5 bytes\n");
        errors++;
    } else {
        printf("REPNZ CMPSB (match): PASS\n");
    }
    
    // Test 3: REPNZ CMPSB (Early mismatch)
    count = 10;
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rsi\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rcx\n\t"
        "repnz cmpsb\n\t"
        "setz %%al"
        : "=a" (result)
        : "r" (str1), "r" (str2), "r" (count)
        : "rsi", "rdi", "rcx", "cc"
    );
    
    if (result) {
        printf("Error: REPNZ CMPSB failed. Expected mismatch after 5 bytes\n");
        errors++;
    } else {
        printf("REPNZ CMPSB (mismatch): PASS\n");
    }
    
    // Test 4: REPNZ SCASW (Word scan for non-match)
    uint16_t words[5] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555};
    uint16_t target_word = 0x3333;
    size_t word_position;
    count = 5;
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%ax\n\t"
        "mov %3, %%rcx\n\t"
        "repnz scasw\n\t"
        "jnz not_found1\n\t"
        "sub $2, %%rdi\n\t"   // Point back to found word
        "mov %%rdi, %0\n\t"
        "jmp done1\n\t"
        "not_found1:\n\t"
        "mov $0, %0\n\t"
        "done1:\n\t"
        : "=r" (word_position)
        : "r" (words), "r" (target_word), "r" (count)
        : "rdi", "rax", "rcx", "cc"
    );
    
    uint16_t* found_word = (uint16_t*)word_position;
    if (word_position == 0 || *found_word != 0x3333) {
        printf("Error: REPNZ SCASW failed to find 0x3333. Found: 0x%x\n", found_word ? *found_word : 0);
        errors++;
    } else {
        printf("REPNZ SCASW: PASS (found at position %ld)\n", (found_word - words));
    }
    
    // Test 5: Zero count
    char empty_data[10] = "test";
    char empty_target = 'X';
    count = 0;
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %0, %%rdi\n\t"
        "mov %1, %%al\n\t"
        "mov %2, %%rcx\n\t"
        "repnz scasb\n\t"
        : 
        : "r" (empty_data), "r" (empty_target), "r" (count)
        : "rdi", "rax", "rcx", "cc"
    );
    
    printf("Zero count: PASS (no crash)\n");
    
    printf("\nREPNZ test completed\n");
    printf("====================\n");
    printf("Total tests: 5\n");
    printf("Passed: %d\n", 5 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
