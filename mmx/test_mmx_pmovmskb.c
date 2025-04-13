#include <stdio.h>
#include <stdint.h>
#include <string.h>

void test_pmovmskb() {
    printf("Testing PMOVMSKB (Gd, Em) instruction\n");
    
    // Test 1: Basic test with alternating bits
    {
        uint8_t data[8] = {0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00};
        uint32_t result = 0;
        
        printf("Test 1: PMOVMSKB with alternating bits\n");
        asm volatile (
            "movq %1, %%mm0;"
            "pmovmskb %%mm0, %0"
            : "=r" (result)
            : "m" (data)
            : "%mm0"
        );
        
        uint32_t expected = 0b01010101;
        printf("Data:     ");
        for (int i = 0; i < 8; i++) printf("%02x ", data[i]);
        printf("\n");
        printf("Expected: 0x%02x\n", expected);
        printf("Got:      0x%02x\n", result);
        printf("Test %s\n", (result == expected) ? "PASSED" : "FAILED");
    }
    
    // Test 2: All bits set
    {
        uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        uint32_t result = 0;
        
        printf("Test 2: PMOVMSKB with all bits set\n");
        asm volatile (
            "movq %1, %%mm0;"
            "pmovmskb %%mm0, %0"
            : "=r" (result)
            : "m" (data)
            : "%mm0"
        );
        
        uint32_t expected = 0xFF;
        printf("Data:     ");
        for (int i = 0; i < 8; i++) printf("%02x ", data[i]);
        printf("\n");
        printf("Expected: 0x%02x\n", expected);
        printf("Got:      0x%02x\n", result);
        printf("Test %s\n", (result == expected) ? "PASSED" : "FAILED");
    }
    
    // Test 3: Random pattern
    {
        uint8_t data[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
        uint32_t result = 0;
        
        printf("Test 3: PMOVMSKB with random pattern\n");
        asm volatile (
            "movq %1, %%mm0;"
            "pmovmskb %%mm0, %0"
            : "=r" (result)
            : "m" (data)
            : "%mm0"
        );
        
        uint32_t expected = 0;
        for (int i = 0; i < 8; i++) {
            expected |= ((data[i] >> 7) & 1) << i;
        }
        printf("Data:     ");
        for (int i = 0; i < 8; i++) printf("%02x ", data[i]);
        printf("\n");
        printf("Expected: 0x%02x\n", expected);
        printf("Got:      0x%02x\n", result);
        printf("Test %s\n", (result == expected) ? "PASSED" : "FAILED");
    }
}

int main() {
    test_pmovmskb();
    return 0;
}
