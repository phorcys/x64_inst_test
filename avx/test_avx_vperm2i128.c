#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 打印YMM寄存器的辅助函数
void test_vperm2i128() {
    printf("==== Testing VPERM2I128 (25 cases) ====\n");
    
    // 测试数据
    ALIGNED(32) int32_t src1[8] = {0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888};
    ALIGNED(32) int32_t src2[8] = {0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x00000000};
    ALIGNED(32) int32_t dst[8];
    
    // 25个立即数组合
    uint8_t imms[25] = {
        // 低位不清零组合
        0x00, 0x01, 0x02, 0x03, 
        0x10, 0x11, 0x12, 0x13, 
        0x20, 0x21, 0x22, 0x23, 
        0x30, 0x31, 0x32, 0x33,
        
        // 低位清零组合 (使用0x8标志)
        0x08, 0x18, 0x28, 0x38,
        
        // 高位清零组合
        0x80, 0x81, 0x82, 0x83,
        
        // 全清零组合
        0x88  // 同时设置低位清零(0x8)和高位清零(0x80)
    };
    
    const char* desc[25] = {
        "0x00: low=src1[0:3], high=src1[0:3]",
        "0x01: low=src1[4:7], high=src1[0:3]",
        "0x02: low=src2[0:3], high=src1[0:3]",
        "0x03: low=src2[4:7], high=src1[0:3]",
        "0x10: low=src1[0:3], high=src1[4:7]",
        "0x11: low=src1[4:7], high=src1[4:7]",
        "0x12: low=src2[0:3], high=src1[4:7]",
        "0x13: low=src2[4:7], high=src1[4:7]",
        "0x20: low=src1[0:3], high=src2[0:3]",
        "0x21: low=src1[4:7], high=src2[0:3]",
        "0x22: low=src2[0:3], high=src2[0:3]",
        "0x23: low=src2[4:7], high=src2[0:3]",
        "0x30: low=src1[0:3], high=src2[4:7]",
        "0x31: low=src1[4:7], high=src2[4:7]",
        "0x32: low=src2[0:3], high=src2[4:7]",
        "0x33: low=src2[4:7], high=src2[4:7]",
        "0x08: low=zero, high=src1[0:3]",
        "0x18: low=zero, high=src1[4:7]",
        "0x28: low=zero, high=src2[0:3]",
        "0x38: low=zero, high=src2[4:7]",
        "0x80: low=src1[0:3], high=zero",
        "0x81: low=src1[4:7], high=zero",
        "0x82: low=src2[0:3], high=zero",
        "0x83: low=src2[4:7], high=zero",
        "0x88: low=zero, high=zero"
    };
    
    // 计算预期结果
    int32_t expected[25][8];
    for (int i = 0; i < 25; i++) {
        uint8_t imm = imms[i];
        uint8_t low_sel = imm & 0x03;
        uint8_t high_sel = (imm >> 4) & 0x03;
        int low_zero = (imm & 0x08) != 0;  // 低位清零标志是0x8
        int high_zero = (imm & 0x80) != 0;
        
        // 处理低位字段
        if (low_zero) {
            memset(&expected[i][0], 0, 4*sizeof(int32_t));
        } else {
            const int32_t* src = (low_sel & 0x02) ? src2 : src1;
            int offset = (low_sel & 0x01) ? 4 : 0;
            memcpy(&expected[i][0], &src[offset], 4*sizeof(int32_t));
        }
        
        // 处理高位字段
        if (high_zero) {
            memset(&expected[i][4], 0, 4*sizeof(int32_t));
        } else {
            const int32_t* src = (high_sel & 0x02) ? src2 : src1;
            int offset = (high_sel & 0x01) ? 4 : 0;
            memcpy(&expected[i][4], &src[offset], 4*sizeof(int32_t));
        }
    }

    __m256i ymm1 = _mm256_load_si256((__m256i*)src1);
    __m256i ymm2 = _mm256_load_si256((__m256i*)src2);
    __m256i ymm_dst;

    int pass_count = 0;
    for (int i = 0; i < 25; i++) {
        uint8_t imm = imms[i];
        
        // 使用内联汇编执行指令，为每个立即数单独生成指令
        switch(imm) {
            case 0x00: asm volatile("vperm2f128 $0x00, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x01: asm volatile("vperm2f128 $0x01, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x02: asm volatile("vperm2f128 $0x02, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x03: asm volatile("vperm2f128 $0x03, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x10: asm volatile("vperm2f128 $0x10, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x11: asm volatile("vperm2f128 $0x11, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x12: asm volatile("vperm2f128 $0x12, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x13: asm volatile("vperm2f128 $0x13, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x20: asm volatile("vperm2f128 $0x20, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x21: asm volatile("vperm2f128 $0x21, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x22: asm volatile("vperm2f128 $0x22, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x23: asm volatile("vperm2f128 $0x23, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x30: asm volatile("vperm2f128 $0x30, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x31: asm volatile("vperm2f128 $0x31, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x32: asm volatile("vperm2f128 $0x32, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x33: asm volatile("vperm2f128 $0x33, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x08: asm volatile("vperm2f128 $0x08, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x18: asm volatile("vperm2f128 $0x18, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x28: asm volatile("vperm2f128 $0x28, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x38: asm volatile("vperm2f128 $0x38, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x80: asm volatile("vperm2f128 $0x80, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x81: asm volatile("vperm2f128 $0x81, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x82: asm volatile("vperm2f128 $0x82, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x83: asm volatile("vperm2f128 $0x83, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            case 0x88: asm volatile("vperm2f128 $0x88, %1, %2, %0" : "=x"(ymm_dst) : "x"(ymm2), "x"(ymm1)); break;
            
            default: 
                printf("Invalid immediate: 0x%02X\n", imm);
                continue;
        }
        
        _mm256_store_si256((__m256i*)dst, ymm_dst);
        
        // 比较结果
        int match = memcmp(dst, expected[i], sizeof(int32_t)*8) == 0;
        if (match) pass_count++;
        
        printf("\nTest %02d: %s\n", i+1, desc[i]);
        printf("Expected: ");
        for (int j = 0; j < 8; j++) printf("0x%x ", (uint32_t)expected[i][j]);
        printf("\nResult  : ");
        for (int j = 0; j < 8; j++) printf("0x%x ", (uint32_t)dst[j]);
        printf("\nStatus  : %s\n", match ? "PASS" : "FAIL");
    }
    
    printf("\n==== Summary: %d/25 tests passed ====\n", pass_count);
    if (pass_count == 25) {
        printf("ALL TESTS PASSED!\n");
    } else {
        printf("SOME TESTS FAILED! REVIEW OUTPUT\n");
    }
}

int main() {
    test_vperm2i128();
    return 0;
}
