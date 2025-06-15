#include "avx.h"
#include <stdio.h>
#include <string.h>

// vinserti128指令测试函数
static void test_vinserti128() {
    printf("--- Testing vinserti128 (insert 128-bit integer data into 256-bit register) ---\n");
    
    // 测试数据
    int32_t src1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t src2[4] = {10, 20, 30, 40};
    int32_t dst[8];
    int32_t expected_low[8];  // 插入低128位的预期结果
    int32_t expected_high[8]; // 插入高128位的预期结果
    
    // 计算预期结果
    memcpy(expected_low, src1, sizeof(int32_t)*8);
    memcpy(expected_low, src2, sizeof(int32_t)*4);
    
    memcpy(expected_high, src1, sizeof(int32_t)*8);
    memcpy(expected_high+4, src2, sizeof(int32_t)*4);
    
    // 测试插入低128位(立即数0)
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%xmm1\n\t"
        "vinserti128 $0, %%xmm1, %%ymm0, %%ymm2\n\t"
        "vmovdqu %%ymm2, %0\n\t"
        : "=m"(dst)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 验证结果
    int pass = 1;
    for(int i=0; i<8; i++) {
        if(dst[i] != expected_low[i]) {
            printf("Low128 mismatch at position %d: got %d, expected %d\n", 
                  i, dst[i], expected_low[i]);
            pass = 0;
        }
    }
    
    if(pass) {
        printf("Insert to low128 test passed\n");
    } else {
        printf("Insert to low128 test failed\n");
    }
    
    // 测试插入高128位(立即数1)
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%xmm1\n\t"
        "vinserti128 $1, %%xmm1, %%ymm0, %%ymm2\n\t"
        "vmovdqu %%ymm2, %0\n\t"
        : "=m"(dst)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    // 验证结果
    pass = 1;
    for(int i=0; i<8; i++) {
        if(dst[i] != expected_high[i]) {
            printf("High128 mismatch at position %d: got %d, expected %d\n", 
                  i, dst[i], expected_high[i]);
            pass = 0;
        }
    }
    
    if(pass) {
        printf("Insert to high128 test passed\n");
    } else {
        printf("Insert to high128 test failed\n");
    }
}

int main() {
    test_vinserti128();
    return 0;
}
