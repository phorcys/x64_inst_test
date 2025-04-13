#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

typedef union {
    uint64_t u64;
    int64_t  s64;
    double   f64;
    uint8_t  bytes[8];
} TestValue;

// 测试数据集
const TestValue test_values[] = {
    {.u64 = 0x0000000000000000},  // 全零
    {.u64 = 0xFFFFFFFFFFFFFFFF},  // 全一
    {.u64 = 0x7FFFFFFFFFFFFFFF},  // 最大正数
    {.u64 = 0x8000000000000000},  // 最小负数
    {.u64 = 0x123456789ABCDEF0},  // 模式数据
    {.u64 = 0xA5A5A5A5A5A5A5A5}   // 交替模式
};
const int NUM_TESTS = sizeof(test_values)/sizeof(test_values[0]);

void print_binary(uint64_t value) {
    for (int i = 63; i >= 0; i--) {
        printf("%c", (value & ((uint64_t)1 << i)) ? '1' : '0');
        if (i % 8 == 0 && i != 0) printf(" ");
    }
}

void test_movdq2q_reg() {
    printf("\n=== Testing MOVDQ2Q (Register Operands) ===\n");
    
    for (int i = 0; i < NUM_TESTS; i++) {
        __m128i xmm_in;
        uint64_t mm_out = 0;
        TestValue input = test_values[i];

        // 加载测试值到XMM寄存器
        memcpy(&xmm_in, &input.u64, sizeof(input.u64));
        
        asm volatile (
            "movdq2q %1, %%mm0\n\t"
            "movq   %%mm0, %0\n\t"
            "emms\n\t"
            : "=m"(mm_out)
            : "x"(xmm_in)
        );

        printf("\nTest %d:\n", i+1);
        printf("Input XMM:   0x%016lX\n", input.u64);
        printf("Expected MMX:0x%016lX\n", input.u64);
        printf("Actual MMX:  0x%016lX\n", mm_out);
        printf("Check:       %s\n", (mm_out == input.u64) ? "PASS" : "FAIL");
        printf("Binary:\n");
        print_binary(input.u64); printf(" (input)\n");
        print_binary(mm_out);    printf(" (result)\n");
    }
}

void test_movdq2q_mem() {
    printf("\n=== Testing MOVDQ2Q (Memory Operands) ===\n");
    
    for (int i = 0; i < NUM_TESTS; i++) {
        __m128i xmm_in;
        TestValue output;
        TestValue input = test_values[i];

        memcpy(&xmm_in, &input.u64, sizeof(input.u64));
        
        asm volatile (
            "movdq2q %1, %%mm0\n\t"
            "movq   %%mm0, %0\n\t"
            "emms\n\t"
            : "=m"(output.f64)  // 使用double类型进行内存存储
            : "x"(xmm_in)
        );

        printf("\nTest %d:\n", i+1);
        printf("Input XMM:   0x%016lX\n", input.u64);
        printf("Expected Mem:0x%016lX\n", input.u64);
        printf("Actual Mem:  0x%016lX\n", output.u64);
        printf("Check:       %s\n", (output.u64 == input.u64) ? "PASS" : "FAIL");
    }
}

int main() {
    printf("MOVDQ2Q Instruction Test\n");
    printf("=======================\n");
    
    test_movdq2q_reg();
    test_movdq2q_mem();

    printf("\nAll tests completed.\n");
    return 0;
}
