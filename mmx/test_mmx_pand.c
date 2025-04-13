#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试内存操作数
void test_memory_operand() {
    uint64_t a __attribute__((aligned(8))) = 0x0123456789ABCDEF;
    uint64_t b __attribute__((aligned(8))) = 0xFEDCBA9876543210;
    uint64_t expected = 0x0000000000000000;
    uint64_t result = 0;
    
    asm volatile (
        "movq %[a], %%mm0\n\t"
        "pand %[b], %%mm0\n\t"
        "movq %%mm0, %[result]\n\t"
        "emms"
        : [result] "=m" (result)
        : [a] "m" (a),
          [b] "m" (b)
        : "mm0"
    );
    
    printf("\n=== Memory Operand Test ===\n");
    printf("  0x%016lX & 0x%016lX = 0x%016lX (expected 0x%016lX) - %s\n",
           a, b, result, expected,
           result == expected ? "PASS" : "FAIL");
}

void test_pand() {
    struct TestCase {
        uint64_t a;
        uint64_t b;
        uint64_t expected;
    } test_cases[] = {
        {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}, // 全1
        {0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0x0000000000000000}, // 全0
        {0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0x0000000000000000}, // 交替1/0
        {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x1214121812141210}  // 特定值
    };

    printf("=== Testing PAND instruction ===\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        uint64_t result = 0;
        
        asm volatile (
            "movq %[a], %%mm0\n\t"
            "movq %[b], %%mm1\n\t"
            "pand %%mm1, %%mm0\n\t"
            "movq %%mm0, %[result]\n\t"
            "emms"
            : [result] "=m" (result)
            : [a] "m" (test_cases[i].a),
              [b] "m" (test_cases[i].b)
            : "mm0", "mm1"
        );
        
        printf("Test %zu:\n", i+1);
        printf("  0x%016lX & 0x%016lX = 0x%016lX (expected 0x%016lX) - %s\n",
               test_cases[i].a, test_cases[i].b, result, test_cases[i].expected,
               result == test_cases[i].expected ? "PASS" : "FAIL");
    }
}

int main() {
    test_pand();
    test_memory_operand();
    return 0;
}
