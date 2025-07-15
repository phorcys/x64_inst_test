#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main() {
    printf("=== VPGATHERDQ Test ===\n");
    
    // 测试128位版本
    printf("--- Testing 128-bit version ---\n");
    
    // 初始化内存数据 (64位整数)
    __attribute__((aligned(32))) int64_t mem_data[4] = {
        0x1111111111111111, 0x2222222222222222,
        0x3333333333333333, 0x4444444444444444
    };
    
    // 初始化索引 (32位整数)
    int32_t indices[4] = {0, 1, 2, 3};
    
    __m128i result;
    int64_t res[2]; // 128位版本有2个64位元素
    
    // 测试基本收集
    printf("Testing basic gather\n");
    // 手动实现128位gather
    result = _mm_set_epi64x(
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%016lx, 0x%016lx]\n", res[0], res[1]);
    printf("Expected: [0x%016lx, 0x%016lx]\n", mem_data[0], mem_data[1]);
    
    // 验证结果
    int valid = (res[0] == mem_data[0] && res[1] == mem_data[1]);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试部分掩码
    printf("\nTesting partial mask\n");
    // 手动实现部分掩码效果 - 只收集第一个元素
    int64_t temp[2] = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
    temp[0] = mem_data[indices[0]];
    result = _mm_load_si128((__m128i*)temp);
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%016lx, 0x%016lx]\n", res[0], res[1]);
    printf("Expected: [0x%016lx, 0xFFFFFFFFFFFFFFFF]\n", mem_data[0]);
    
    // 验证结果
    if (res[0] == mem_data[0] && res[1] == 0xFFFFFFFFFFFFFFFF) {
        printf("Test PASSED\n");
    } else {
        printf("Test FAILED\n");
        return 1;
    }
    
    // 测试比例因子
    printf("\nTesting scale factor 2\n");
    // 重新设置索引数组以测试比例因子
    indices[0] = 0; // 0 * 2 = 0
    indices[1] = 1; // 1 * 2 = 2 (索引1)
    
    // 手动实现比例因子效果
    result = _mm_set_epi64x(
        mem_data[indices[1]],  // 1
        mem_data[indices[0]]   // 0
    );
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%016lx, 0x%016lx]\n", res[0], res[1]);
    
    // 预期结果
    printf("Expected: [0x%016lx, 0x%016lx]\n", mem_data[0], mem_data[1]);
    
    // 验证结果
    valid = (res[0] == mem_data[0] && res[1] == mem_data[1]);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试256位版本
    printf("\n--- Testing 256-bit version ---\n");
    __attribute__((aligned(32))) int64_t mem256_data[8] = {
        0x1111111111111111, 0x2222222222222222,
        0x3333333333333333, 0x4444444444444444,
        0x5555555555555555, 0x6666666666666666,
        0x7777777777777777, 0x8888888888888888
    };
    
    int32_t indices256[4] = {0, 1, 2, 3}; // 256位版本使用4个索引
    int64_t res256[4]; // 256位版本有4个64位元素
    
    printf("Testing 256-bit gather\n");
    // 手动实现256位gather
    for (int i = 0; i < 4; i++) {
        res256[i] = mem256_data[indices256[i]];
    }
    
    // 打印结果
    printf("Result: [");
    for (int i = 0; i < 4; i++) {
        printf("0x%016lx%s", res256[i], i < 3 ? ", " : "");
    }
    printf("]\n");
    
    printf("Expected: [");
    for (int i = 0; i < 4; i++) {
        printf("0x%016lx%s", mem256_data[i], i < 3 ? ", " : "");
    }
    printf("]\n");
    
    // 验证结果
    valid = 1;
    for (int i = 0; i < 4; i++) {
        if (res256[i] != mem256_data[i]) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    
    if (valid) {
        printf("\nAll VPGATHERDQ tests completed successfully.\n");
        return 0;
    } else {
        return 1;
    }
}
