#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试vperm2i128指令
int test_vperm2i128() {
    int ret = 0;
    printf("===== Testing vperm2i128 =====\n");

    // 测试数据
    ALIGNED(32) uint8_t src1[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    
    ALIGNED(32) uint8_t src2[32] = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
    };

    ALIGNED(32) uint8_t dst[32];
    ALIGNED(32) uint8_t expected[32];

    __m256i ymm1, ymm2, ymm_res;

    // 加载测试数据到寄存器
    ymm1 = _mm256_loadu_si256((__m256i*)src1);
    ymm2 = _mm256_loadu_si256((__m256i*)src2);

    // 定义测试宏
    #define TEST_VPERM2I128(imm) do { \
        printf("\n=== Test case: imm8=0x%02X ===\n", imm); \
        __asm__ __volatile__( \
            "vperm2i128 $"#imm", %2, %1, %0\n\t" \
            : "=x"(ymm_res) \
            : "x"(ymm1), "x"(ymm2) \
        ); \
        int lane1_src = ((imm) >> 0) & 3; \
        int lane2_src = ((imm) >> 4) & 3; \
        memset(expected, 0, 32); \
        /* 低128位 */ \
        if(lane1_src == 0) { \
            memcpy(expected, src1, 16); \
        } else if(lane1_src == 1) { \
            memcpy(expected, src1+16, 16); \
        } else if(lane1_src == 2) { \
            memcpy(expected, src2, 16); \
        } else { \
            memcpy(expected, src2+16, 16); \
        } \
        /* 高128位 */ \
        if(lane2_src == 0) { \
            memcpy(expected+16, src1, 16); \
        } else if(lane2_src == 1) { \
            memcpy(expected+16, src1+16, 16); \
        } else if(lane2_src == 2) { \
            memcpy(expected+16, src2, 16); \
        } else { \
            memcpy(expected+16, src2+16, 16); \
        } \
        /* 处理零填充 */ \
        if(imm & 0x08) memset(expected, 0, 16); \
        if(imm & 0x80) memset(expected+16, 0, 16); \
        _mm256_storeu_si256((__m256i*)dst, ymm_res); \
        print_ymm("Result", ymm_res); \
        print_ymm("Expected", _mm256_loadu_si256((__m256i*)expected)); \
        if(!cmp_ymm(ymm_res, _mm256_loadu_si256((__m256i*)expected))) { \
            printf("Test failed for imm8=0x%02X\n", imm); \
            ret = 1; \
        } else { \
            printf("Test passed\n"); \
        } \
    } while(0)

    // 测试所有16种立即数组合
    TEST_VPERM2I128(0x00);
    TEST_VPERM2I128(0x01);
    TEST_VPERM2I128(0x02);
    TEST_VPERM2I128(0x03);
    TEST_VPERM2I128(0x04);
    TEST_VPERM2I128(0x05);
    TEST_VPERM2I128(0x06);
    TEST_VPERM2I128(0x07);
    TEST_VPERM2I128(0x08);
    TEST_VPERM2I128(0x09);
    TEST_VPERM2I128(0x0A);
    TEST_VPERM2I128(0x0B);
    TEST_VPERM2I128(0x0C);
    TEST_VPERM2I128(0x0D);
    TEST_VPERM2I128(0x0E);
    TEST_VPERM2I128(0x0F);
    TEST_VPERM2I128(0x10);
    TEST_VPERM2I128(0x20);
    TEST_VPERM2I128(0x30);
    TEST_VPERM2I128(0x80);
    TEST_VPERM2I128(0x88);
    #undef TEST_VPERM2I128

    // 测试内存操作数
    printf("\n=== Testing memory operand ===\n");
    __asm__ __volatile__(
        "vperm2i128 $0x02, %2, %1, %0\n\t"
        : "=x"(ymm_res)
        : "x"(ymm1), "m"(*((__m256i*)src2))
    );

    // 预期结果: 低128位=src2低128位, 高128位=src1低128位
    memcpy(expected, src2, 16);
    memcpy(expected+16, src1, 16);

    _mm256_storeu_si256((__m256i*)dst, ymm_res);
    print_ymm("Memory operand result", ymm_res);
    print_ymm("Expected", _mm256_loadu_si256((__m256i*)expected));

    if(!cmp_ymm(ymm_res, _mm256_loadu_si256((__m256i*)expected))) {
        printf("Memory operand test failed\n");
        ret = 1;
    } else {
        printf("Memory operand test passed\n");
    }

    return ret;
}

int main() {
    return test_vperm2i128();
}
