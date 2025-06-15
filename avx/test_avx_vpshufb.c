#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vpshufb_128() {
    printf("=== Testing vpshufb (128-bit) ===\n");
    
    uint8_t a[16] ALIGNED(16) = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    uint8_t mask[16] ALIGNED(16) = {
        0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06,
        0x09, 0x08, 0x0B, 0x0A, 0x0D, 0x0C, 0x0F, 0x0E
    };
    uint8_t dst[16] ALIGNED(16) = {0};
    uint8_t expected[16] ALIGNED(16) = {0};
    
    // Test case 1: Swap adjacent bytes
    __m128i v_a = _mm_load_si128((__m128i*)a);
    __m128i v_mask = _mm_load_si128((__m128i*)mask);
    __m128i v_dst;
    asm volatile(
        "vpshufb %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_mask)
    );
    _mm_store_si128((__m128i*)dst, v_dst);
    for (int i = 0; i < 16; i += 2) {
        expected[i] = a[i+1];
        expected[i+1] = a[i];
    }
    
    printf("Result: ");
    for (int i = 0; i < 16; i++) printf("%02X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 16; i++) printf("%02X ", expected[i]);
    printf("\nTest 1: %s\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL");

    // Test case 2: Special mask values (high bit set)
    // Reset entire mask array
    for (int i = 0; i < 16; i++) mask[i] = 0x80; // Set high bit for all
    mask[2] = 0x0A; // Only this index should be used
    
    v_mask = _mm_load_si128((__m128i*)mask);
    asm volatile(
        "vpshufb %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_mask)
    );
    _mm_store_si128((__m128i*)dst, v_dst);
    
    expected[0] = 0; expected[1] = 0; expected[2] = a[10];
    for (int i = 3; i < 16; i++) expected[i] = 0;
    
    printf("Result: ");
    for (int i = 0; i < 16; i++) printf("%02X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 16; i++) printf("%02X ", expected[i]);
    printf("\nTest 2: %s\n", memcmp(dst, expected, 16) == 0 ? "PASS" : "FAIL");
}

static void test_vpshufb_256() {
    printf("\n=== Testing vpshufb (256-bit) ===\n");
    
    uint8_t a[32] ALIGNED(32) = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    uint8_t mask[32] ALIGNED(32) = {
        0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18, // Reverse order for low lane
        0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
        0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, // Reverse order for high lane
        0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
    };
    uint8_t dst[32] ALIGNED(32) = {0};
    uint8_t expected[32] ALIGNED(32) = {0};
    
    // Test case 1: Reverse each 16-byte lane
    __m256i v_a = _mm256_load_si256((__m256i*)a);
    __m256i v_mask = _mm256_load_si256((__m256i*)mask);
    __m256i v_dst;
    asm volatile(
        "vpshufb %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_mask)
    );
    _mm256_store_si256((__m256i*)dst, v_dst);
    
    // Low lane reversed
    for (int i = 0; i < 16; i++) expected[i] = a[15 - i];
    // High lane reversed
    for (int i = 16; i < 32; i++) expected[i] = a[47 - i];
    
    printf("Result: ");
    for (int i = 0; i < 32; i++) printf("%02X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 32; i++) printf("%02X ", expected[i]);
    printf("\nTest 1: %s\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL");

    // Test case 2: Boundary and special values
    for (int i = 0; i < 32; i++) {
        mask[i] = (i < 16) ? (0x80 | i) : (i - 16); // High bit set for low lane
    }
    
    v_mask = _mm256_load_si256((__m256i*)mask);
    asm volatile(
        "vpshufb %2, %1, %0"
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_mask)
    );
    _mm256_store_si256((__m256i*)dst, v_dst);
    
    // Low lane should be all zeros, high lane: use indices from mask relative to high lane
    for (int i = 0; i < 16; i++) expected[i] = 0;
    for (int i = 16; i < 32; i++) expected[i] = a[16 + mask[i]]; // Start from high lane base
    
    printf("Result: ");
    for (int i = 0; i < 32; i++) printf("%02X ", dst[i]);
    printf("\nExpected: ");
    for (int i = 0; i < 32; i++) printf("%02X ", expected[i]);
    printf("\nTest 2: %s\n", memcmp(dst, expected, 32) == 0 ? "PASS" : "FAIL");
}

int main() {
    test_vpshufb_128();
    test_vpshufb_256();
    return 0;
}
