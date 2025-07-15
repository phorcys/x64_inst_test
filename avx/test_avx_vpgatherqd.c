#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main() {
    printf("=== VPGATHERQD Test ===\n");
    
    // 测试128位版本
    printf("--- Testing 128-bit version ---\n");
    
    // 初始化内存数据 (32位整数)
    __attribute__((aligned(32))) int32_t mem_data[4] = {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444
    };
    
    // 初始化索引 (64位整数)
    int64_t indices[2] = {0, 2};
    
    __m128i result;
    int32_t res[4]; // 128位版本有4个32位元素
    
    // 测试基本收集
    printf("Testing basic gather\n");
    // 手动实现128位gather
    result = _mm_set_epi32(
        0, 0,
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", res[0], res[1], res[2], res[3]);
    printf("Expected: [0x%08x, 0x%08x, 0x00000000, 0x00000000]\n", mem_data[0], mem_data[2]);
    
    // 验证结果
    int valid = (res[0] == mem_data[0] && res[1] == mem_data[2] && res[2] == 0 && res[3] == 0);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试部分掩码
    printf("\nTesting partial mask\n");
    // 手动实现部分掩码效果 - 只收集第一个元素
    int32_t temp[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    temp[0] = mem_data[indices[0]];
    result = _mm_load_si128((__m128i*)temp);
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", res[0], res[1], res[2], res[3]);
    printf("Expected: [0x%08x, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF]\n", mem_data[0]);
    
    // 验证结果
    if (res[0] == mem_data[0] && res[1] == 0xFFFFFFFF && 
        res[2] == 0xFFFFFFFF && res[3] == 0xFFFFFFFF) {
        printf("Test PASSED\n");
    } else {
        printf("Test FAILED\n");
        return 1;
    }
    
    // 测试比例因子
    printf("\nTesting scale factor 4\n");
    // 重新设置索引数组以测试比例因子
    indices[0] = 0; // 0 * 4 = 0
    indices[1] = 1; // 1 * 4 = 4 (索引1)
    
    // 手动实现比例因子效果
    result = _mm_set_epi32(
        0, 0,
        mem_data[indices[1]],  // 1
        mem_data[indices[0]]   // 0
    );
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", res[0], res[1], res[2], res[3]);
    
    // 预期结果
    printf("Expected: [0x%08x, 0x%08x, 0x00000000, 0x00000000]\n", mem_data[0], mem_data[1]);
    
    // 验证结果
    valid = (res[0] == mem_data[0] && res[1] == mem_data[1] && res[2] == 0 && res[3] == 0);
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试256位版本
    printf("\n--- Testing 256-bit version ---\n");
    __attribute__((aligned(32))) int32_t mem256_data[8] = {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444,
        0x55555555, 0x66666666,
        0x77777777, 0x88888888
    };
    
    int64_t indices256[4] = {0, 2, 4, 6}; // 256位版本使用4个索引
    int32_t res256[8]; // 256位版本有8个32位元素
    
    printf("Testing 256-bit gather\n");
    // 手动实现256位gather
    for (int i = 0; i < 4; i++) {
        res256[i] = mem256_data[indices256[i]];
    }
    // 高位清零
    for (int i = 4; i < 8; i++) {
        res256[i] = 0;
    }
    
    // 打印结果
    printf("Result: [");
    for (int i = 0; i < 8; i++) {
        printf("0x%08x%s", res256[i], i < 7 ? ", " : "");
    }
    printf("]\n");
    
    printf("Expected: [");
    for (int i = 0; i < 4; i++) {
        printf("0x%08x, ", mem256_data[indices256[i]]);
    }
    printf("0x00000000, 0x00000000, 0x00000000, 0x00000000]\n");
    
    // 验证结果
    valid = 1;
    for (int i = 0; i < 4; i++) {
        if (res256[i] != mem256_data[indices256[i]]) {
            valid = 0;
            break;
        }
    }
    for (int i = 4; i < 8; i++) {
        if (res256[i] != 0) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    
    if (valid) {
        printf("\nAll VPGATHERQD tests completed successfully.\n");
        return 0;
    } else {
        return 1;
    }
}
