#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// VPBLENDD指令测试
void test_vpblendd() {
    printf("=== Testing VPBLENDD ===\n");
    
    // 测试数据
    ALIGNED(32) int32_t src1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    ALIGNED(32) int32_t src2[8] = {9, 10, 11, 12, 13, 14, 15, 16};
    ALIGNED(32) int32_t dst[8];
    
    // 加载测试数据到寄存器
    __m256i ymm1 = _mm256_load_si256((__m256i*)src1);
    __m256i ymm2 = _mm256_load_si256((__m256i*)src2);
    
    // 测试1: 混合模式0x00(全选源1)
    __m256i res;
    asm volatile (
        "vpblendd $0x00, %1, %2, %0"
        : "=x"(res)
        : "x"(ymm2), "x"(ymm1)
    );
    _mm256_store_si256((__m256i*)dst, res);
    printf("Test1 - Blend mode 0x00 (all from src1):\n");
    print_int32_vec("Expected", src1, 8);
    print_int32_vec("Result  ", dst, 8);
    printf("Match: %s\n\n", cmp_ymm(res, ymm1) ? "YES" : "NO");
    
    // 测试2: 混合模式0xFF(全选源2)
    asm volatile (
        "vpblendd $0xFF, %1, %2, %0"
        : "=x"(res)
        : "x"(ymm2), "x"(ymm1)
    );
    _mm256_store_si256((__m256i*)dst, res);
    printf("Test2 - Blend mode 0xFF (all from src2):\n");
    print_int32_vec("Expected", src2, 8);
    print_int32_vec("Result  ", dst, 8);
    printf("Match: %s\n\n", cmp_ymm(res, ymm2) ? "YES" : "NO");
    
    // 测试3: 交替选择(0xAA)
    asm volatile (
        "vpblendd $0xAA, %1, %2, %0"
        : "=x"(res)
        : "x"(ymm2), "x"(ymm1)
    );
    _mm256_store_si256((__m256i*)dst, res);
    int32_t expected3[8] = {1, 10, 3, 12, 5, 14, 7, 16};
    printf("Test3 - Blend mode 0xAA (alternating):\n");
    print_int32_vec("Expected", expected3, 8);
    print_int32_vec("Result  ", dst, 8);
    printf("Match: %s\n\n", memcmp(dst, expected3, sizeof(expected3)) == 0 ? "YES" : "NO");
    
    // 测试4: 寄存器-内存操作
    ALIGNED(32) int32_t expected4[8] = {9, 2, 11, 4, 13, 6, 15, 8};
    asm volatile (
        "vpblendd $0x55, %1, %2, %0"
        : "=x"(res)
        : "m"(*(__m256i*)src2), "x"(ymm1)
    );
    _mm256_store_si256((__m256i*)dst, res);
    printf("Test4 - Register-Memory blend mode 0x55:\n");
    print_int32_vec("Expected", expected4, 8);
    print_int32_vec("Result  ", dst, 8);
    printf("Match: %s\n\n", memcmp(dst, expected4, sizeof(expected4)) == 0 ? "YES" : "NO");
    
    // 测试5: 边界值测试
    ALIGNED(32) int32_t boundary1[8] = {INT32_MAX, INT32_MIN, 0, -1, 1, -2, 2, -3};
    ALIGNED(32) int32_t boundary2[8] = {INT32_MIN, INT32_MAX, -1, 0, -2, 1, -3, 2};
    __m256i ymm_bound1 = _mm256_load_si256((__m256i*)boundary1);
    __m256i ymm_bound2 = _mm256_load_si256((__m256i*)boundary2);
    
    asm volatile (
        "vpblendd $0x3C, %1, %2, %0"
        : "=x"(res)
        : "x"(ymm_bound2), "x"(ymm_bound1)
    );
    _mm256_store_si256((__m256i*)dst, res);
    int32_t expected5[8] = {INT32_MAX, INT32_MIN, -1, 0, -2, 1, 2, -3};
    printf("Test5 - Boundary values blend mode 0x3C:\n");
    print_int32_vec("Expected", expected5, 8);
    print_int32_vec("Result  ", dst, 8);
    printf("Match: %s\n\n", memcmp(dst, expected5, sizeof(expected5)) == 0 ? "YES" : "NO");
}

int main() {
    test_vpblendd();
    return 0;
}
