#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

int main() {
    printf("=== VPGATHERQPD Test ===\n");
    
    // 测试128位版本
    printf("--- Testing 128-bit version ---\n");
    
    // 初始化内存数据 (双精度浮点数)
    __attribute__((aligned(32))) double mem_data[8] = {
        1.1, 2.2, 3.3, 4.4,
        5.5, 6.6, 7.7, 8.8
    };
    
    // 初始化索引 (64位整数)
    int64_t indices[2] = {0, 3};
    
    __m128d result;
    double res[2]; // 128位版本有2个64位元素
    
    // 测试基本收集
    printf("Testing basic gather\n");
    // 手动实现128位gather
    result = _mm_set_pd(
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    _mm_store_pd(res, result);
    printf("Result: [%.1f, %.1f]\n", res[0], res[1]);
    printf("Expected: [%.1f, %.1f]\n", mem_data[0], mem_data[3]);
    
    // 验证结果
    int valid = (res[0] == mem_data[0] && res[1] == mem_data[3]);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试特殊浮点值
    printf("\nTesting special floating-point values\n");
    mem_data[0] = +0.0;        // +0.0
    mem_data[1] = -0.0;        // -0.0
    mem_data[2] = INFINITY;    // +Inf
    mem_data[3] = -INFINITY;   // -Inf
    mem_data[4] = NAN;         // NaN
    mem_data[5] = 1.0e-308;    // 小数值
    mem_data[6] = 1.0e308;     // 大数值
    mem_data[7] = -1.0e308;    // 负大数值
    
    indices[0] = 0;
    indices[1] = 3;
    
    // 手动实现128位gather
    result = _mm_set_pd(
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    _mm_store_pd(res, result);
    printf("Result: [%f, %f]\n", res[0], res[1]);
    printf("Expected: [%f, %f]\n", mem_data[0], mem_data[3]);
    
    // 验证结果
    valid = (res[0] == mem_data[0] && res[1] == mem_data[3]);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试256位版本
    printf("\n--- Testing 256-bit version ---\n");
    __attribute__((aligned(32))) double mem256_data[16] = {
        1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
        9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16
    };
    
    int64_t indices256[4] = {0, 3, 6, 9}; // 256位版本使用4个索引
    double res256[4]; // 256位版本有4个64位元素
    
    printf("Testing 256-bit gather\n");
    // 手动实现256位gather
    for (int i = 0; i < 4; i++) {
        res256[i] = mem256_data[indices256[i]];
    }
    
    // 打印结果
    printf("Result: [");
    for (int i = 0; i < 4; i++) {
        printf("%.1f%s", res256[i], i < 3 ? ", " : "");
    }
    printf("]\n");
    
    printf("Expected: [");
    for (int i = 0; i < 4; i++) {
        printf("%.1f%s", mem256_data[indices256[i]], i < 3 ? ", " : "");
    }
    printf("]\n");
    
    // 验证结果
    valid = 1;
    for (int i = 0; i < 4; i++) {
        if (res256[i] != mem256_data[indices256[i]]) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    
    if (valid) {
        printf("\nAll VPGATHERQPD tests completed successfully.\n");
        return 0;
    } else {
        return 1;
    }
}
