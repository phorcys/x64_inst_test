#include <stdio.h>
#include "base.h"

int main() {
    unsigned long flags, flags_after;
    int passed = 0;
    int total = 0;
    const unsigned int relevant_flags = X_CF | X_PF | X_AF | X_ZF | X_SF | X_OF;

    // Test: Start with CF=0
    printf("=== Test: Start with CF=0 ===\n");
    asm volatile (
        "push $0x202\n\t"   // Push initial flags state (CF=0)
        "popfq\n\t"         // Pop into flags
        "pushfq; pop %0\n\t"    // Save flags state
        "cmc\n\t"
        "pushfq; pop %1\n\t"    // Save flags state
        : "=r"(flags),"=r"(flags_after)
        : 
        : "cc"
    );
    printf("Initial flags: 0x%lx\n", flags);
    print_eflags_cond(flags, relevant_flags);
    
    printf("After CMC: 0x%lx\n", flags_after);
    print_eflags_cond(flags_after, relevant_flags);
    
    // Verify CF is set (0->1) and others unchanged
    if (flags_after & X_CF) {
        printf("PASS: CF toggled from 0 to 1\n");
        passed++;
    } else {
        printf("FAIL: CF not set after CMC\n");
    }
    total++;
    
    // Test: Start with CF=0
    printf("=== Test: Start with CF=0 ===\n");
    asm volatile (
        "push $0x201\n\t"   // Push initial flags state (CF=0)
        "popfq\n\t"         // Pop into flags
        "pushfq; pop %0\n\t"    // Save flags state
        "cmc\n\t"
        "pushfq; pop %1\n\t"    // Save flags state
        : "=r"(flags),"=r"(flags_after)
        : 
        : "cc"
    );
    printf("Initial flags: 0x%lx\n", flags);
    print_eflags_cond(flags, relevant_flags);
    
    printf("After CMC: 0x%lx\n", flags_after);
    print_eflags_cond(flags_after, relevant_flags);
    
    // Verify CF is set (1->0) and others unchanged
    if (!(flags_after & X_CF)) {
        printf("PASS: CF toggled from 1 to 1\n");
        passed++;
    } else {
        printf("FAIL: CF not set after CMC\n");
    }
    total++;

    // Summary
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
