#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("REPZ prefix instruction test\n");
    printf("===========================\n");
    
    int errors = 0;
    
    // Test 1: REPZ CMPSB (Byte compare until mismatch)
    char str1[20] = "identical";
    char str2[20] = "identical";
    char str3[20] = "identicaX";
    int result;
    size_t count = strlen(str1);
    
    // Full match
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rsi\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rcx\n\t"
        "repz cmpsb\n\t"
        "setz %%al"
        : "=a" (result)
        : "r" (str1), "r" (str2), "r" (count)
        : "rsi", "rdi", "rcx", "cc"
    );
    
    if (!result) {
        printf("Error: REPZ CMPSB failed. Expected match for identical strings\n");
        errors++;
    } else {
        printf("REPZ CMPSB (full match): PASS\n");
    }
    
    // Test 2: REPZ CMPSB (Mismatch)
    count = strlen(str1);
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rsi\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rcx\n\t"
        "repz cmpsb\n\t"
        "setz %%al"
        : "=a" (result)
        : "r" (str1), "r" (str3), "r" (count)
        : "rsi", "rdi", "rcx", "cc"
    );
    
    if (result) {
        printf("Error: REPZ CMPSB failed. Expected mismatch\n");
        errors++;
    } else {
        printf("REPZ CMPSB (mismatch): PASS\n");
    }
    
    // Test 3: REPZ SCASB (Byte scan for match)
    char data[20] = "0000X0000";
    char target = '0';
    size_t position;
    count = strlen(data);
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%al\n\t"
        "mov %3, %%rcx\n\t"
        "repz scasb\n\t"
        "jz all_matched\n\t"
        "dec %%rdi\n\t"   // Point back to mismatch
        "mov %%rdi, %0\n\t"
        "jmp done\n\t"
        "all_matched:\n\t"
        "mov $0, %0\n\t"
        "done:\n\t"
        : "=r" (position)
        : "r" (data), "r" (target), "r" (count)
        : "rdi", "rax", "rcx", "cc"
    );
    
    if (position == 0 || *(char*)position != 'X') {
        printf("Error: REPZ SCASB failed to find mismatch at 'X'. Position: %p\n", (void*)position);
        errors++;
    } else {
        printf("REPZ SCASB: PASS (found mismatch at %ld)\n", position - (size_t)data);
    }
    
    // Test 4: REPZ SCASW (Word scan for match)
    uint16_t words[5] = {0xAAAA, 0xAAAA, 0xBBBB, 0xAAAA, 0xAAAA};
    uint16_t target_word = 0xAAAA;
    size_t word_position;
    count = 5;
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%ax\n\t"
        "mov %3, %%rcx\n\t"
        "repz scasw\n\t"
        "jz all_matched1\n\t"
        "sub $2, %%rdi\n\t"   // Point back to mismatch
        "mov %%rdi, %0\n\t"
        "jmp done1\n\t"
        "all_matched1:\n\t"
        "mov $0, %0\n\t"
        "done1:\n\t"
        : "=r" (word_position)
        : "r" (words), "r" (target_word), "r" (count)
        : "rdi", "rax", "rcx", "cc"
    );
    
    uint16_t* found_word = (uint16_t*)word_position;
    if (word_position == 0 || *found_word != 0xBBBB) {
        printf("Error: REPZ SCASW failed to find 0xBBBB. Found: 0x%x\n", found_word ? *found_word : 0);
        errors++;
    } else {
        printf("REPZ SCASW: PASS (found at position %ld)\n", (found_word - words));
    }
    
    // Test 5: Partial match
    char partial[20] = "match1234";
    count = 5;  // Only check first 5 characters
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %1, %%rsi\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rcx\n\t"
        "repz cmpsb\n\t"
        "setz %%al"
        : "=a" (result)
        : "r" (str1), "r" (partial), "r" (count)
        : "rsi", "rdi", "rcx", "cc"
    );
    
    if (!result) {
        printf("Error: REPZ CMPSB failed. Expected match for first 5 bytes\n");
        errors++;
    } else {
        printf("Partial match: PASS\n");
    }
    
    // Test 6: Zero count
    char empty_data[10] = "test";
    char empty_target = '0';
    count = 0;
    
    __asm__ __volatile__ (
        "cld\n\t"
        "mov %0, %%rdi\n\t"
        "mov %1, %%al\n\t"
        "mov %2, %%rcx\n\t"
        "repz scasb\n\t"
        : 
        : "r" (empty_data), "r" (empty_target), "r" (count)
        : "rdi", "rax", "rcx", "cc"
    );
    
    printf("Zero count: PASS (no crash)\n");
    
    printf("\nREPZ test completed\n");
    printf("===================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
