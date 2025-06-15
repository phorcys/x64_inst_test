#include "avx.h"
#include <stdio.h>
#include <string.h>

static void test_vbroadcasti128() {
    printf("--- Testing vbroadcasti128 (broadcast 128-bit integer memory to 256-bit register) ---\n");

    // 测试数据
    int32_t src_i32[4] ALIGNED(16) = {1, 2, 3, 4};
    int64_t src_i64[2] ALIGNED(16) = {1, 2};
    uint8_t src_u8[16] ALIGNED(16) = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };

    // 32位整数测试
    int32_t result_i32[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcasti128 %1, %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_i32)
        : "m"(src_i32)
        : "ymm0"
    );
    printf("--- Int32 Test ---\n");
    print_int32_vec("Source", src_i32, 4);
    print_int32_vec("Result", result_i32, 8);

    // 验证32位整数结果
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (result_i32[i] != src_i32[i] || result_i32[i+4] != src_i32[i]) {
            pass = 0;
            break;
        }
    }

    // 64位整数测试
    int64_t result_i64[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcasti128 %1, %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_i64)
        : "m"(src_i64)
        : "ymm0"
    );
    printf("\n--- Int64 Test ---\n");
    print_int64_vec("Source", src_i64, 2);
    print_int64_vec("Result", result_i64, 4);

    // 验证64位整数结果
    for (int i = 0; i < 2; i++) {
        if (result_i64[i] != src_i64[i] || result_i64[i+2] != src_i64[i]) {
            pass = 0;
            break;
        }
    }

    // 边界值测试
    uint8_t result_u8[32] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcasti128 %1, %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_u8)
        : "m"(src_u8)
        : "ymm0"
    );
    printf("\n--- Boundary Test ---\n");
    printf("Expected: ");
    for (int i = 0; i < 32; i++) {
        printf("%02X ", src_u8[i % 16]);
    }
    printf("\nResult:   ");
    for (int i = 0; i < 32; i++) {
        printf("%02X ", result_u8[i]);
    }
    printf("\n");

    // 验证边界测试结果
    for (int i = 0; i < 32; i++) {
        if (result_u8[i] != src_u8[i % 16]) {
            pass = 0;
            break;
        }
    }

    if (pass) {
        printf("\nvbroadcasti128 all tests passed\n");
    } else {
        printf("\nvbroadcasti128 some tests failed\n");
    }
}

int main() {
    test_vbroadcasti128();
    return 0;
}
