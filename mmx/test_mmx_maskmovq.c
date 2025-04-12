#include <stdio.h>
#include <stdint.h>
#include <string.h>

void test_maskmovq() {
    printf("Testing MASKMOVQ (Gm, Em) instruction\
\
");
    
    // Align memory to 16-byte boundary
    uint8_t dest[8] __attribute__((aligned(16))) = {0};
    uint8_t src[8] __attribute__((aligned(16))) = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t mask[8] __attribute__((aligned(16))) = {0x80, 0x80, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00};
    
    // Initialize destination with pattern
    memset(dest, 0xAA, sizeof(dest));
    
    // Test 1: Basic MASKMOVQ operation
    printf("Test 1: Basic MASKMOVQ operation\n");
    printf("Source: ");
    for (int i = 0; i < 8; i++) printf("%02x ", src[i]);
    printf("\nMask:   ");
    for (int i = 0; i < 8; i++) printf("%02x ", mask[i]);
    printf("\n");
    
    asm volatile (
        "push %%rdi;"
        "movq %1, %%mm0;"   // Load source
        "movq %2, %%mm1;"   // Load mask
        "lea %0, %%rdi;"    // Load dest address
        "maskmovq %%mm1, %%mm0;"  // Store masked bytes
        "pop %%rdi;"
        "sfence;"
        : 
        : "m" (dest), "m" (src), "m" (mask)
        : "%mm0", "%mm1", "memory"
    );
    
    printf("Dest:   ");
    for (int i = 0; i < 8; i++) printf("%02x ", dest[i]);
    printf("\n");
    
    // Verify expected result
    int passed = 1;
    for (int i = 0; i < 8; i++) {
        uint8_t expected = (mask[i] & 0x80) ? src[i] : 0xAA;
        if (dest[i] != expected) {
            passed = 0;
            break;
        }
    }
    printf("Test %s\n", passed ? "PASSED" : "FAILED");
    
    // Test 2: Different mask patterns
    uint8_t masks[][8] = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // All bits set
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // No bits set
        {0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00}  // Alternating
    };
    
    for (int t = 0; t < sizeof(masks)/sizeof(masks[0]); t++) {
        memcpy(mask, masks[t], sizeof(mask));
        memset(dest, 0xAA, sizeof(dest));
        
        printf("Test 2.%d: MASKMOVQ with mask pattern\n", t+1);
        printf("Source: ");
        for (int i = 0; i < 8; i++) printf("%02x ", src[i]);
        printf("\nMask:   ");
        for (int i = 0; i < 8; i++) printf("%02x ", mask[i]);
        printf("\n");
        
        asm volatile (
            "push %%rdi;"
            "movq %1, %%mm0;"
            "movq %2, %%mm1;"
            "lea %0, %%rdi;"
            "maskmovq %%mm1, %%mm0;"
            "pop %%rdi;"
            "sfence;"
            : 
            : "m" (dest), "m" (src), "m" (mask)
            : "%mm0", "%mm1", "memory"
        );
        
        printf("Dest:   ");
        for (int i = 0; i < 8; i++) printf("%02x ", dest[i]);
        printf("\n");
        
        passed = 1;
        for (int i = 0; i < 8; i++) {
            uint8_t expected = (mask[i] & 0x80) ? src[i] : 0xAA;
            if (dest[i] != expected) {
                passed = 0;
                break;
            }
        }
        printf("Test %s\n", passed ? "PASSED" : "FAILED");
    }
}

int main() {
    test_maskmovq();
    return 0;
}
