#include <stdio.h>
#include <stdint.h>
#include "base.h"

#define DF_MASK 0x0400  // Direction Flag is bit 10
#define RELEVANT_FLAGS_MASK 0xCD5  // CF, PF, AF, ZF, SF, DF, OF

int main() {
    printf("Starting CLD (Clear Direction Flag) instruction test\n");
    
    // Test 1: Clear direction flag when it's set
    printf("Test 1: Clear direction flag when set\n");
    unsigned int flags_before, flags_after;
    
    asm volatile (
        "std\n\t"                 // Set direction flag
        "pushf\n\t"               // Save flags to stack
        "pop %q0\n\t"             // Load flags into flags_before (64-bit)
        "cld\n\t"                 // Clear direction flag
        "pushf\n\t"               // Save flags to stack
        "pop %q1"                 // Load flags into flags_after (64-bit)
        : "=r" (flags_before), "=r" (flags_after)
        : 
        : "cc"
    );
    
    uint16_t masked_before = flags_before & RELEVANT_FLAGS_MASK;
    uint16_t masked_after = flags_after & RELEVANT_FLAGS_MASK;
  //  printf("  Flags before: 0x%03X, Flags after: 0x%03X\n", masked_before, masked_after);
    printf("  DF before: %d, DF after: %d - %s\n", 
           (flags_before & DF_MASK) ? 1 : 0, 
           (flags_after & DF_MASK) ? 1 : 0,
           (flags_after & DF_MASK) ? "FAIL" : "PASS");
    
    // Test 2: Clear direction flag when it's already cleared
    printf("\nTest 2: Clear direction flag when already cleared\n");
    asm volatile (
        "cld\n\t"                 // Clear direction flag
        "pushf\n\t"               // Save flags to stack
        "pop %q0\n\t"             // Load flags into flags_before (64-bit)
        "cld\n\t"                 // Clear direction flag again
        "pushf\n\t"               // Save flags to stack
        "pop %q1"                 // Load flags into flags_after (64-bit)
        : "=r" (flags_before), "=r" (flags_after)
        : 
        : "cc"
    );
    
    masked_before = flags_before & RELEVANT_FLAGS_MASK;
    masked_after = flags_after & RELEVANT_FLAGS_MASK;
   // printf("  Flags before: 0x%03X, Flags after: 0x%03X\n", masked_before, masked_after);
    printf("  DF before: %d, DF after: %d - %s\n", 
           (flags_before & DF_MASK) ? 1 : 0, 
           (flags_after & DF_MASK) ? 1 : 0,
           (flags_after & DF_MASK) ? "FAIL" : "PASS");
    
    // Test 3: Verify other flags are not affected
    printf("\nTest 3: Verify other flags are not affected\n");
    asm volatile (
        "pushf\n\t"
        "pop %q0\n\t"             // Get initial flags (64-bit)
        "std\n\t"                 // Set direction flag
        "cld\n\t"                 // Clear direction flag
        "pushf\n\t"
        "pop %q1"                 // Get flags after CLD (64-bit)
        : "=r" (flags_before), "=r" (flags_after)
        : 
        : "cc"
    );
    
    // Only check relevant flags
    flags_before &= RELEVANT_FLAGS_MASK;
    flags_after &= RELEVANT_FLAGS_MASK;
    
    unsigned int changed_flags = flags_before ^ flags_after;
    changed_flags &= ~DF_MASK;  // Ignore DF changes
    
    printf("  Changed flags (excluding DF): 0x%03X - %s\n", 
           changed_flags,
           changed_flags ? "FAIL (other flags changed)" : "PASS");
    
    printf("\nCLD instruction tests completed!\n");
    return 0;
}
