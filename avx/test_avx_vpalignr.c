#include "avx.h"
#include <stdio.h>
#include <string.h>

static void test_vpalignr() {
    printf("--- Testing vpalignr (packed byte alignment) ---\n");

    // 测试数据
    uint8_t src1[16] ALIGNED(16) = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    uint8_t src2[16] ALIGNED(16) = {
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    // 测试不同偏移量
    #define TEST_OFFSET(offset) \
    do { \
        uint8_t result[16] ALIGNED(16) = {0}; \
        __asm__ __volatile__( \
            "vmovdqa %1, %%xmm0\n\t" \
            "vmovdqa %2, %%xmm1\n\t" \
            "vpalignr $"#offset", %%xmm1, %%xmm0, %%xmm2\n\t" \
            "vmovdqa %%xmm2, %0\n\t" \
            : "=m"(result) \
            : "m"(src1), "m"(src2) \
            : "xmm0", "xmm1", "xmm2" \
        ); \
        printf("\n--- Offset %d Test ---\n", offset); \
        printf("Src1: "); \
        for (int i = 0; i < 16; i++) printf("%02X ", src1[i]); \
        printf("\nSrc2: "); \
        for (int i = 0; i < 16; i++) printf("%02X ", src2[i]); \
        printf("\nResult: "); \
        for (int i = 0; i < 16; i++) printf("%02X ", result[i]); \
        printf("\n"); \
    } while(0)

    TEST_OFFSET(0);
    TEST_OFFSET(8);
    TEST_OFFSET(16);
    TEST_OFFSET(24);
    TEST_OFFSET(32);
}

int main() {
    test_vpalignr();
    return 0;
}
