#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

void test_pinsrw_reg() {
    TEST_CASE("PINSRW with register src");
    
    __m128i vec = _mm_setzero_si128();
    uint16_t src = 0xABCD;
    uint16_t expected[8] = {0};
    uint16_t result[8] = {0};
    
    // Test all positions using macros
    #define TEST_POS(pos) \
        expected[pos] = src; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "movw %2, %%ax\n\t" \
            "pinsrw $"#pos", %%eax, %%xmm0\n\t" \
            "movdqu %%xmm0, %0\n\t" \
            : "=m"(result) \
            : "m"(vec), "m"(src) \
            : "%rax", "%xmm0", "memory" \
        ); \
        printf("Position %d: ", pos); \
        for (int j = 0; j < 8; j++) { \
            printf("%04X ", result[j]); \
            if (j == pos) { \
                assert(result[j] == expected[j]); \
            } else { \
                assert(result[j] == 0); \
            } \
        } \
        printf("- PASS\n"); \
        expected[pos] = 0; \
        _mm_storeu_si128((__m128i*)result, _mm_setzero_si128())
    
    TEST_POS(0);
    TEST_POS(1);
    TEST_POS(2);
    TEST_POS(3);
    TEST_POS(4);
    TEST_POS(5);
    TEST_POS(6);
    TEST_POS(7);
    
    #undef TEST_POS
}

void test_pinsrw_mem() {
    TEST_CASE("PINSRW with memory src");
    
    __m128i vec = _mm_setzero_si128();
    uint16_t mem_src = 0x1234;
    uint16_t expected[8] = {0};
    uint16_t result[8] = {0};
    
    // Test all positions using macros
    #define TEST_POS_MEM(pos) \
        expected[pos] = mem_src; \
        __asm__ volatile ( \
            "pinsrw $"#pos", %1, %0\n\t" \
            : "+x"(vec) \
            : "m"(mem_src) \
            : "memory" \
        ); \
        _mm_storeu_si128((__m128i*)result, vec); \
        printf("Position %d: ", pos); \
        for (int j = 0; j < 8; j++) { \
            printf("%04X ", result[j]); \
            if (j == pos) { \
                assert(result[j] == expected[j]); \
            } else { \
                assert(result[j] == 0); \
            } \
        } \
        printf("- PASS\n"); \
        expected[pos] = 0; \
        vec = _mm_setzero_si128()
    
    TEST_POS_MEM(0);
    TEST_POS_MEM(1);
    TEST_POS_MEM(2);
    TEST_POS_MEM(3);
    TEST_POS_MEM(4);
    TEST_POS_MEM(5);
    TEST_POS_MEM(6);
    TEST_POS_MEM(7);
    
    #undef TEST_POS_MEM
}

int main() {
    printf("=== Testing PINSRW instruction variants ===\n");
    
    test_pinsrw_reg();
    test_pinsrw_mem();
    
    printf("\nAll PINSRW tests passed successfully!\n");
    return 0;
}
