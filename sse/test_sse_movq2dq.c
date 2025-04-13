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

void test_movq2dq_reg() {
    printf("\n=== Testing MOVQ2DQ (Register Operands) ===\n");
    
    for (int i = 0; i < NUM_TESTS; i++) {
        TestValue input = test_values[i];
        __m128i xmm_out;
        uint64_t mm_val;

        // 加载测试值到MMX寄存器
        asm volatile (
            "movq   %1, %%mm0\n\t"
            "movq2dq %%mm0, %0\n\t"
            "movq   %%mm0, %2\n\t"
            "emms\n\t"
            : "=x"(xmm_out), "=m"(input), "=m"(mm_val)
            : "m"(input)
        );

        // 提取结果
        uint64_t result;
        memcpy(&result, &xmm_out, sizeof(result));
        
        printf("\nTest %d:\n", i+1);
        printf("Input MMX:   0x%016lX\n", input.u64);
        printf("Expected XMM:0x%016lX (low 64-bit)\n", input.u64);
        printf("Actual XMM:  0x%016lX\n", result);
        printf("MMX Check:   %s\n", (mm_val == input.u64) ? "PASS" : "FAIL");
        printf("XMM Check:   %s\n", (result == input.u64) ? "PASS" : "FAIL");
        printf("Binary:\n");
        print_binary(input.u64); printf(" (input)\n");
        print_binary(result);    printf(" (result)\n");
    }
}

void test_movq2dq_mem() {
    printf("\n=== Testing MOVQ2DQ (Memory Operands) ===\n");
    
    for (int i = 0; i < NUM_TESTS; i++) {
        TestValue input = test_values[i];
        __m128i xmm_out;

        asm volatile (
            "movq   %1, %%mm0\n\t"
            "movq2dq %%mm0, %0\n\t"
            "emms\n\t"
            : "=x"(xmm_out)
            : "m"(input.f64)  // 使用double类型进行内存访问
        );

        uint64_t result;
        memcpy(&result, &xmm_out, sizeof(result));
        
        printf("\nTest %d:\n", i+1);
        printf("Input Mem:   0x%016lX\n", input.u64);
        printf("Expected XMM:0x%016lX\n", input.u64);
        printf("Actual XMM:  0x%016lX\n", result);
        printf("Check:       %s\n", (result == input.u64) ? "PASS" : "FAIL");
    }
}

int main() {
    printf("MOVQ2DQ Instruction Test\n");
    printf("=======================\n");
    
    test_movq2dq_reg();
    test_movq2dq_mem();

    printf("\nAll tests completed.\n");
    return 0;
}
