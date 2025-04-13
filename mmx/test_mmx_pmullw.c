#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试内存操作数
void test_memory_operand() {
    int16_t a[4] __attribute__((aligned(8))) = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
    int16_t b[4] __attribute__((aligned(8))) = {0x1111, 0x2222, 0x3333, 0x4444};
    int16_t expected[4] = {0xA974, 0x6BF0, 0x4774, 0xF7C0};
    int16_t result[4] = {0};
    
    asm volatile (
        "movq %[a], %%mm0\n\t"
        "pmullw %[b], %%mm0\n\t"
        "movq %%mm0, %[result]\n\t"
        "emms"
        : [result] "=m" (result)
        : [a] "m" (*a),
          [b] "m" (*b)
        : "mm0"
    );
    
    printf("\n=== Memory Operand Test ===\n");
    for (size_t i = 0; i < 4; i++) {
        printf("  [%zu] 0x%04X * 0x%04X = 0x%04X (expected 0x%04X) - %s\n",
               i, a[i], b[i], result[i], expected[i],
               result[i] == expected[i] ? "PASS" : "FAIL");
    }
}

// 测试寄存器与内存混合操作
void test_mixed_operand() {
    int16_t a[4] = {0x7FFF, 0x8000, 0x0000, 0xFFFF};
    int16_t b[4] = {0x0001, 0xFFFF, 0x7FFF, 0x8000};
    int16_t expected[4] = {0x7FFF, 0x8000, 0x0000, 0x8000};
    int16_t result[4] = {0};
    
    asm volatile (
        "movq %[a], %%mm0\n\t"
        "movq %[b], %%mm1\n\t"
        "pmullw %%mm1, %%mm0\n\t"
        "movq %%mm0, %[result]\n\t"
        "emms"
        : [result] "=m" (result)
        : [a] "m" (*a),
          [b] "m" (*b)
        : "mm0", "mm1"
    );
    
    printf("\n=== Mixed Operand Test ===\n");
    for (size_t i = 0; i < 4; i++) {
        printf("  [%zu] 0x%04X * 0x%04X = 0x%04X (expected 0x%04X) - %s\n",
               i, a[i], b[i], result[i], expected[i],
               result[i] == expected[i] ? "PASS" : "FAIL");
    }
}

void test_pmullw() {
    struct TestCase {
        int16_t a[4];
        int16_t b[4];
        int16_t expected[4];
    } test_cases[] = {
        {{0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF}, 
         {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
         {0x0001, 0x0001, 0x0001, 0x0001}},  // 最大正数相乘
        {{0x8000, 0x8000, 0x8000, 0x8000},
         {0x8000, 0x8000, 0x8000, 0x8000},
         {0x0000, 0x0000, 0x0000, 0x0000}},  // 最小负数相乘
        {{0x7FFF, 0x8000, 0x0001, 0xFFFF},
         {0x0001, 0xFFFF, 0x7FFF, 0x8000},
         {0x7FFF, 0x8000, 0x7FFF, 0x8000}},  // 各种组合
        {{0x0000, 0x0000, 0x0000, 0x0000},
         {0x7FFF, 0x8000, 0x0001, 0xFFFF},
         {0x0000, 0x0000, 0x0000, 0x0000}}   // 零值测试
    };

    printf("=== Testing PMULLW instruction ===\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        int16_t result[4] = {0};
        
        asm volatile (
            "movq %[a], %%mm0\n\t"
            "movq %[b], %%mm1\n\t"
            "pmullw %%mm1, %%mm0\n\t"
            "movq %%mm0, %[result]\n\t"
            "emms"
            : [result] "=m" (result)
            : [a] "m" (test_cases[i].a),
              [b] "m" (test_cases[i].b)
            : "mm0", "mm1"
        );
        
        printf("Test %zu:\n", i+1);
        for (size_t j = 0; j < 4; j++) {
            printf("  [%zu] 0x%04X * 0x%04X = 0x%04X (expected 0x%04X) - %s\n",
                   j, test_cases[i].a[j], test_cases[i].b[j], 
                   result[j], test_cases[i].expected[j],
                   result[j] == test_cases[i].expected[j] ? "PASS" : "FAIL");
        }
    }
}

int main() {
    test_pmullw();
    test_memory_operand();
    test_mixed_operand();
    return 0;
}
