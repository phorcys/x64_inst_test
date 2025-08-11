#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Function to get the current CS segment selector
static uint16_t get_cs(void) {
    uint16_t seg;
    asm volatile ("mov %%cs, %0" : "=r" (seg));
    return seg;
}

// Function to get the current SS segment selector
static uint16_t get_ss(void) {
    uint16_t seg;
    asm volatile ("mov %%ss, %0" : "=r" (seg));
    return seg;
}

int main() {
    int errors = 0;
    
    printf("Starting LAR tests\n");
    printf("==================\n\n");
    
    uint16_t valid_selectors[] = {
        get_cs(),  // Code segment
        get_ss(),  // Stack segment
        0x33       // Common user segment in Linux
    };
    const char* selector_names[] = {"CS", "SS", "User Segment"};
    int num_valid = sizeof(valid_selectors) / sizeof(valid_selectors[0]);
    
    uint16_t invalid_selectors[] = {
        0x0000,    // Null selector
        0xFFF8,    // Likely invalid selector
        0x1234     // Random selector
    };
    int num_invalid = sizeof(invalid_selectors) / sizeof(invalid_selectors[0]);
    
    // Test valid selectors with 16-bit destination
    for (int i = 0; i < num_valid; i++) {
        uint16_t selector = valid_selectors[i];
        uint16_t access_rights;
        uint8_t zf;
        
        __asm__ __volatile__(
            "lar %[sel], %[ar]\n\t"
            "setz %[zf]"
            : [ar] "=r" (access_rights), [zf] "=r" (zf)
            : [sel] "r" (selector)
            : "cc"
        );
        
        printf("Test %d: Valid selector %s (0x%04X)\n", i+1, selector_names[i], selector);
        printf("  ZF: %d\n", zf);
        printf("  Access Rights: 0x%04X\n", access_rights);
        
        // Verify ZF is set (1) for valid selector
        if (zf != 1) {
            printf("  Error: ZF not set for valid selector!\n");
            errors++;
        }
        
        // Verify P (Present) bit is set (bit 15)
        if (!(access_rights & 0x8000)) {
            printf("  Error: P bit not set!\n");
            errors++;
        }
        
        // Verify S (System) bit is set (bit 12)
        if (!(access_rights & 0x1000)) {
            printf("  Error: S bit not set!\n");
            errors++;
        }
        
        printf("\n");
    }
    
    // Test invalid selectors with 16-bit destination
    for (int i = 0; i < num_invalid; i++) {
        uint16_t selector = invalid_selectors[i];
        uint16_t access_rights = 0xFFFF; // Initialize with non-zero value
        uint8_t zf;
        
        __asm__ __volatile__(
            "lar %[sel], %[ar]\n\t"
            "setz %[zf]"
            : [ar] "=r" (access_rights), [zf] "=r" (zf)
            : [sel] "r" (selector)
            : "cc"
        );
        
        printf("Test %d: Invalid selector 0x%04X\n", num_valid + i + 1, selector);
        printf("  ZF: %d\n", zf);
        printf("  Access Rights: 0x%04X\n", access_rights);
        
        // Verify ZF is not set (0) for invalid selector
        if (zf != 0) {
            printf("  Error: ZF set for invalid selector!\n");
            errors++;
        }
        
        printf("\n");
    }
    
    // Test valid selectors with 32-bit destination
    for (int i = 0; i < num_valid; i++) {
        uint16_t selector = valid_selectors[i];
        uint32_t access_rights;
        uint8_t zf;
        
        __asm__ __volatile__(
            "lar %[sel], %[ar]\n\t"
            "setz %[zf]"
            : [ar] "=r" (access_rights), [zf] "=r" (zf)
            : [sel] "r" (selector)
            : "cc"
        );
        
        printf("Test %d: Valid selector %s (0x%04X) with 32-bit destination\n", 
               num_valid + num_invalid + i + 1, selector_names[i], selector);
        printf("  ZF: %d\n", zf);
        printf("  Access Rights: 0x%08X\n", access_rights);
        
        // Verify ZF is set (1) for valid selector
        if (zf != 1) {
            printf("  Error: ZF not set for valid selector!\n");
            errors++;
        }
        
        // Verify P and S bits are set
        if (!(access_rights & 0x8000) || !(access_rights & 0x1000)) {
            printf("  Error: P or S bit not set in lower 16 bits!\n");
            errors++;
        }
        
        printf("\n");
    }
    
    printf("\nLAR tests completed\n");
    printf("====================\n");
    printf("Total tests: %d\n", num_valid * 2 + num_invalid);
    printf("Passed: %d\n", num_valid * 2 + num_invalid - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
