#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// 打印128位寄存器内容(使用avx.h中的定义)
static void print_xmm_values(const char* name, uint8_t* val) {
    __m128i xmm;
    memcpy(&xmm, val, 16);
    print_xmm(name, xmm);
}

// 打印256位寄存器内容(使用avx.h中的定义) 
static void print_ymm_values(const char* name, uint8_t* val) {
    __m256i ymm;
    memcpy(&ymm, val, 32);
    print_ymm(name, ymm);
}

int test_vpslldq() {
    printf("Testing VPSLLDQ instruction\n");
    printf("==========================\n");
    
    int all_tests_passed = 1;

    // 测试1: 128位基本功能测试
    {
        printf("Test 1: 128-bit basic functionality\n");
        
        uint8_t src[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                          0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
        uint8_t dst[16] = {0};
        uint8_t expected[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vpslldq $1, %%xmm0, %%xmm1\n\t"
            "vmovdqu %%xmm1, %0\n\t"
            : "=m" (dst)
            : "m" (src)
            : "xmm0", "xmm1", "memory"
        );

        print_xmm_values("Input", src);
        print_xmm_values("Result", dst);
        print_xmm_values("Expected", expected);

        int pass = 1;
        for (int i = 0; i < 16; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at byte %d: got %02X, expected %02X\n",
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) printf("[PASS]\n");
        else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    // 测试2: 256位AVX测试
    {
        printf("\nTest 2: 256-bit AVX operation\n");
        
        uint8_t src[32] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                         0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                         0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20};
        uint8_t dst[32] = {0};
        uint8_t expected[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                              0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                              0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

        __asm__ __volatile__(
            "vmovdqu %1, %%ymm0\n\t"
            "vpslldq $1, %%ymm0, %%ymm1\n\t"
            "vmovdqu %%ymm1, %0\n\t"
            : "=m" (dst)
            : "m" (src)
            : "ymm0", "ymm1", "memory"
        );

        print_ymm_values("Input", src);
        print_ymm_values("Result", dst);
        print_ymm_values("Expected", expected);

        int pass = 1;
        for (int i = 0; i < 32; i++) {
            if (dst[i] != expected[i]) {
                pass = 0;
                printf("Mismatch at byte %d: got %02X, expected %02X\n",
                      i, dst[i], expected[i]);
            }
        }
        
        if (pass) printf("[PASS]\n");
        else {
            printf("[FAIL]\n");
            all_tests_passed = 0;
        }
    }

    printf("\nVPSLLDQ tests %s\n", all_tests_passed ? "PASSED" : "FAILED");
    return all_tests_passed;
}

int main() {
    return test_vpslldq() ? 0 : 1;
}
