#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test XLAT in 64-bit mode
static void test_xlat(uint8_t *table, uint8_t index, uint8_t expected) {
    uint8_t result = index; // Set AL to index
    uint64_t flags_before, flags_after;
    
    // Capture flags immediately before and after in the same asm block
    asm volatile (
        "pushfq\n\t"
        "pop %1\n\t"        // Save flags before
        "xlatb\n\t"         // Perform XLAT
        "pushfq\n\t"
        "pop %2\n\t"        // Save flags after
        : "+a" (result), "=&r" (flags_before), "=&r" (flags_after)
        : "b" (table)
        : "memory", "cc"
    );
    
    printf("  Index: 0x%02x, Expected: 0x%02x, Result: 0x%02x - %s\n",
           index, expected, result, 
           result == expected ? "PASS" : "FAIL");
    
    if (result != expected) {
        printf("    ERROR: Incorrect value\n");
    }
    
    if (flags_before != flags_after) {
        printf("    ERROR: Flags changed (expected unchanged)\n");
        printf("      Before: "); print_eflags_cond(flags_before, 0); printf("\n");
        printf("      After:  "); print_eflags_cond(flags_after, 0); printf("\n");
    }
}

// Test XLAT with segment overrides
static void test_segment_override(uint8_t *table, uint8_t index, uint8_t expected) {
    uint8_t result;
    
    result = index; // Set AL to index
    asm volatile (
        "gs xlatb \n\t"             // Perform XLAT with GS override
        : "+a" (result)
        : "b" (table)
        : "memory"
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
    
    result = index; // Set AL to index
    asm volatile (
        "xlatb \n\t"                // Perform XLAT with explicit operand
        : "+a" (result)
        : "b" (table)
        : "memory"
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
