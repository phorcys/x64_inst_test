#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <immintrin.h>
#include "avx.h"

// 比较两个单精度浮点向量是否相等（考虑NaN）
static int float_vector_equal(float* a, float* b, int count) {
    for (int i = 0; i < count; i++) {
        if (isnan(a[i])) {
            if (!isnan(b[i])) return 0;
        } else {
            if (a[i] != b[i]) return 0;
        }
    }
    return 1;
}

// 测试VMOVUPS指令
void test_vmovups() {
    int errors = 0;
    printf("Testing VMOVUPS instruction...\n");

    // 分配未对齐内存（故意偏移1字节）
    uint8_t buffer[64];
    float* unaligned_mem = (float*)(buffer + 1);
    
    // 测试128位寄存器到寄存器传输
    {
        float data[4] = {1.5f, 2.5f, 3.5f, 4.5f};
        __m128 src, dst;
        __m128 expected;

        memcpy(&src, data, sizeof(data));
        expected = src;

        // 使用内联汇编实现VMOVUPS
        asm volatile("vmovups %1, %0" : "=x"(dst) : "x"(src));
        
        float dst_arr[4], expected_arr[4];
        _mm_storeu_ps(dst_arr, dst);
        _mm_storeu_ps(expected_arr, expected);
        
        if (!float_vector_equal(dst_arr, expected_arr, 4)) {
            printf("Error in 128-bit reg-to-reg transfer\n");
            print_vector128("Expected", expected);
            print_vector128("Got", dst);
            errors++;
        }
    }

    // 测试256位寄存器到寄存器传输
    {
        float data[8] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
        __m256 src, dst;
        __m256 expected;
        
        memcpy(&src, data, sizeof(data));
        expected = src;

        asm volatile("vmovups %1, %0" : "=x"(dst) : "x"(src));
        
        float dst_arr[8], expected_arr[8];
        _mm256_storeu_ps(dst_arr, dst);
        _mm256_storeu_ps(expected_arr, expected);
        
        if (!float_vector_equal(dst_arr, expected_arr, 8)) {
            printf("Error in 256-bit reg-to-reg transfer\n");
            print_vector256("Expected", expected);
            print_vector256("Got", dst);
            errors++;
        }
    }

    // 测试128位未对齐内存到寄存器传输
    {
        float data[4] = {-1.0f, -2.0f, -3.0f, -4.0f};
        memcpy(unaligned_mem, data, sizeof(data));
        
        __m128 dst;
        __m128 expected;
        memcpy(&expected, data, sizeof(data));

        asm volatile("vmovups %1, %0" : "=x"(dst) : "m"(*unaligned_mem));
        
        float dst_arr[4], expected_arr[4];
        _mm_storeu_ps(dst_arr, dst);
        _mm_storeu_ps(expected_arr, expected);
        
        if (!float_vector_equal(dst_arr, expected_arr, 4)) {
            printf("Error in 128-bit mem-to-reg (unaligned)\n");
            print_vector128("Expected", expected);
            print_vector128("Got", dst);
            errors++;
        }
    }

    // 测试256位未对齐内存到寄存器传输
    {
        float data[8] = {1e10f, 1e-10f, INFINITY, -INFINITY, NAN, -NAN, 0.0f, -0.0f};
        memcpy(unaligned_mem, data, sizeof(data));
        
        __m256 dst;
        __m256 expected;
        memcpy(&expected, data, sizeof(data));

        asm volatile("vmovups %1, %0" : "=x"(dst) : "m"(*unaligned_mem));
        
        float dst_arr[8], expected_arr[8];
        _mm256_storeu_ps(dst_arr, dst);
        _mm256_storeu_ps(expected_arr, expected);
        
        // 特殊值需要特殊比较
        int match = 1;
        for (int i = 0; i < 8; i++) {
            if (isnan(expected_arr[i])) {
                if (!isnan(dst_arr[i])) match = 0;
            } else {
                if (dst_arr[i] != expected_arr[i]) match = 0;
            }
        }
        
        if (!match) {
            printf("Error in 256-bit mem-to-reg (unaligned) with special values\n");
            print_vector256("Expected", expected);
            print_vector256("Got", dst);
            errors++;
        }
    }

    // 测试128位寄存器到未对齐内存传输
    {
        float expected[4] = {123.456f, 789.012f, 345.678f, 901.234f};
        __m128 src;
        memcpy(&src, expected, sizeof(expected));

        asm volatile("vmovups %1, %0" : "=m"(*unaligned_mem) : "x"(src));
        
        if (memcmp(unaligned_mem, expected, sizeof(expected)) != 0) {
            printf("Error in 128-bit reg-to-mem (unaligned)\n");
            printf("Expected: ");
            for (int i = 0; i < 4; i++) printf("%f ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 4; i++) printf("%f ", unaligned_mem[i]);
            printf("\n");
            errors++;
        }
    }

    // 测试256位寄存器到未对齐内存传输
    {
        float expected[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        __m256 src;
        memcpy(&src, expected, sizeof(expected));

        asm volatile("vmovups %1, %0" : "=m"(*unaligned_mem) : "x"(src));
        
        if (memcmp(unaligned_mem, expected, sizeof(expected)) != 0) {
            printf("Error in 256-bit reg-to-mem (unaligned)\n");
            printf("Expected: ");
            for (int i = 0; i < 8; i++) printf("%f ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 8; i++) printf("%f ", unaligned_mem[i]);
            printf("\n");
            errors++;
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VMOVUPS tests passed successfully!\n");
    } else {
        printf("VMOVUPS tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vmovups();
    return 0;
}
