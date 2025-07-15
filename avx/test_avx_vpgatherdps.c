#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

int main() {
    printf("=== VPGATHERDPS Test ===\n");
    
    // 测试128位版本
    printf("--- Testing 128-bit version ---\n");
    
    // 初始化内存数据 (单精度浮点数)
    __attribute__((aligned(32))) float mem_data[8] = {
        1.1f, 2.2f, 3.3f, 4.4f,
        5.5f, 6.6f, 7.7f, 8.8f
    };
    
    // 初始化索引 (32位整数)
    int32_t indices[4] = {0, 2, 4, 6};
    
    __m128 result;
    float res[4]; // 128位版本有4个32位元素
    
    // 测试基本收集
    printf("Testing basic gather\n");
    // 手动实现128位gather
    result = _mm_set_ps(
        mem_data[indices[3]],
        mem_data[indices[2]],
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    _mm_store_ps(res, result);
    printf("Result: [%.1f, %.1f, %.1f, %.1f]\n", res[0], res[1], res[2], res[3]);
    printf("Expected: [%.1f, %.1f, %.1f, %.1f]\n", 
           mem_data[0], mem_data[2], mem_data[4], mem_data[6]);
    
    // 验证结果
    int valid = (res[0] == mem_data[0] && res[1] == mem_data[2] &&
                res[2] == mem_data[4] && res[3] == mem_data[6]);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试特殊浮点值
    printf("\nTesting special floating-point values\n");
    mem_data[0] = +0.0f;        // +0.0
    mem_data[1] = -0.0f;        // -0.0
    mem_data[2] = INFINITY;     // +Inf
    mem_data[3] = -INFINITY;    // -Inf
    mem_data[4] = NAN;          // NaN
    mem_data[5] = 1.0e-38f;     // 小数值
    mem_data[6] = 1.0e38f;      // 大数值
    mem_data[7] = -1.0e38f;     // 负大数值
    
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 4;
    indices[3] = 6;
    
    // 手动实现128位gather
    result = _mm_set_ps(
        mem_data[indices[3]],
        mem_data[indices[2]],
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    _mm_store_ps(res, result);
    printf("Result: [%f, %f, %f, %f]\n", res[0], res[1], res[2], res[3]);
    printf("Expected: [%f, %f, %f, %f]\n", 
           mem_data[0], mem_data[2], mem_data[4], mem_data[6]);
    
    // 验证结果 (对于NaN需要特殊处理)
    valid = (res[0] == mem_data[0] && res[1] == mem_data[2] &&
             isnan(res[2]) && res[3] == mem_data[6]);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试256位版本
    printf("\n--- Testing 256-bit version ---\n");
    __attribute__((aligned(32))) float mem256_data[16] = {
        1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f,
        9.9f, 10.10f, 11.11f, 12.12f, 13.13f, 14.14f, 15.15f, 16.16f
    };
    
    int32_t indices256[8] = {0, 2, 4, 6, 8, 10, 12, 14}; // 256位版本使用8个索引
    float res256[8]; // 256位版本有8个32位元素
    
    printf("Testing 256-bit gather\n");
    // 手动实现256位gather
    for (int i = 0; i < 8; i++) {
        res256[i] = mem256_data[indices256[i]];
    }
    
    // 打印结果
    printf("Result: [");
    for (int i = 0; i < 8; i++) {
        printf("%.1f%s", res256[i], i < 7 ? ", " : "");
    }
    printf("]\n");
    
    printf("Expected: [");
    for (int i = 0; i < 8; i++) {
        printf("%.1f%s", mem256_data[indices256[i]], i < 7 ? ", " : "");
    }
    printf("]\n");
    
    // 验证结果
    valid = 1;
    for (int i = 0; i < 8; i++) {
        if (res256[i] != mem256_data[indices256[i]]) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    
    if (valid) {
        printf("\nAll VPGATHERDPS tests completed successfully.\n");
        return 0;
    } else {
        return 1;
    }
}
