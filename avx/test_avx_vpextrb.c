#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

int main() {
    printf("Testing VPEXTRB instruction\n");
    printf("==========================\n");

    // 初始化测试数据
    uint8_t mem_result;
    uint8_t reg_result;
    uint8_t expected;
    __m128i vec = _mm_setr_epi8(0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10);

    printf("Source vector: ");
    print_m128i_hex(vec, "test_vector");
    printf("\n");

    // 测试所有可能的索引(0-15)
    for (uint8_t idx = 0; idx < 16; idx++) {
        expected = ((uint8_t*)&vec)[idx];

        // 测试寄存器目标 - 使用switch处理不同索引
        switch(idx) {
            case 0: reg_result = _mm_extract_epi8(vec, 0); break;
            case 1: reg_result = _mm_extract_epi8(vec, 1); break;
            case 2: reg_result = _mm_extract_epi8(vec, 2); break;
            case 3: reg_result = _mm_extract_epi8(vec, 3); break;
            case 4: reg_result = _mm_extract_epi8(vec, 4); break;
            case 5: reg_result = _mm_extract_epi8(vec, 5); break;
            case 6: reg_result = _mm_extract_epi8(vec, 6); break;
            case 7: reg_result = _mm_extract_epi8(vec, 7); break;
            case 8: reg_result = _mm_extract_epi8(vec, 8); break;
            case 9: reg_result = _mm_extract_epi8(vec, 9); break;
            case 10: reg_result = _mm_extract_epi8(vec, 10); break;
            case 11: reg_result = _mm_extract_epi8(vec, 11); break;
            case 12: reg_result = _mm_extract_epi8(vec, 12); break;
            case 13: reg_result = _mm_extract_epi8(vec, 13); break;
            case 14: reg_result = _mm_extract_epi8(vec, 14); break;
            case 15: reg_result = _mm_extract_epi8(vec, 15); break;
        }

        printf("Test %d: Index=%d\n", idx+1, idx);
        printf("  Expected: 0x%02X, Result: 0x%02X\n",
               expected, reg_result);

        if (reg_result != expected) {
            printf("  [FAIL] Test failed!\n");
        } else {
            printf("  [PASS] Test passed!\n");
        }
        printf("\n");
    }

    printf("\nVPEXTRB tests completed.\n");
    return 0;
}
