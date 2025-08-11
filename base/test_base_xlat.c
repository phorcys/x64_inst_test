#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test XLAT in 64-bit mode
static void test_xlat(uint8_t *table, uint8_t index, uint8_t expected) {
    uint8_t result;
    uint64_t flags_before, flags_after;
    
    CLEAR_FLAGS;
    flags_before = get_eflags();
    
    asm volatile (
        "movq %[table], %%rbx \n\t" // Load table address into RBX
        "movb %[index], %%al \n\t"  // Set index in AL
        "xlatb \n\t"                // Perform XLAT
        : "=a" (result)
        : [table] "r" (table), [index] "r" (index)
        : "rbx", "memory"
    );
    
    flags_after = get_eflags();
    
    printf("  Index: 0x%02x, Expected: 0x%02x, Result: 0x%02x - %s\n",
           index, expected, result, 
           result == expected ? "PASS" : "FAIL");
    
    if (result != expected) {
        printf("    ERROR: Incorrect value\n");
    }
    
    verify_flags_unchanged(flags_before, flags_after, "XLAT");
}

// Test XLAT with segment overrides
static void test_segment_override(uint8_t *table, uint8_t index, uint8_t expected) {
    uint8_t result;
    
    // Test with GS override
    asm volatile (
        "movq %[table], %%rbx \n\t" // Load table address into RBX
        "movb %[index], %%al \n\t"  // Set index in AL
        "gs xlatb \n\t"             // Perform XLAT with GS override
        : "=a" (result)
        : [table] "r" (table), [index] "r" (index)
        : "rbx", "memory"
    );
    
    printf("  GS Override, Index: 0x%02x, Expected: 0x%02x, Result: 0x%02x - %s\n",
           index, expected, result, 
           result == expected ? "PASS" : "FAIL");
    
    if (result != expected) {
        printf("    ERROR: Incorrect value with GS override\n");
    }
}

// Test XLAT with explicit operand
static void test_explicit_operand(uint8_t *table, uint8_t index, uint8_t expected) {
    uint8_t result;
    
    asm volatile (
        "movq %[table], %%rbx \n\t" // Load table address into RBX
        "movb %[index], %%al \n\t"  // Set index in AL
        "xlatb \n\t"                // Perform XLAT with explicit operand
        : "=a" (result)
        : [table] "r" (table), [index] "r" (index)
        : "rbx", "memory"
    );
    
    printf("  Explicit operand, Index: 0x%02x, Expected: 0x%02x, Result: 0x%02x - %s\n",
           index, expected, result, 
           result == expected ? "PASS" : "FAIL");
    
    if (result != expected) {
        printf("    ERROR: Incorrect value with explicit operand\n");
    }
}

// Test boundary conditions
static void test_boundaries(uint8_t *table) {
    // Test index 0
    test_xlat(table, 0, table[0]);
    
    // Test index 255 (max)
    test_xlat(table, 0xFF, table[0xFF]);
}

int main() {
    printf("XLAT instruction comprehensive test (64-bit mode)\n");
    printf("===============================================\n");
    
    // Create a 256-byte table with known values
    uint8_t table[256];
    for (int i = 0; i < 256; i++) {
        table[i] = (i + 1) & 0xFF; // Values 1-255, 0 at index 255
    }
    
    printf("\nTesting basic operations:\n");
    test_xlat(table, 10, table[10]);
    test_xlat(table, 20, table[20]);
    test_xlat(table, 30, table[30]);
    
    printf("\nTesting segment overrides:\n");
    test_segment_override(table, 40, table[40]);
    
    printf("\nTesting explicit operand form:\n");
    test_explicit_operand(table, 50, table[50]);
    
    printf("\nTesting boundary conditions:\n");
    test_boundaries(table);
    
    printf("\nXLAT test completed\n");
    printf("===================\n");
    
    return 0;
}
