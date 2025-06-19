#include <stdio.h>
#include <stdint.h>
#include "avx.h"

// 打印64位整数数组
static void print_qwords(int64_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%ld ", value[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDQ instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试
    {
        int64_t a[2] = {10000000000, 20000000000};
        int64_t b[2] = {5000000000, 10000000000};
        int64_t result[2] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("Test 1: Basic functionality\n");
        print_qwords(a, 2, "Operand A");
        print_qwords(b, 2, "Operand B");
        print_qwords(result, 2, "Result");
        
        int64_t expected[2] = {15000000000, 30000000000};
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (result[i] != expected[i]) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_qwords(expected, 2, "Expected");
        }
    }
    
    // 测试2: 边界值测试（包括溢出）
    {
        int64_t a[2] = {9223372036854775807LL, -9223372036854775808LL};
        int64_t b[2] = {1, -1};
        int64_t result[2] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("\nTest 2: Boundary values (with overflow)\n");
        print_qwords(a, 2, "Operand A");
        print_qwords(b, 2, "Operand B");
        print_qwords(result, 2, "Result");
        
        // 预期结果：溢出后回绕
        int64_t expected[2] = {
            -9223372036854775808LL,   // 9223372036854775807+1 溢出
            9223372036854775807LL     // -9223372036854775808-1 溢出
        };
        
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (result[i] != expected[i]) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_qwords(expected, 2, "Expected");
        }
    }
    
    // 测试3: 全零测试
    {
        int64_t a[2] = {0};
        int64_t b[2] = {0};
        int64_t result[2] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("\nTest 3: All zeros\n");
        print_qwords(a, 2, "Operand A");
        print_qwords(b, 2, "Operand B");
        print_qwords(result, 2, "Result");
        
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (result[i] != 0) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: 0 0\n");
        }
    }
    
    // 测试4: 内存操作数
    {
        int64_t a_mem[2] = {10000000000, 20000000000};
        int64_t b[2] = {5000000000, 10000000000};
        int64_t result[2] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddq %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a_mem), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("\nTest 4: Memory operand\n");
        print_qwords(a_mem, 2, "Operand A (Memory)");
        print_qwords(b, 2, "Operand B");
        print_qwords(result, 2, "Result");
        
        int64_t expected[2] = {15000000000, 30000000000};
        int pass = 1;
        for (int i = 0; i < 2; i++) {
            if (result[i] != expected[i]) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_qwords(expected, 2, "Expected");
        }
    }
    
    // 测试5: 256位AVX测试
    {
        int64_t a[4] = {
            10000000000, 20000000000,
            -10000000000, -20000000000};
        int64_t b[4] = {
            5000000000, 10000000000,
            -5000000000, -10000000000};
        int64_t result[4] = {0};
        
        asm volatile(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpaddq %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "ymm0", "ymm1", "ymm2", "memory"
        );
        
        printf("\nTest 5: 256-bit AVX\n");
        print_qwords(a, 4, "Operand A");
        print_qwords(b, 4, "Operand B");
        print_qwords(result, 4, "Result");
        
        int64_t expected[4] = {
            15000000000, 30000000000,
            -15000000000, -30000000000};
        
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (result[i] != expected[i]) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_qwords(expected, 4, "Expected");
        }
    }
    
    printf("\nVPADDQ tests completed.\n");
    return 0;
}
