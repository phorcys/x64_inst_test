#include "avx.h"
#include <stdio.h>
#include <stdint.h>

#define TEST_VMPSADBW_128(imm) \
    do { \
        __m128i res; \
        asm volatile ( \
            "vmovdqa %1, %%xmm0\n\t" \
            "vmovdqa %2, %%xmm1\n\t" \
            "vmpsadbw $" #imm ", %%xmm1, %%xmm0, %%xmm0\n\t" \
            "vmovdqa %%xmm0, %0\n\t" \
            : "=m"(res) \
            : "m"(a), "m"(b) \
            : "xmm0", "xmm1" \
        ); \
        uint16_t out[8]; \
        _mm_store_si128((__m128i*)out, res); \
        printf("128-bit imm=%d: [%d,%d,%d,%d,%d,%d,%d,%d]\n", imm, \
               out[0], out[1], out[2], out[3], \
               out[4], out[5], out[6], out[7]); \
    } while(0)

void test_vmpsadbw_128() {
    __m128i a = _mm_setr_epi8(1, 2, 3, 4, 5, 6, 7, 8, 
                             9, 10, 11, 12, 13, 14, 15, 16);
    __m128i b = _mm_setr_epi8(8, 7, 6, 5, 4, 3, 2, 1,
                             16, 15, 14, 13, 12, 11, 10, 9);
    
    // Test all immediate values (0-7)
    TEST_VMPSADBW_128(0);
    TEST_VMPSADBW_128(1);
    TEST_VMPSADBW_128(2);
    TEST_VMPSADBW_128(3);
    TEST_VMPSADBW_128(4);
    TEST_VMPSADBW_128(5);
    TEST_VMPSADBW_128(6);
    TEST_VMPSADBW_128(7);
}

#define TEST_VMPSADBW_256(imm) \
    do { \
        __m256i res; \
        asm volatile ( \
            "vmovdqa %1, %%ymm0\n\t" \
            "vmovdqa %2, %%ymm1\n\t" \
            "vmpsadbw $" #imm ", %%ymm1, %%ymm0, %%ymm0\n\t" \
            "vmovdqa %%ymm0, %0\n\t" \
            : "=m"(res) \
            : "m"(a), "m"(b) \
            : "ymm0", "ymm1" \
        ); \
        uint16_t out[16]; \
        _mm256_store_si256((__m256i*)out, res); \
        printf("256-bit imm=%d: [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", imm, \
               out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], \
               out[8], out[9], out[10], out[11], out[12], out[13], out[14], out[15]); \
    } while(0)

void test_vmpsadbw_256() {
    __m256i a = _mm256_setr_epi8(
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32);
    __m256i b = _mm256_setr_epi8(
        8, 7, 6, 5, 4, 3, 2, 1, 16, 15, 14, 13, 12, 11, 10, 9,
        24, 23, 22, 21, 20, 19, 18, 17, 32, 31, 30, 29, 28, 27, 26, 25);
    
    // Test all immediate values (0-7)
    TEST_VMPSADBW_256(0);
    TEST_VMPSADBW_256(1);
    TEST_VMPSADBW_256(2);
    TEST_VMPSADBW_256(3);
    TEST_VMPSADBW_256(4);
    TEST_VMPSADBW_256(5);
    TEST_VMPSADBW_256(6);
    TEST_VMPSADBW_256(7);
}

int main() {
    printf("=== Testing vmpsadbw ===\n");
    test_vmpsadbw_128();
    test_vmpsadbw_256();
    return 0;
}
