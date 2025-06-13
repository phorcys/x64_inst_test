#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <immintrin.h>
#include "avx.h"

// 比较两个双精度浮点向量是否相等（考虑NaN）
static int double_vector_equal(double* a, double* b, int count) {
    for (int i = 0; i < count; i++) {
        if (isnan(a[i])) {
            if (!isnan(b[i])) return 0;
        } else {
            if (a[i] != b[i]) return 0;
        }
    }
    return 1;
}

// 测试VMOVAPD指令
void test_vmovapd() {
    int errors = 0;
    printf("Testing VMOVAPD instruction...\n");

    // 确保内存对齐
    #define ALIGNMENT 32
    void* aligned_ptr = NULL;
    
    // 测试128位寄存器到寄存器传输
    {
        double data[2] = {1.0, 2.0};
        __m128d src, dst;
        __m128d expected;

        memcpy(&src, data, sizeof(data));
        expected = src;

        // 使用内联汇编实现VMOVAPD
        asm volatile("vmovapd %1, %0" : "=x"(dst) : "x"(src));
        
        double dst_arr[2], expected_arr[2];
        _mm_storeu_pd(dst_arr, dst);
        _mm_storeu_pd(expected_arr, expected);
        
        if (!double_vector_equal(dst_arr, expected_arr, 2)) {
            printf("Error in 128-bit reg-to-reg transfer\n");
            printf("Expected: [%f, %f]\n", expected_arr[0], expected_arr[1]);
            printf("Got:      [%f, %f]\n", dst_arr[0], dst_arr[1]);
            errors++;
        }
    }

    // 测试256位寄存器到寄存器传输
    {
        double data[4] = {1.0, 2.0, 3.0, 4.0};
        __m256d src, dst;
        __m256d expected;
        
        memcpy(&src, data, sizeof(data));
        expected = src;

        asm volatile("vmovapd %1, %0" : "=x"(dst) : "x"(src));
        
        double dst_arr[4], expected_arr[4];
        _mm256_storeu_pd(dst_arr, dst);
        _mm256_storeu_pd(expected_arr, expected);
        
        if (!double_vector_equal(dst_arr, expected_arr, 4)) {
            printf("Error in 256-bit reg-to-reg transfer\n");
            printf("Expected: [%f, %f, %f, %f]\n", 
                   expected_arr[0], expected_arr[1], expected_arr[2], expected_arr[3]);
            printf("Got:      [%f, %f, %f, %f]\n", 
                   dst_arr[0], dst_arr[1], dst_arr[2], dst_arr[3]);
            errors++;
        }
    }

    // 测试128位对齐内存到寄存器传输
    {
        if (posix_memalign(&aligned_ptr, 16, 16) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        double* aligned_mem = (double*)aligned_ptr;
        
        double data[2] = {10.0, 20.0};
        memcpy(aligned_mem, data, sizeof(data));
        
        __m128d dst;
        __m128d expected;
        memcpy(&expected, data, sizeof(data));

        asm volatile("vmovapd %1, %0" : "=x"(dst) : "m"(*aligned_mem));
        
        double dst_arr[2], expected_arr[2];
        _mm_storeu_pd(dst_arr, dst);
        _mm_storeu_pd(expected_arr, expected);
        
        if (!double_vector_equal(dst_arr, expected_arr, 2)) {
            printf("Error in 128-bit mem-to-reg (aligned)\n");
            printf("Expected: [%f, %f]\n", expected_arr[0], expected_arr[1]);
            printf("Got:      [%f, %f]\n", dst_arr[0], dst_arr[1]);
            errors++;
        }
        free(aligned_ptr);
    }

    // 测试256位对齐内存到寄存器传输
    {
        if (posix_memalign(&aligned_ptr, 32, 32) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        double* aligned_mem = (double*)aligned_ptr;
        
        double data[4] = {0.1, 0.2, 0.3, 0.4};
        memcpy(aligned_mem, data, sizeof(data));
        
        __m256d dst;
        __m256d expected;
        memcpy(&expected, data, sizeof(data));

        asm volatile("vmovapd %1, %0" : "=x"(dst) : "m"(*aligned_mem));
        
        double dst_arr[4], expected_arr[4];
        _mm256_storeu_pd(dst_arr, dst);
        _mm256_storeu_pd(expected_arr, expected);
        
        if (!double_vector_equal(dst_arr, expected_arr, 4)) {
            printf("Error in 256-bit mem-to-reg (aligned)\n");
            printf("Expected: [%f, %f, %f, %f]\n", 
                   expected_arr[0], expected_arr[1], expected_arr[2], expected_arr[3]);
            printf("Got:      [%f, %f, %f, %f]\n", 
                   dst_arr[0], dst_arr[1], dst_arr[2], dst_arr[3]);
            errors++;
        }
        free(aligned_ptr);
    }

    // 测试128位寄存器到对齐内存传输
    {
        if (posix_memalign(&aligned_ptr, 16, 16) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        double* aligned_mem = (double*)aligned_ptr;
        
        double expected[2] = {-1.0, -2.0};
        __m128d src;
        memcpy(&src, expected, sizeof(expected));

        asm volatile("vmovapd %1, %0" : "=m"(*aligned_mem) : "x"(src));
        
        if (memcmp(aligned_mem, expected, sizeof(expected)) != 0) {
            printf("Error in 128-bit reg-to-mem (aligned)\n");
            printf("Expected: [%f, %f]\n", expected[0], expected[1]);
            printf("Got:      [%f, %f]\n", aligned_mem[0], aligned_mem[1]);
            errors++;
        }
        free(aligned_ptr);
    }

    // 测试256位寄存器到对齐内存传输
    {
        if (posix_memalign(&aligned_ptr, 32, 32) != 0) {
            perror("posix_memalign");
            errors++;
            return;
        }
        double* aligned_mem = (double*)aligned_ptr;
        
        double expected[4] = {0.0, -0.0, INFINITY, NAN};
        __m256d src;
        memcpy(&src, expected, sizeof(expected));

        asm volatile("vmovapd %1, %0" : "=m"(*aligned_mem) : "x"(src));
        
        // 特殊值需要特殊比较
        int match = 1;
        for (int i = 0; i < 4; i++) {
            if (isnan(expected[i])) {
                if (!isnan(aligned_mem[i])) match = 0;
            } else {
                if (aligned_mem[i] != expected[i]) match = 0;
            }
        }
        
        if (!match) {
            printf("Error in 256-bit reg-to-mem (aligned) with special values\n");
            printf("Expected: ");
            for (int i = 0; i < 4; i++) printf("%f ", expected[i]);
            printf("\nGot:      ");
            for (int i = 0; i < 4; i++) printf("%f ", aligned_mem[i]);
            printf("\n");
            errors++;
        }
        free(aligned_ptr);
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VMOVAPD tests passed successfully!\n");
    } else {
        printf("VMOVAPD tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vmovapd();
    return 0;
}
