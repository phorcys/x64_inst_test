#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试内存操作数
void test_memory_operand() {
    uint16_t a[4] __attribute__((aligned(8))) = {0xFFFF, 0x8000, 0x7FFF, 0x0000};
    uint16_t b[4] __attribute__((aligned(8))) = {0x0000, 0x7FFF, 0x8000, 0xFFFF};
    uint16_t expected[4] = {0xFFFF, 0x0001, 0x0000, 0x0000};
    uint16_t result[4] = {0};
    
    asm volatile (
        "movq %[a], %%mm0\n\t"
        "psubusw %[b], %%mm0\n\t"
        "movq %%mm0, %[result]\n\t"
        "emms"
        : [result] "=m" (result)
        : [a] "m" (*a),
          [b] "m" (*b)
        : "mm0"
    );
    
    printf("\n=== Memory Operand Test ===\n");
    for (size_t i = 0; i < 4; i++) {
        printf("  [%zu] 0x%04X - 0x%04X = 0x%04X (expected 0x%04X) - %s\n",
               i, a[i], b[i], result[i], expected[i],
               result[i] == expected[i] ? "PASS" : "FAIL");
    }
}

void test_psubusw() {
    struct TestCase {
        uint16_t a[4];
        uint16_t b[4];
        uint16_t expected[4];
    } test_cases[] = {
        {{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
         {0x0000, 0x0001, 0x7FFF, 0x8000},
         {0xFFFF, 0xFFFE, 0x8000, 0x7FFF}},  // 各种减法情况
        {{0x0000, 0x0001, 0x7FFF, 0x8000},
         {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
         {0x0000, 0x0000, 0x0000, 0x0000}},  // 饱和到0的情况
        {{0x8000, 0x8000, 0x8000, 0x8000},
         {0x7FFF, 0x8000, 0x8001, 0xFFFF},
         {0x0001, 0x0000, 0x0000, 0x0000}}   // 边界值测试
    };

    printf("=== Testing PSUBUSW instruction ===\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        uint16_t result[4] = {0};
        
        asm volatile (
            "movq %[a], %%mm0\n\t"
            "movq %[b], %%mm1\n\t"
            "psubusw %%mm1, %%mm0\n\t"
            "movq %%mm0, %[result]\n\t"
            "emms"
            : [result] "=m" (result)
            : [a] "m" (test_cases[i].a),
              [b] "m" (test_cases[i].b)
            : "mm0", "mm1"
        );
        
        printf("Test %zu:\n", i+1);
        for (size_t j = 0; j < 4; j++) {
            printf("  [%zu] 0x%04X - 0x%04X = 0x%04X (expected 0x%04X) - %s\n",
                   j, test_cases[i].a[j], test_cases[i].b[j], 
                   result[j], test_cases[i].expected[j],
                   result[j] == test_cases[i].expected[j] ? "PASS" : "FAIL");
        }
    }
}

int main() {
    test_psubusw();
    test_memory_operand();
    return 0;
}
