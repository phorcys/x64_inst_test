#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 测试VMOVDDUP指令
int test_vmovddup() {
    int ret = 0;
    
    // 测试128位版本
    {
        printf("=== Testing VMOVDDUP (128-bit) ===\n");
        
        // 寄存器-寄存器测试
        {
            __m128d a = _mm_setr_pd(1.0, 2.0);
            __m128d result;
            
            // VMOVDDUP xmm1, xmm2
            __asm__ volatile (
                "vmovddup %1, %0"
                : "=x"(result)
                : "x"(a)
            );
            
            __m128d expected = _mm_setr_pd(1.0, 1.0);
            
            printf("Testing register-register operation\n");
            print_xmmd("a", a);
            print_xmmd("result", result);
            print_xmmd("expected", expected);
            
            if (!cmp_xmmd(result, expected)) {
                printf("Mismatch in register-register test\n");
                ret = 1;
            }
        }
        
        // 寄存器-内存测试
        {
            ALIGNED(16) double mem[2] = {3.0, 4.0};
            __m128d result;
            
            // VMOVDDUP xmm1, [mem]
            __asm__ volatile (
                "vmovddup %1, %0"
                : "=x"(result)
                : "m"(*mem)
            );
            
            __m128d expected = _mm_setr_pd(3.0, 3.0);
            
            printf("\nTesting register-memory operation\n");
            printf("Memory operand: %f %f\n", mem[0], mem[1]);
            print_xmmd("result", result);
            print_xmmd("expected", expected);
            
            if (!cmp_xmmd(result, expected)) {
                printf("Mismatch in register-memory test\n");
                ret = 1;
            }
        }
    }
    
    // 测试256位版本
    {
        printf("\n=== Testing VMOVDDUP (256-bit) ===\n");
        
        // 寄存器-寄存器测试
        {
            __m256d a = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
            __m256d result;
            
            // VMOVDDUP ymm1, ymm2
            __asm__ volatile (
                "vmovddup %1, %0"
                : "=x"(result)
                : "x"(a)
            );
            
            __m256d expected = _mm256_setr_pd(1.0, 1.0, 3.0, 3.0);
            
            printf("Testing register-register operation\n");
            print_ymmd("a", a);
            print_ymmd("result", result);
            print_ymmd("expected", expected);
            
            if (!cmp_ymmd(result, expected)) {
                printf("Mismatch in register-register test\n");
                ret = 1;
            }
        }
        
        // 寄存器-内存测试
        {
            ALIGNED(32) double mem[4] = {5.0, 6.0, 7.0, 8.0};
            __m256d result;
            
            // VMOVDDUP ymm1, [mem]
            __asm__ volatile (
                "vmovddup %1, %0"
                : "=x"(result)
                : "m"(*mem)
            );
            
            __m256d expected = _mm256_setr_pd(5.0, 5.0, 7.0, 7.0);
            
            printf("\nTesting register-memory operation\n");
            printf("Memory operand: %f %f %f %f\n", mem[0], mem[1], mem[2], mem[3]);
            print_ymmd("result", result);
            print_ymmd("expected", expected);
            
            if (!cmp_ymmd(result, expected)) {
                printf("Mismatch in register-memory test\n");
                ret = 1;
            }
        }
    }
    
    return ret;
}

int main() {
    return test_vmovddup();
}
