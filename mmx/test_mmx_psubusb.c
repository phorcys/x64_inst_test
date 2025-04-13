#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试内存操作数
void test_memory_operand() {
    uint8_t a[8] __attribute__((aligned(8))) = {0xFF, 0x80, 0x7F, 0x00, 0x01, 0xFE, 0x81, 0x7E};
    uint8_t b[8] __attribute__((aligned(8))) = {0x00, 0x7F, 0x80, 0xFF, 0xFE, 0x01, 0x7F, 0x81};
    uint8_t expected[8] = {0xFF, 0x01, 0x00, 0x00, 0x00, 0xFD, 0x02, 0x00};
    uint8_t result[8] = {0};
    
    asm volatile (
        "movq %[a], %%mm0\n\t"
        "psubusb %[b], %%mm0\n\t"
        "movq %%mm0, %[result]\n\t"
        "emms"
        : [result] "=m" (result)
        : [a] "m" (*a),
          [b] "m" (*b)
        : "mm0"
    );
    
    printf("\n=== Memory Operand Test ===\n");
    for (size_t i = 0; i < 8; i++) {
        printf("  [%zu] 0x%02X - 0x%02X = 0x%02X (expected 0x%02X) - %s\n",
               i, a[i], b[i], result[i], expected[i],
               result[i] == expected[i] ? "PASS" : "FAIL");
    }
}

void test_psubusb() {
    struct TestCase {
        uint8_t a[8];
        uint8_t b[8];
        uint8_t expected[8];
    } test_cases[] = {
        {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
         {0x00, 0x01, 0x7F, 0x80, 0xFE, 0xFF, 0x00, 0x00},
         {0xFF, 0xFE, 0x80, 0x7F, 0x01, 0x00, 0xFF, 0xFF}},  // 各种减法情况
        {{0x00, 0x01, 0x7F, 0x80, 0xFE, 0xFF, 0x00, 0x00},
         {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00},
         {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  // 饱和到0的情况
        {{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
         {0x7F, 0x80, 0x81, 0xFF, 0x00, 0x01, 0x7F, 0x80},
         {0x01, 0x00, 0x00, 0x00, 0x80, 0x7F, 0x01, 0x00}}   // 边界值测试
    };

    printf("=== Testing PSUBUSB instruction ===\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        uint8_t result[8] = {0};
        
        asm volatile (
            "movq %[a], %%mm0\n\t"
            "movq %[b], %%mm1\n\t"
            "psubusb %%mm1, %%mm0\n\t"
            "movq %%mm0, %[result]\n\t"
            "emms"
            : [result] "=m" (result)
            : [a] "m" (test_cases[i].a),
              [b] "m" (test_cases[i].b)
            : "mm0", "mm1"
        );
        
        printf("Test %zu:\n", i+1);
        for (size_t j = 0; j < 8; j++) {
            printf("  [%zu] 0x%02X - 0x%02X = 0x%02X (expected 0x%02X) - %s\n",
                   j, test_cases[i].a[j], test_cases[i].b[j], 
                   result[j], test_cases[i].expected[j],
                   result[j] == test_cases[i].expected[j] ? "PASS" : "FAIL");
        }
    }
}

int main() {
    test_psubusb();
    test_memory_operand();
    return 0;
}
