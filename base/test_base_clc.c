#include <stdio.h>
#include <stdint.h>
#include "base.h"

#define CF_MASK 0x0001

int main() {
    printf("Starting CLC (Clear Carry Flag) instruction test\n");
    
    // Test 1: Clear carry flag when it's set
    printf("Test 1: Clear carry flag when set\n");
    unsigned int flags_before, flags_after;
    
    asm volatile (
        "stc\n\t"                 // Set carry flag
        "pushf\n\t"               // Save flags to stack
        "pop %q0\n\t"             // Load flags into flags_before (64-bit)
        "clc\n\t"                 // Clear carry flag
        "pushf\n\t"               // Save flags to stack
        "pop %q1"                 // Load flags into flags_after (64-bit)
        : "=r" (flags_before), "=r" (flags_after)
        : 
        : "cc"
    );
    
 //   printf("  Flags before: 0x%04X, Flags after: 0x%04X\n", flags_before, flags_after);
    printf("  CF before: %d, CF after: %d - %s\n", 
           (flags_before & CF_MASK) ? 1 : 0, 
           (flags_after & CF_MASK) ? 1 : 0,
           (flags_after & CF_MASK) ? "FAIL" : "PASS");
    
    // Test 2: Clear carry flag when it's already cleared
    printf("\nTest 2: Clear carry flag when already cleared\n");
    asm volatile (
        "clc\n\t"                 // Clear carry flag
        "pushf\n\t"               // Save flags to stack
        "pop %q0\n\t"             // Load flags into flags_before (64-bit)
        "clc\n\t"                 // Clear carry flag again
        "pushf\n\t"               // Save flags to stack
        "pop %q1"                 // Load flags into flags_after (64-bit)
        : "=r" (flags_before), "=r" (flags_after)
        : 
        : "cc"
    );
    
//  printf("  Flags before: 0x%04X, Flags after: 0x%04X\n", flags_before, flags_after);
    printf("  CF before: %d, CF after: %d - %s\n", 
           (flags_before & CF_MASK) ? 1 : 0, 
           (flags_after & CF_MASK) ? 1 : 0,
           (flags_after & CF_MASK) ? "FAIL" : "PASS");
    
    // Test 3: Verify other flags are not affected
    printf("\nTest 3: Verify other flags are not affected\n");
    asm volatile (
        "pushf\n\t"
        "pop %q0\n\t"             // Get initial flags (64-bit)
        "stc\n\t"                 // Set carry flag
        "clc\n\t"                 // Clear carry flag
        "pushf\n\t"
        "pop %q1"                 // Get flags after CLC (64-bit)
        : "=r" (flags_before), "=r" (flags_after)
        : 
        : "cc"
    );
    
    unsigned int changed_flags = flags_before ^ flags_after;
    changed_flags &= ~CF_MASK;  // Ignore CF changes
    
    printf("  Changed flags (excluding CF): 0x%04X - %s\n", 
           changed_flags,
           changed_flags ? "FAIL (other flags changed)" : "PASS");
    
    printf("\nCLC instruction tests completed!\n");
    return 0;
}
