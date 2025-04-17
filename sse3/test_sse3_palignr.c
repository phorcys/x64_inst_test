#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>

// MMX version test
void test_mmx_palignr() {
    printf("=== Testing MMX PALIGNR ===\n");
    
    // Test data
    uint8_t data1[8] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};
    uint8_t data2[8] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28};
    
    __m64 a = *(__m64*)data1;
    __m64 b = *(__m64*)data2;
    
    // Test MMX alignments with macro
    #define TEST_MMX_PALIGNR(shift) \
        do { \
            __m64 res = _mm_alignr_pi8(a, b, shift); \
            uint8_t result[8]; \
            *(__m64*)result = res; \
            printf("Shift: %d\n", shift); \
            printf("Expected: "); \
            for (int i = 0; i < 8; i++) { \
                int idx = i + shift; \
                uint8_t val = (idx < 8) ? data2[idx] : (idx < 16) ? data1[idx-8] : 0; \
                printf("%02x ", val); \
            } \
            printf("\nResult:   "); \
            for (int i = 0; i < 8; i++) { \
                printf("%02x ", result[i]); \
            } \
            printf("\n\n"); \
        } while(0)

    TEST_MMX_PALIGNR(0);
    TEST_MMX_PALIGNR(1);
    TEST_MMX_PALIGNR(7);
    TEST_MMX_PALIGNR(8);
    TEST_MMX_PALIGNR(15);
    TEST_MMX_PALIGNR(16);
    TEST_MMX_PALIGNR(31);

    #undef TEST_MMX_PALIGNR
    
    _mm_empty();
}

// SSE version test
void test_sse_palignr() {
    printf("\n=== Testing SSE PALIGNR ===\n");
    
    // Test data
    uint8_t data1[16] = {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
                         0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x10};
    uint8_t data2[16] = {0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
                         0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x20};
    
    __m128i a = _mm_loadu_si128((__m128i*)data1);
    __m128i b = _mm_loadu_si128((__m128i*)data2);
    
    // Test SSE alignments with macro
    #define TEST_SSE_PALIGNR(shift) \
        do { \
            __m128i res = _mm_alignr_epi8(a, b, shift); \
            uint8_t result[16]; \
            _mm_storeu_si128((__m128i*)result, res); \
            printf("Shift: %d\n", shift); \
            printf("Expected: "); \
            for (int i = 0; i < 16; i++) { \
                int idx = i + shift; \
                uint8_t val = (idx < 16) ? data2[idx] : data1[idx-16]; \
                printf("%02x ", val); \
            } \
            printf("\nResult:   "); \
            for (int i = 0; i < 16; i++) { \
                printf("%02x ", result[i]); \
            } \
            printf("\n\n"); \
        } while(0)

    TEST_SSE_PALIGNR(0);
    TEST_SSE_PALIGNR(1);
    TEST_SSE_PALIGNR(7);
    TEST_SSE_PALIGNR(8);
    TEST_SSE_PALIGNR(15);
    TEST_SSE_PALIGNR(16);
    TEST_SSE_PALIGNR(31);
    TEST_SSE_PALIGNR(32);
    TEST_SSE_PALIGNR(63);

    #undef TEST_SSE_PALIGNR
}

// Test different data types (8/16/32/64 bit)
void test_data_types() {
    printf("\n=== Testing Different Data Types ===\n");
    
    // 16-bit data
    {
        uint16_t data1[8] = {0x1111,0x1212,0x1313,0x1414,0x1515,0x1616,0x1717,0x1818};
        uint16_t data2[8] = {0x2121,0x2222,0x2323,0x2424,0x2525,0x2626,0x2727,0x2828};
        
        __m128i a = _mm_loadu_si128((__m128i*)data1);
        __m128i b = _mm_loadu_si128((__m128i*)data2);
        
        __m128i res = _mm_alignr_epi8(a, b, 4); // Shift by 4 bytes (2 elements)
        uint16_t result[8];
        _mm_storeu_si128((__m128i*)result, res);
        
        printf("16-bit data (shift 4 bytes):\n");
        printf("Expected: 0x2323 0x2424 0x2525 0x2626 0x2727 0x2828 0x1111 0x1212\n");
        printf("Result:   ");
        for (int i = 0; i < 8; i++) {
            printf("0x%04x ", result[i]);
        }
        printf("\n\n");
    }
    
    // 32-bit data
    {
        uint32_t data1[4] = {0x11111111,0x12121212,0x13131313,0x14141414};
        uint32_t data2[4] = {0x21212121,0x22222222,0x23232323,0x24242424};
        
        __m128i a = _mm_loadu_si128((__m128i*)data1);
        __m128i b = _mm_loadu_si128((__m128i*)data2);
        
        __m128i res = _mm_alignr_epi8(a, b, 8); // Shift by 8 bytes (2 elements)
        uint32_t result[4];
        _mm_storeu_si128((__m128i*)result, res);
        
        printf("32-bit data (shift 8 bytes):\n");
        printf("Expected: 0x23232323 0x24242424 0x11111111 0x12121212\n");
        printf("Result:   ");
        for (int i = 0; i < 4; i++) {
            printf("0x%08x ", result[i]);
        }
        printf("\n\n");
    }
    
    // 64-bit data
    {
        uint64_t data1[2] = {0x1111111111111111,0x1212121212121212};
        uint64_t data2[2] = {0x2121212121212121,0x2222222222222222};
        
        __m128i a = _mm_loadu_si128((__m128i*)data1);
        __m128i b = _mm_loadu_si128((__m128i*)data2);
        
        __m128i res = _mm_alignr_epi8(a, b, 8); // Shift by 8 bytes (1 element)
        uint64_t result[2];
        _mm_storeu_si128((__m128i*)result, res);
        
        printf("64-bit data (shift 8 bytes):\n");
        printf("Expected: 0x2222222222222222 0x1111111111111111\n");
        printf("Result:   ");
        for (int i = 0; i < 2; i++) {
            printf("0x%016lx ", result[i]);
        }
        printf("\n\n");
    }
}

int main() {
    test_mmx_palignr();
    test_sse_palignr();
    test_data_types();
    
    printf("=== PALIGNR Tests Completed ===\n");
    return 0;
}
