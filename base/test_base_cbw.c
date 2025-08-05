#include <stdio.h>
#include <stdint.h>
#include "base.h"

void test_cbw() {
    printf("Testing CBW (Convert Byte to Word):\n");
    
    uint8_t test_values[] = {0x00, 0x7F, 0x80, 0xFF, 0x55, 0xAA};
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_tests; i++) {
        uint8_t input = test_values[i];
        uint16_t expected = (int16_t)(int8_t)input;  // Manual sign extension
        
        uint16_t result;
        asm volatile (
            "movb %1, %%al\n\t"
            "cbw\n\t"
            "movw %%ax, %0"
            : "=r" (result)
            : "r" (input)
            : "al", "ax"
        );
        
        printf("  Input: 0x%02X, Expected: 0x%04X, Result: 0x%04X - %s\n",
               input, expected, result, 
               result == expected ? "PASS" : "FAIL");
    }
}

void test_cwde() {
    printf("Testing CWDE (Convert Word to Doubleword):\n");
    
    uint16_t test_values[] = {0x0000, 0x7FFF, 0x8000, 0xFFFF, 0x5555, 0xAAAA};
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_tests; i++) {
        uint16_t input = test_values[i];
        uint32_t expected = (int32_t)(int16_t)input;  // Manual sign extension
        
        uint32_t result;
        asm volatile (
            "movw %1, %%ax\n\t"
            "cwde\n\t"
            "movl %%eax, %0"
            : "=r" (result)
            : "r" (input)
            : "ax", "eax"
        );
        
        printf("  Input: 0x%04X, Expected: 0x%08X, Result: 0x%08X - %s\n",
               input, expected, result, 
               result == expected ? "PASS" : "FAIL");
    }
}

void test_cdqe() {
    printf("Testing CDQE (Convert Doubleword to Quadword):\n");
    
    uint32_t test_values[] = {0x00000000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA};
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_tests; i++) {
        uint32_t input = test_values[i];
        uint64_t expected = (int64_t)(int32_t)input;  // Manual sign extension
        
        uint64_t result;
        asm volatile (
            "movl %1, %%eax\n\t"
            "cdqe\n\t"
            "movq %%rax, %0"
            : "=r" (result)
            : "r" (input)
            : "rax", "eax"
        );
        
        printf("  Input: 0x%08X, Expected: 0x%016lX, Result: 0x%016lX - %s\n",
               input, expected, result, 
               result == expected ? "PASS" : "FAIL");
    }
}

int main() {
    printf("Starting CBW/CWDE/CDQE instruction tests\n");
    
    test_cbw();
    test_cwde();
    test_cdqe();
    
    printf("All CBW/CWDE/CDQE instruction tests completed!\n");
    return 0;
}
