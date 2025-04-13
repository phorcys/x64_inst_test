#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试内存操作数
void test_memory_operand() {
    uint64_t src __attribute__((aligned(8))) = 0xFEDCBA9876543210;
    uint64_t dst __attribute__((aligned(8))) = 0x0123456789ABCDEF;
    uint64_t expected = 0xFEDCBA9876543210; // NOT(dst) AND src
    uint64_t result = 0;
    
    asm volatile (
        "movq %[dst], %%mm0\n\t"
        "pandn %[src], %%mm0\n\t"  // mm0 = NOT(mm0) AND src
        "movq %%mm0, %[result]\n\t"
        "emms"
        : [result] "=m" (result)
        : [dst] "m" (dst),
          [src] "m" (src)
        : "mm0"
    );
    
    printf("\n=== Memory Operand Test ===\n");
    printf("  NOT(0x%016lX) & 0x%016lX = 0x%016lX (expected 0x%016lX) - %s\n",
           dst, src, result, expected,
           result == expected ? "PASS" : "FAIL");
}

void test_pandn() {
    struct TestCase {
        uint64_t dst;  // 第一个操作数(被NOT)
        uint64_t src;  // 第二个操作数
        uint64_t expected;
    } test_cases[] = {
        {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000}, // NOT(全1) AND 全1 = 0
        {0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0x0000000000000000}, // NOT(全1) AND 0 = 0
        {0x5555555555555555, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA}, // NOT(0101) AND 1010 = 1010
        {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0xECC8A88064402000}  // 特定值测试
    };

    printf("=== Testing PANDN instruction ===\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        uint64_t result = 0;
        
        asm volatile (
            "movq %[dst], %%mm0\n\t"
            "movq %[src], %%mm1\n\t"
            "pandn %%mm1, %%mm0\n\t"  // mm0 = NOT(mm0) AND mm1
            "movq %%mm0, %[result]\n\t"
            "emms"
            : [result] "=m" (result)
            : [dst] "m" (test_cases[i].dst),
              [src] "m" (test_cases[i].src)
            : "mm0", "mm1"
        );
        
        printf("Test %zu:\n", i+1);
        printf("  NOT(0x%016lX) & 0x%016lX = 0x%016lX (expected 0x%016lX) - %s\n",
               test_cases[i].dst, test_cases[i].src, result, test_cases[i].expected,
               result == test_cases[i].expected ? "PASS" : "FAIL");
    }
}

int main() {
    test_pandn();
    test_memory_operand();
    return 0;
}
