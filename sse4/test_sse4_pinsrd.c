#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

#define TEST_PINSRD_REG(pos) \
    do { \
        expected[pos] = src; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "mov %2, %%eax\n\t" \
            "pinsrd $"#pos", %%eax, %%xmm0\n\t" \
            "movdqu %%xmm0, %0\n\t" \
            : "=m"(result) \
            : "m"(vec), "r"(src) \
            : "%eax", "%xmm0", "cc" \
        ); \
        printf("Position %d: ", pos); \
        for (int i = 0; i < 4; i++) { \
            printf("%08X ", result[i]); \
            assert(result[i] == expected[i]); \
        } \
        printf("- PASS\n"); \
        expected[pos] = 0; \
        _mm_storeu_si128((__m128i*)result, _mm_setzero_si128()); \
    } while (0)

static void test_pinsrd_reg() {
    TEST_CASE("PINSRD with registers");
    
    __m128i vec = _mm_setzero_si128();
    uint32_t src = 0x12345678;
    uint32_t expected[4] = {0};
    uint32_t result[4] = {0};
    
    // Test all positions with macro expansion
    TEST_PINSRD_REG(0);
    TEST_PINSRD_REG(1);
    TEST_PINSRD_REG(2);
    TEST_PINSRD_REG(3);
}

#define TEST_PINSRD_MEM(pos) \
    do { \
        expected[pos] = src; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "pinsrd $"#pos", %2, %%xmm0\n\t" \
            "movdqu %%xmm0, %0\n\t" \
            : "=m"(result) \
            : "m"(vec), "m"(src) \
            : "%xmm0" \
        ); \
        printf("Position %d: ", pos); \
        for (int i = 0; i < 4; i++) { \
            printf("%08X ", result[i]); \
            assert(result[i] == expected[i]); \
        } \
        printf("- PASS\n"); \
        expected[pos] = 0; \
        _mm_storeu_si128((__m128i*)result, _mm_setzero_si128()); \
    } while (0)

static void test_pinsrd_mem() {
    TEST_CASE("PINSRD with memory src");
    
    __m128i vec = _mm_setzero_si128();
    uint32_t src = 0x87654321;
    uint32_t expected[4] = {0};
    uint32_t result[4] = {0};
    
    // Test all positions with macro expansion
    TEST_PINSRD_MEM(0);
    TEST_PINSRD_MEM(1);
    TEST_PINSRD_MEM(2);
    TEST_PINSRD_MEM(3);
}

int main() {
    printf("=== Testing PINSRD instruction variants ===\n");
    
    test_pinsrd_reg();
    test_pinsrd_mem();
    
    printf("\nAll PINSRD tests passed successfully!\n");
    return 0;
}
