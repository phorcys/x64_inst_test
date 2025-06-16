#include "avx.h"
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// 测试vmovddup指令
int test_vmovddup() {
    int ret = 0;
    printf("===== Test vmovddup instruction =====\n");

    // 测试128位版本
    {
        double src[2] ALIGNED(16) = {1.23456789, -2.34567890};
        double dst[2] ALIGNED(16) = {0};
        double expected[2] = {src[0], src[0]};

        printf("\n--- Testing 128-bit vmovddup ---\n");
        print_double_vec("Source", src, 2);
        print_double_vec("Expected", expected, 2);

        __m128d src_reg, dst_reg;
        memcpy(&src_reg, src, sizeof(src_reg));
        __asm__ __volatile__(
            "vmovddup %1, %0\n\t"
            : "=x"(dst_reg)
            : "x"(src_reg)
        );
        memcpy(dst, &dst_reg, sizeof(dst_reg));

        print_double_vec("Result", dst, 2);

        if(memcmp(dst, expected, sizeof(dst)) != 0) {
            printf("FAIL: 128-bit vmovddup result mismatch\n");
            ret = 1;
        } else {
            printf("PASS: 128-bit vmovddup\n");
        }
    }

    // 测试256位版本
    {
        double src[4] ALIGNED(32) = {1.23456789, -2.34567890, 3.45678901, -4.56789012};
        double dst[4] ALIGNED(32) = {0};
        double expected[4] = {src[0], src[0], src[2], src[2]};

        printf("\n--- Testing 256-bit vmovddup ---\n");
        print_double_vec("Source", src, 4);
        print_double_vec("Expected", expected, 4);

        __m256d src_reg, dst_reg;
        memcpy(&src_reg, src, sizeof(src_reg));
        __asm__ __volatile__(
            "vmovddup %1, %0\n\t"
            : "=x"(dst_reg)
            : "x"(src_reg)
        );
        memcpy(dst, &dst_reg, sizeof(dst_reg));

        print_double_vec("Result", dst, 4);

        if(memcmp(dst, expected, sizeof(dst)) != 0) {
            printf("FAIL: 256-bit vmovddup result mismatch\n");
            ret = 1;
        } else {
            printf("PASS: 256-bit vmovddup\n");
        }
    }

    // 测试特殊值(NaN, 0, 最大/最小值)
    {
        double special_values[4] ALIGNED(32) = {
            NAN, 0.0, 
            -DBL_MAX, DBL_MAX
        };
        double dst[4] ALIGNED(32) = {0};
        double expected[4] = {
            special_values[0], special_values[0],
            special_values[2], special_values[2]
        };

        printf("\n--- Testing special values ---\n");
        print_double_vec_hex("Source", special_values, 4);
        print_double_vec_hex("Expected", expected, 4);

        __m256d src_reg, dst_reg;
        memcpy(&src_reg, special_values, sizeof(src_reg));
        __asm__ __volatile__(
            "vmovddup %1, %0\n\t"
            : "=x"(dst_reg)
            : "x"(src_reg)
        );
        memcpy(dst, &dst_reg, sizeof(dst_reg));

        print_double_vec_hex("Result", dst, 4);

        if(memcmp(dst, expected, sizeof(dst)) != 0) {
            printf("FAIL: Special values test failed\n");
            ret = 1;
        } else {
            printf("PASS: Special values test\n");
        }
    }

    return ret;
}

int main() {
    return test_vmovddup();
}
