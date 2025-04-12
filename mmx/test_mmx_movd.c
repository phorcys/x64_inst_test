#include <stdio.h>
#include <stdint.h>
#include <string.h>

void test_movd() {
    printf("Testing MOVD (Gm, Ed) instruction\n\n");
    
    // Test 1: 32-bit integer to MMX register
    {
        uint32_t val = 0x12345678;
        uint64_t mm_val = 0;
        
        printf("Test 1: MOVD mm0, [val] (0x%08x)\n", val);
        asm volatile (
            "movd %1, %%mm0;"
            "movd %%mm0, %0"
            : "=r" (mm_val)
            : "m" (val)
            : "%mm0"
        );
        
        uint32_t result = (uint32_t)mm_val;
        printf("Expected: 0x%08x\n", val);
        printf("Got:      0x%08x\n", result);
        printf("Test %s\n\n", (result == val) ? "PASSED" : "FAILED");
    }
    
    // Test 2: MMX register to memory
    {
        uint32_t val = 0x87654321;
        uint32_t dest = 0;
        
        printf("Test 2: MOVD [dest], mm0 (0x%08x)\n", val);
        asm volatile (
            "movd %1, %%mm0;"
            "movd %%mm0, %0"
            : "=m" (dest)
            : "m" (val)
            : "%mm0"
        );
        
        printf("Expected: 0x%08x\n", val);
        printf("Got:      0x%08x\n", dest);
        printf("Test %s\n\n", (dest == val) ? "PASSED" : "FAILED");
    }
    
    // Test 3: Boundary values
    {
        uint32_t tests[] = {0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF};
        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
            uint32_t val = tests[i];
            uint64_t mm_val = 0;
            
            printf("Test 3.%d: MOVD boundary value 0x%08x\n", i+1, val);
            asm volatile (
                "movd %1, %%mm0;"
                "movd %%mm0, %0"
                : "=r" (mm_val)
                : "m" (val)
                : "%mm0"
            );
            
            uint32_t result = (uint32_t)mm_val;
            printf("Expected: 0x%08x\n", val);
            printf("Got:      0x%08x\n", result);
            printf("Test %s\n\n", (result == val) ? "PASSED" : "FAILED");
        }
    }
}

int main() {
    test_movd();
    return 0;
}
