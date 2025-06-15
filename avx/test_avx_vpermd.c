#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPERMD指令测试
void test_vpermd() {
    printf("=== Testing VPERMD ===\n");
    
    // 测试数据
    int32_t src[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    int32_t idx[8] = {7, 6, 5, 4, 3, 2, 1, 0}; // 反转顺序
    int32_t dst[8] = {0};
    
    // 打印输入
    print_int32_vec("Source", src, 8);
    print_int32_vec("Indices", idx, 8);
    
    // 执行VPERMD指令
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"   // 加载源数据到ymm0
        "vmovdqu %2, %%ymm1\n\t"   // 加载索引到ymm1
        "vpermd %%ymm0, %%ymm1, %%ymm0\n\t" // 执行排列
        "vmovdqu %%ymm0, %0\n\t"   // 存储结果
        : "=m"(dst)
        : "m"(src), "m"(idx)
        : "ymm0", "ymm1"
    );
    
    // 打印结果
    print_int32_vec("Result", dst, 8);
    
    // 验证结果
    int pass = 1;
    for(int i=0; i<8; i++) {
        if(dst[i] != src[7-i]) {
            printf("Mismatch at %d: expected %d, got %d\n", 
                  i, src[7-i], dst[i]);
            pass = 0;
        }
    }
    printf("Test %s\n", pass ? "PASSED" : "FAILED");
}

// VPERMILPS指令测试(128位版本)
void test_vpermilps() {
    printf("\n=== Testing VPERMILPS ===\n");
    
    float src[4] = {1.1f, 2.2f, 3.3f, 4.4f};
    int32_t idx = 0b00011011; // 反转顺序
    float dst[4] = {0};
    
    // 打印输入
    print_float_vec("Source", src, 4);
    printf("Control: 0x%08X\n", idx);
    
    // 执行VPERMILPS指令
    __asm__ __volatile__(
        "vmovups %1, %%xmm0\n\t"   // 加载源数据
        "vpermilps $0x1B, %%xmm0, %%xmm0\n\t" // 使用硬编码控制字(0x1B=00011011)
        "vmovups %%xmm0, %0\n\t"   // 存储结果
        : "=m"(dst)
        : "m"(src)
        : "xmm0"
    );
    
    // 打印结果
    print_float_vec("Result", dst, 4);
    
    // 验证结果
    int pass = 1;
    for(int i=0; i<4; i++) {
        if(!float_equal(dst[i], src[3-i], 0.0001f)) {
            printf("Mismatch at %d: expected %f, got %f\n", 
                  i, src[3-i], dst[i]);
            pass = 0;
        }
    }
    printf("Test %s\n", pass ? "PASSED" : "FAILED");
}

int main() {
    test_vpermd();
    test_vpermilps();
    return 0;
}
