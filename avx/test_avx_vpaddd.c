#include <stdio.h>
#include <stdint.h>
#include "avx.h"

void print_dwords(int32_t *value, int count, const char* name) {
    printf("%s: ", name);
    for (int i = 0; i < count; i++) {
        printf("%d ", value[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VPADDD instruction\n");
    printf("=========================\n");
    
    // 测试1: 基本功能测试
    {
        int32_t a[4] = {10, 20, 30, 40};
        int32_t b[4] = {5, 10, 15, 20};
        int32_t result[4] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("Test 1: Basic functionality\n");
        print_dwords(a, 4, "Operand A");
        print_dwords(b, 4, "Operand B");
        print_dwords(result, 4, "Result");
        
        int32_t expected[4] = {15, 30, 45, 60};
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
            print_dwords(expected, 4, "Expected");
        }
    }
    
    // 测试2: 边界值测试（包括溢出）
    {
        int32_t a[4] = {2147483647, 2147483647, -2147483648, -2147483648};
        int32_t b[4] = {1, -1, 1, -1};
        int32_t result[4] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("\nTest 2: Boundary values (with overflow)\n");
        print_dwords(a, 4, "Operand A");
        print_dwords(b, 4, "Operand B");
        print_dwords(result, 4, "Result");
        
        // 预期结果：溢出后回绕
        int32_t expected[4] = {
            -2147483648,   // 2147483647+1 溢出
            2147483646,    // 2147483647-1
            -2147483647,   // -2147483648+1
            2147483647     // -2147483648-1 溢出
        };
        
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
            print_dwords(expected, 4, "Expected");
        }
    }
    
    // 测试3: 全零测试
    {
        int32_t a[4] = {0};
        int32_t b[4] = {0};
        int32_t result[4] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("\nTest 3: All zeros\n");
        print_dwords(a, 4, "Operand A");
        print_dwords(b, 4, "Operand B");
        print_dwords(result, 4, "Result");
        
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (result[i] != 0) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            printf("Expected: 0 0 0 0\n");
        }
    }
    
    // 测试4: 内存操作数
    {
        int32_t a_mem[4] = {100, 200, 300, 400};
        int32_t b[4] = {50, 100, 150, 200};
        int32_t result[4] = {0};
        
        asm volatile(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpaddd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a_mem), "m" (*b)
            : "xmm0", "xmm1", "xmm2", "memory"
        );
        
        printf("\nTest 4: Memory operand\n");
        print_dwords(a_mem, 4, "Operand A (Memory)");
        print_dwords(b, 4, "Operand B");
        print_dwords(result, 4, "Result");
        
        int32_t expected[4] = {150, 300, 450, 600};
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
            print_dwords(expected, 4, "Expected");
        }
    }
    
    // 测试5: 256位AVX测试
    {
        int32_t a[8] = {
            1000000, 2000000, 3000000, 4000000,
            -1000000, -2000000, -3000000, -4000000};
        int32_t b[8] = {
            500000, 1000000, 1500000, 2000000,
            -500000, -1000000, -1500000, -2000000};
        int32_t result[8] = {0};
        
        asm volatile(
            "vmovdqu %1, %%ymm0\n\t"
            "vmovdqu %2, %%ymm1\n\t"
            "vpaddd %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovdqu %%ymm2, %0\n\t"
            : "=m" (*result)
            : "m" (*a), "m" (*b)
            : "ymm0", "ymm1", "ymm2", "memory"
        );
        
        printf("\nTest 5: 256-bit AVX\n");
        print_dwords(a, 8, "Operand A");
        print_dwords(b, 8, "Operand B");
        print_dwords(result, 8, "Result");
        
        int32_t expected[8] = {
            1500000, 3000000, 4500000, 6000000,
            -1500000, -3000000, -4500000, -6000000};
        
        int pass = 1;
        for (int i = 0; i < 8; i++) {
            if (result[i] != expected[i]) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_dwords(expected, 8, "Expected");
        }
    }
    
    printf("\nVPADDD tests completed.\n");
    return 0;
}
