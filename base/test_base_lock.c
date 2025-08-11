#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("LOCK instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    uint32_t counter = 0;
    uint32_t expected = 0;
    
    // Test LOCK ADD with memory operand
    uint32_t value = 5;
    uint32_t increment = 3;
    expected = value + increment;
    
    __asm__ __volatile__ (
        "lock add %[inc], %[val]"
        : [val] "+m" (value)
        : [inc] "r" (increment)
    );
    
    if (value != expected) {
        printf("Error: LOCK ADD expected %u, got %u\n", expected, value);
        errors++;
    } else {
        printf("LOCK ADD test: PASS\n");
    }
    
    // Test LOCK XADD (exchange and add)
    value = 10;
    increment = 5;
    uint32_t original = value;
    expected = value + increment;
    uint32_t result = 0;
    
    __asm__ __volatile__ (
        "lock xadd %[result], %[val]"
        : [result] "+r" (result), [val] "+m" (value)
        : 
        : "memory"
    );
    
    if (value != expected) {
        printf("Error: LOCK XADD value expected %u, got %u\n", expected, value);
        errors++;
    }
    if (result != original) {
        printf("Error: LOCK XADD result expected %u, got %u\n", original, result);
        errors++;
    }
    if (value == expected && result == original) {
        printf("LOCK XADD test: PASS\n");
    }
    
    printf("\nLOCK test completed\n");
    printf("===================\n");
    printf("Total tests: 2\n");
    printf("Passed: %d\n", 2 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
