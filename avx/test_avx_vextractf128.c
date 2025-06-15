#include "avx.h"
#include <stdio.h>
#include <string.h>

// vextractf128指令测试函数
static void test_vextractf128() {
    printf("--- Testing vextractf128 (extract 128-bit float data from 256-bit register) ---\n");
    
    // 测试数据
    float src[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float dst_low[4];  // 提取低128位的结果
    float dst_high[4]; // 提取高128位的结果
    float expected_low[4] = {1.0f, 2.0f, 3.0f, 4.0f};  // 预期结果
    float expected_high[4] = {5.0f, 6.0f, 7.0f, 8.0f}; // 预期结果
    
    // 测试提取低128位(立即数0)
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vextractf128 $0, %%ymm0, %0\n\t"
        : "=m"(dst_low)
        : "m"(src)
        : "ymm0"
    );
    
    // 验证结果
    int pass = 1;
    for(int i=0; i<4; i++) {
        if(dst_low[i] != expected_low[i]) {
            printf("Low128 mismatch at position %d: got %f, expected %f\n", 
                  i, dst_low[i], expected_low[i]);
            pass = 0;
        }
    }
    
    if(pass) {
        printf("Extract low128 test passed\n");
    } else {
        printf("Extract low128 test failed\n");
    }
    
    // 测试提取高128位(立即数1)
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vextractf128 $1, %%ymm0, %0\n\t"
        : "=m"(dst_high)
        : "m"(src)
        : "ymm0"
    );
    
    // 验证结果
    pass = 1;
    for(int i=0; i<4; i++) {
        if(dst_high[i] != expected_high[i]) {
            printf("High128 mismatch at position %d: got %f, expected %f\n", 
                  i, dst_high[i], expected_high[i]);
            pass = 0;
        }
    }
    
    if(pass) {
        printf("Extract high128 test passed\n");
    } else {
        printf("Extract high128 test failed\n");
    }
}

int main() {
    test_vextractf128();
    return 0;
}
