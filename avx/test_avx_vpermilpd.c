#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPERMILPD指令测试(256位版本)
void test_vpermilpd_ymm() {
    printf("=== Testing VPERMILPD (256-bit) ===\n");
    
    // 测试数据
    double src[4] = {1.1, 2.2, 3.3, 4.4};
    double dst[4] = {0};
    int32_t ctrl = 0b0101; // 交换每对元素
    
    // 打印输入
    print_double_vec("Source", src, 4);
    printf("Control: 0x%08X\n", ctrl);
    
    // 执行VPERMILPD指令
    __asm__ __volatile__(
        "vmovupd %1, %%ymm0\n\t"   // 加载源数据
        "vpermilpd $0x05, %%ymm0, %%ymm0\n\t" // 使用控制字0x05(0101)
        "vmovupd %%ymm0, %0\n\t"   // 存储结果
        : "=m"(dst)
        : "m"(src)
        : "ymm0"
    );
    
    // 打印结果
    print_double_vec("Result", dst, 4);
    
    // 验证结果
    int pass = 1;
    if(!double_equal(dst[0], src[1], 0.0001)) pass = 0;
    if(!double_equal(dst[1], src[0], 0.0001)) pass = 0;
    if(!double_equal(dst[2], src[3], 0.0001)) pass = 0;
    if(!double_equal(dst[3], src[2], 0.0001)) pass = 0;
    
    printf("Test %s\n", pass ? "PASSED" : "FAILED");
}

// VPERMILPD指令测试(128位版本)
void test_vpermilpd_xmm() {
    printf("\n=== Testing VPERMILPS (128-bit) ===\n");
    
    double src[2] = {1.1, 2.2};
    double dst[2] = {0};
    int32_t ctrl = 0b01; // 交换元素
    
    // 打印输入
    print_double_vec("Source", src, 2);
    printf("Control: 0x%08X\n", ctrl);
    
    // 执行VPERMILPD指令
    __asm__ __volatile__(
        "vmovupd %1, %%xmm0\n\t"   // 加载源数据
        "vpermilpd $0x01, %%xmm0, %%xmm0\n\t" // 使用控制字0x01(01)
        "vmovupd %%xmm0, %0\n\t"   // 存储结果
        : "=m"(dst)
        : "m"(src)
        : "xmm0"
    );
    
    // 打印结果
    print_double_vec("Result", dst, 2);
    
    // 验证结果
    int pass = 1;
    if(!double_equal(dst[0], src[1], 0.0001)) pass = 0;
    if(!double_equal(dst[1], src[0], 0.0001)) pass = 0;
    
    printf("Test %s\n", pass ? "PASSED" : "FAILED");
}

int main() {
    test_vpermilpd_ymm();
    test_vpermilpd_xmm();
    return 0;
}
