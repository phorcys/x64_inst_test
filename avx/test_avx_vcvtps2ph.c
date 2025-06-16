#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// 内联汇编实现vcvtps2ph指令 - 128位版本
#define TEST_VCVTPS2PH_128(imm) \
static void test_vcvtps2ph_128_##imm(float *src, uint16_t *dst) { \
    __asm__ __volatile__( \
        "vcvtps2ph $"#imm", %1, %0" \
        : "=m" (*dst) \
        : "x" (*(__m128*)src) \
    ); \
}

// 内联汇编实现vcvtps2ph指令 - 256位版本
#define TEST_VCVTPS2PH_256(imm) \
static void test_vcvtps2ph_256_##imm(float *src, uint16_t *dst) { \
    __asm__ __volatile__( \
        "vcvtps2ph $"#imm", %1, %0" \
        : "=m" (*dst) \
        : "x" (*(__m256*)src) \
    ); \
}

// 生成4种舍入模式的测试函数
TEST_VCVTPS2PH_128(0)  // 就近舍入
TEST_VCVTPS2PH_128(1)  // 向下舍入
TEST_VCVTPS2PH_128(2)  // 向上舍入
TEST_VCVTPS2PH_128(3)  // 截断

TEST_VCVTPS2PH_256(0)
TEST_VCVTPS2PH_256(1)
TEST_VCVTPS2PH_256(2)
TEST_VCVTPS2PH_256(3)

// 测试用例
static const float test_cases[] = {
    0.0f, -0.0f,
    1.0f, -1.0f,
    123.456f, -123.456f,
    INFINITY, -INFINITY,
    NAN, -NAN,
    1.0e-38f, -1.0e-38f,  // 接近下界的值
    3.4028235e38f, -3.4028235e38f  // 接近上界的值
};

// 打印半精度浮点向量
static void print_half_vec(const char* name, uint16_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%04x ", vec[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing vcvtps2ph instruction\n");
    printf("---------------------------\n");

    // 测试所有4种舍入模式
    const char* rounding_modes[] = {
        "Round to nearest", 
        "Round down", 
        "Round up", 
        "Truncate"
    };
    const int imm_values[] = {0, 1, 2, 3};

    for (int r = 0; r < 4; r++) {
        printf("\nRounding mode: %s (imm=%d)\n", rounding_modes[r], imm_values[r]);

        // 测试128位版本
        printf("\nTesting 128-bit version:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 4) {
            float src[4] ALIGNED(32) = {test_cases[i], test_cases[i+1], test_cases[i+2], test_cases[i+3]};
            uint16_t dst[8] ALIGNED(32) = {0};
            
            switch(imm_values[r]) {
                case 0: test_vcvtps2ph_128_0(src, dst); break;
                case 1: test_vcvtps2ph_128_1(src, dst); break;
                case 2: test_vcvtps2ph_128_2(src, dst); break;
                case 3: test_vcvtps2ph_128_3(src, dst); break;
            }
            
            printf("Input:  ");
            print_float_vec("", src, 4);
            printf("Output: ");
            print_half_vec("", dst, 4);
            printf("\n");
        }

        // 测试256位版本
        printf("\nTesting 256-bit version:\n");
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i += 8) {
            float src[8] ALIGNED(32) = {test_cases[i], test_cases[i+1], test_cases[i+2], test_cases[i+3],
                                      test_cases[i+4], test_cases[i+5], test_cases[i+6], test_cases[i+7]};
            uint16_t dst[16] ALIGNED(32) = {0};
            
            switch(imm_values[r]) {
                case 0: test_vcvtps2ph_256_0(src, dst); break;
                case 1: test_vcvtps2ph_256_1(src, dst); break;
                case 2: test_vcvtps2ph_256_2(src, dst); break;
                case 3: test_vcvtps2ph_256_3(src, dst); break;
            }
            
            printf("Input:  ");
            print_float_vec("", src, 8);
            printf("Output: ");
            print_half_vec("", dst, 8);
            printf("\n");
        }
    }

    printf("Tests completed\n");
    return 0;
}
