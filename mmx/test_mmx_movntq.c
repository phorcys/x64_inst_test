#include <stdio.h>
#include <stdint.h>
#include <string.h>

void test_movntq() {
    printf("Testing MOVNTQ (Em, Gm) instruction\
\
");
    
    // Align memory to 16-byte boundary for better performance
    uint64_t dest __attribute__((aligned(16))) = 0;
    uint64_t src __attribute__((aligned(16))) = 0x123456789ABCDEF0;
    
    // Test 1: Basic MOVNTQ operation
    printf("Test 1: Basic MOVNTQ operation\
");
    printf("Source: 0x%016lx\
", src);
    
    asm volatile (
        "movq %1, %%mm0;"
        "movntq %%mm0, %0"
        : "=m" (dest)
        : "m" (src)
        : "%mm0"
    );
    
    printf("Dest:   0x%016lx\
", dest);
    printf("Test %s\
\
", (dest == src) ? "PASSED" : "FAILED");
    
    // Test 2: Multiple writes with different values
    uint64_t values[] = {
        0x0000000000000000,
        0xFFFFFFFFFFFFFFFF, 
        0x5555555555555555,
        0xAAAAAAAAAAAAAAAA
    };
    
    for (int i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        src = values[i];
        dest = ~src; // Set dest to opposite value
        
        printf("Test 2.%d: MOVNTQ with value 0x%016lx\
", i+1, src);
        asm volatile (
            "movq %1, %%mm0;"
            "movntq %%mm0, %0"
            : "=m" (dest)
            : "m" (src)
            : "%mm0"
        );
        
        printf("Expected: 0x%016lx\
", src);
        printf("Got:      0x%016lx\
", dest);
        printf("Test %s\
\
", (dest == src) ? "PASSED" : "FAILED");
    }
    
    // Test 3: Verify non-temporal behavior (can't directly test, but can verify correctness)
    {
        uint64_t buffer[2] __attribute__((aligned(16))) = {0, 0};
        src = 0xDEADBEEFCAFEBABE;
        
        printf("Test 3: Verify MOVNTQ correctness\
");
        asm volatile (
            "movq %1, %%mm0;"
            "movntq %%mm0, %0"
            : "=m" (buffer[0])
            : "m" (src)
            : "%mm0"
        );
        
        printf("Expected: 0x%016lx\
", src);
        printf("Got:      0x%016lx\
", buffer[0]);
        printf("Test %s\
\
", (buffer[0] == src) ? "PASSED" : "FAILED");
    }
}

int main() {
    test_movntq();
    return 0;
}
