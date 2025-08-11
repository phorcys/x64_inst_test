#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("SLDT instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    
    // Store LDTR selector twice
    uint16_t ldt1 = 0;
    uint16_t ldt2 = 0;
    
    __asm__ __volatile__ ("sldt %0" : "=r" (ldt1));
    __asm__ __volatile__ ("sldt %0" : "=r" (ldt2));
    
    printf("LDTR selector: 0x%04X\n", ldt1);
    
    // Verify consistency between two reads
    if (ldt1 != ldt2) {
        printf("Error: Inconsistent LDTR reads (0x%04X vs 0x%04X)\n", ldt1, ldt2);
        errors++;
    }
    
    // Note: We can't assume LDTR value as it's system-dependent
    // But we can verify that the value is a valid segment selector or zero
    // Bit 0-1: RPL, Bit 2: TI (0=GDT, 1=LDT), Bit 3-15: Index
    if ((ldt1 & 0xFFFC) != ldt1) {
        printf("Error: Invalid segment selector format (0x%04X)\n", ldt1);
        errors++;
    }
    
    printf("\nSLDT test completed\n");
    printf("===================\n");
    printf("Total tests: 2\n");
    printf("Passed: %d\n", 2 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
