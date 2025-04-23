#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

#define TEST_PINSRQ_REG(pos) \
    do { \
        expected[pos] = src; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "mov %2, %%rax\n\t" \
            "pinsrq $"#pos", %%rax, %%xmm0\n\t" \
            "movdqu %%xmm0, %0\n\t" \
            : "=m"(result) \
            : "m"(vec), "r"(src) \
            : "%rax", "%xmm0", "cc" \
        ); \
        printf("Position %d: ", pos); \
        for (int i = 0; i < 2; i++) { \
            printf("%016lX ", result[i]); \
            assert(result[i] == expected[i]); \
        } \
        printf("- PASS\n"); \
        expected[pos] = 0; \
        _mm_storeu_si128((__m128i*)result, _mm_setzero_si128()); \
    } while (0)

static void test_pinsrq_reg() {
    TEST_CASE("PINSRQ with registers");
    
    __m128i vec = _mm_setzero_si128();
    uint64_t src = 0x123456789ABCDEF0;
    uint64_t expected[2] = {0};
    uint64_t result[2] = {0};
    
    // Test all positions
    TEST_PINSRQ_REG(0);
    TEST_PINSRQ_REG(1);
}

int main() {
    printf("=== Testing PINSRQ instruction variants ===\n");
    
    test_pinsrq_reg();
    
    printf("\nAll PINSRQ tests passed successfully!\n");
    return 0;
}
