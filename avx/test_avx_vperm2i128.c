#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试单个imm8组合的辅助函数
static void test_case(__m256i ymm1, __m256i ymm2, __m256i* result, unsigned imm8, uint8_t* buf1, uint8_t* buf2);

// 测试VPERM2I128指令
void test_vperm2i128() {
    printf("=== Testing VPERM2I128 ===\n");

    // 准备测试数据
    __m256i ymm1, ymm2, result;
    uint8_t buf1[32], buf2[32];
    
    // 初始化数据
    for(int i=0; i<32; i++) {
        buf1[i] = i;
        buf2[i] = 32 + i;
    }
    ymm1 = _mm256_loadu_si256((__m256i*)buf1);
    ymm2 = _mm256_loadu_si256((__m256i*)buf2);

    // 测试关键imm8组合
    const unsigned test_imm8[] = {
        0x00, 0x01, 0x02, 0x03,  // 所有src1组合
        0x10, 0x11, 0x12, 0x13,  // 所有src2组合
        0x08, 0x80,               // zero标志位
        0x20, 0x31, 0x44, 0x55   // 其他随机组合
    };
    const size_t num_tests = sizeof(test_imm8)/sizeof(test_imm8[0]);
    
    for(size_t i = 0; i < num_tests; i++) {
        test_case(ymm1, ymm2, &result, test_imm8[i], buf1, buf2);
    }

    // 测试内存操作数
    printf("\n--- Testing memory operand ---\n");
    __m256i mem_operand;
    memcpy(&mem_operand, buf2, 32);
    
    asm volatile (
        "vperm2i128 $0x20, %[src2], %[src1], %[dst]"
        : [dst]"=x"(result)
        : [src1]"x"(ymm1), [src2]"m"(mem_operand)
    );
    
    print_ymm("Memory operand result", result);
}

// 测试单个imm8组合的辅助函数实现
static void test_case(__m256i ymm1, __m256i ymm2, __m256i* result, unsigned imm8, uint8_t* buf1, uint8_t* buf2) {
    printf("\n--- Testing imm8=0x%02x ---\n", imm8);
    
    switch(imm8) {
        case 0x00:
            asm volatile("vperm2i128 $0x00, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x01:
            asm volatile("vperm2i128 $0x01, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x02:
            asm volatile("vperm2i128 $0x02, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x03:
            asm volatile("vperm2i128 $0x03, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x10:
            asm volatile("vperm2i128 $0x10, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x11:
            asm volatile("vperm2i128 $0x11, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x12:
            asm volatile("vperm2i128 $0x12, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x13:
            asm volatile("vperm2i128 $0x13, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x08:
            asm volatile("vperm2i128 $0x08, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x80:
            asm volatile("vperm2i128 $0x80, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x20:
            asm volatile("vperm2i128 $0x20, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x31:
            asm volatile("vperm2i128 $0x31, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x44:
            asm volatile("vperm2i128 $0x44, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        case 0x55:
            asm volatile("vperm2i128 $0x55, %1, %2, %0" : "=x"(*result) : "x"(ymm2), "x"(ymm1)); break;
        default:
            printf("Unsupported imm8 value: 0x%02x\n", imm8);
            return;
    }

    // 打印结果
    print_ymm("Result", *result);

    // 验证结果
    uint8_t expected[32] = {0};
    int src1 = (imm8 >> 0) & 3;
    int src2 = (imm8 >> 4) & 3;
    int zero_low = (imm8 >> 3) & 1;
    int zero_high = (imm8 >> 7) & 1;

    // 计算预期结果
    if(!zero_low) {
        if(src1 == 0) memcpy(&expected[0], &buf1[0], 16);
        else if(src1 == 1) memcpy(&expected[0], &buf1[16], 16);
        else if(src1 == 2) memcpy(&expected[0], &buf2[0], 16);
        else if(src1 == 3) memcpy(&expected[0], &buf2[16], 16);
    }

    if(!zero_high) {
        if(src2 == 0) memcpy(&expected[16], &buf1[0], 16);
        else if(src2 == 1) memcpy(&expected[16], &buf1[16], 16);
        else if(src2 == 2) memcpy(&expected[16], &buf2[0], 16);
        else if(src2 == 3) memcpy(&expected[16], &buf2[16], 16);
    }

    // 比较结果
    uint8_t actual[32];
    memcpy(actual, result, 32);
    
    if(memcmp(actual, expected, 32) == 0) {
        printf("Test passed for imm8=0x%02x\n", imm8);
    } else {
        printf("Test FAILED for imm8=0x%02x\n", imm8);
        print_ymm("Expected", *(__m256i*)expected);
    }
}

int main() {
    test_vperm2i128();
    return 0;
}
