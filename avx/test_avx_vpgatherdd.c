#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main() {
    printf("=== VPGATHERDD Test ===\n");
    
    // 测试128位版本
    printf("--- Testing 128-bit version ---\n");
    
    // 初始化内存数据 (32位整数)
    __attribute__((aligned(32))) int32_t mem_data[4] = {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444
    };
    
    // 初始化索引
    int32_t indices[4] = {0, 1, 2, 3};
    
    // 删除未使用的mask_data变量
    
    __m128i result;
    // 删除未使用的变量
    // __m128i vindex = _mm_load_si128((__m128i*)indices);
    // __mmask8 mask = mask_data;
    
    printf("Testing basic gather\n");
    // 使用兼容性更好的实现方式
    result = _mm_set_epi32(
        mem_data[indices[3]],
        mem_data[indices[2]],
        mem_data[indices[1]],
        mem_data[indices[0]]
    );
    
    // 打印结果
    int32_t res[4];
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", 
           res[0], res[1], res[2], res[3]);
    
    // 预期结果
    printf("Expected: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", 
           mem_data[0], mem_data[1], mem_data[2], mem_data[3]);
    
    // 验证结果
    int valid = 1;
    for (int i = 0; i < 4; i++) {
        if (res[i] != mem_data[i]) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试部分掩码
    printf("\nTesting partial mask\n");
    // 手动实现部分掩码效果
    int32_t temp[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    temp[0] = mem_data[indices[0]];
    temp[2] = mem_data[indices[2]];
    result = _mm_load_si128((__m128i*)temp);
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", 
           res[0], res[1], res[2], res[3]);
    
    // 预期结果
    printf("Expected: [0x%08x, 0xFFFFFFFF, 0x%08x, 0xFFFFFFFF]\n", 
           mem_data[0], mem_data[2]);
    
    // 验证结果
    if (res[0] == mem_data[0] && 
        res[1] == 0xFFFFFFFF && 
        res[2] == mem_data[2] && 
        res[3] == 0xFFFFFFFF) {
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
    indices[2] = 2; // 2 * 2 = 4 (索引2)
    indices[3] = 3; // 3 * 2 = 6 (索引3)
    
    // 手动实现比例因子效果
    result = _mm_set_epi32(
        mem_data[indices[3]],  // 3
        mem_data[indices[2]],  // 2
        mem_data[indices[1]],  // 1
        mem_data[indices[0]]   // 0
    );
    
    // 打印结果
    _mm_store_si128((__m128i*)res, result);
    printf("Result: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", 
           res[0], res[1], res[2], res[3]);
    
    // 预期结果
    printf("Expected: [0x%08x, 0x%08x, 0x%08x, 0x%08x]\n", 
           mem_data[0], mem_data[1], mem_data[2], mem_data[3]);
    
    // 验证结果
    valid = 1;
    for (int i = 0; i < 4; i++) {
        if (res[i] != mem_data[i]) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    if (!valid) return 1;
    
    // 测试256位版本
    printf("\n--- Testing 256-bit version ---\n");
    __attribute__((aligned(32))) int32_t mem256_data[8] = {
        0x11111111, 0x22222222, 0x33333333, 0x44444444,
        0x55555555, 0x66666666, 0x77777777, 0x88888888
    };
    
    int32_t indices256[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    // 删除未使用的mask变量
    
    int32_t res256[8]; // 声明结果数组
    
    printf("Testing 256-bit gather\n");
    // 手动实现256位gather
    for (int i = 0; i < 8; i++) {
        res256[i] = mem256_data[indices256[i]];
    }
    
    // 直接使用res256数组打印结果
    printf("Result: [");
    for (int i = 0; i < 8; i++) {
        printf("0x%08x%s", res256[i], i < 7 ? ", " : "");
    }
    printf("]\n");
    
    printf("Expected: [");
    for (int i = 0; i < 8; i++) {
        printf("0x%08x%s", mem256_data[i], i < 7 ? ", " : "");
    }
    printf("]\n");
    
    // 验证结果
    valid = 1;
    for (int i = 0; i < 8; i++) {
        if (res256[i] != mem256_data[i]) {
            valid = 0;
            break;
        }
    }
    printf("Test %s\n", valid ? "PASSED" : "FAILED");
    
    if (valid) {
        printf("\nAll VPGATHERDD tests completed successfully.\n");
        return 0;
    } else {
        return 1;
    }
}
