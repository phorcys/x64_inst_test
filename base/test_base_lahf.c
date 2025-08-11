#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Expected AH bit pattern:
// Bit 7: SF (Sign Flag)
// Bit 6: ZF (Zero Flag)
// Bit 5: 0 (Fixed)
// Bit 4: AF (Auxiliary Flag)
// Bit 3: 0 (Fixed)
// Bit 2: PF (Parity Flag)
// Bit 1: 1 (Fixed)
// Bit 0: CF (Carry Flag)

int main() {
    int errors = 0;
    
    printf("Starting LAHF tests\n");
    printf("==================\n\n");
    
    // Test all combinations of SF, ZF, AF, PF, CF
    for (int i = 0; i < 32; i++) {
        uint8_t cf = (i >> 0) & 1;
        uint8_t pf = (i >> 1) & 1;
        uint8_t af = (i >> 2) & 1;
        uint8_t zf = (i >> 3) & 1;
        uint8_t sf = (i >> 4) & 1;
        
        // Build expected AH value
        uint8_t expected_ah = (sf << 7) | (zf << 6) | (af << 4) | (pf << 2) | 0x02 | cf;
        
        // Build eflags value
        uint64_t eflags = (cf ? X_CF : 0) | 
                          (pf ? X_PF : 0) | 
                          (af ? X_AF : 0) | 
                          (zf ? X_ZF : 0) | 
                          (sf ? X_SF : 0);
        
        // Set flags
        CLEAR_FLAGS;
        set_eflags(eflags);
        
        // Get AH via LAHF
        uint8_t ah_value;
        __asm__ __volatile__(
            "lahf\n"
            "movb %%ah, %0"
            : "=r" (ah_value)
            :
            : "memory"
        );
        
        // Verify results
        printf("Test case %d:\n", i+1);
        printf("  Flags set: SF=%d, ZF=%d, AF=%d, PF=%d, CF=%d\n", sf, zf, af, pf, cf);
        printf("  Expected AH: 0x%02X\n", expected_ah);
        printf("  Actual AH:   0x%02X\n", ah_value);
        
        if (ah_value == expected_ah) {
            printf("  Result: PASSED\n\n");
        } else {
            printf("  Result: FAILED\n\n");
            errors++;
        }
    }
    
    printf("\nLAHF tests completed\n");
    printf("====================\n");
    printf("Total tests: 32\n");
    printf("Passed: %d\n", 32 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
