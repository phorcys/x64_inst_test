#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

#define TEST_PEXTRD_REG(pos) \
    do { \
        expected = ((uint32_t*)&vec)[pos]; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "pextrd $"#pos", %%xmm0, %%eax\n\t" \
            "mov %%eax, %0\n\t" \
            : "=m"(result) \
            : "m"(vec) \
            : "%eax", "%xmm0" \
        ); \
        printf("Position %d: result=0x%08X expected=0x%08X - %s\n", \
               pos, result, expected, \
               result == expected ? "PASS" : "FAIL"); \
    } while (0)

#define TEST_PEXTRD_MEM(pos) \
    do { \
        expected = ((uint32_t*)&vec)[pos]; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "pextrd $"#pos", %%xmm0, %0\n\t" \
            : "=m"(mem_result) \
            : "m"(vec) \
            : "%xmm0" \
        ); \
        printf("Position %d: result=0x%08X expected=0x%08X - %s\n", \
               pos, mem_result, expected, \
               mem_result == expected ? "PASS" : "FAIL"); \
    } while (0)

static void test_pextrd_reg() {
    TEST_CASE("PEXTRD with registers");
    
    __m128i vec = _mm_setr_epi32(0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788);
    uint32_t result, expected;
    
    // Test all positions
    TEST_PEXTRD_REG(0);
    TEST_PEXTRD_REG(1);
    TEST_PEXTRD_REG(2);
    TEST_PEXTRD_REG(3);
}

static void test_pextrd_mem() {
    TEST_CASE("PEXTRD with memory dst");
    
    __m128i vec = _mm_setr_epi32(0xA1B2C3D4, 0xE5F60718, 0x91A2B3C4, 0xD5E6F708);
    uint32_t mem_result, expected;
    
    // Test all positions
    TEST_PEXTRD_MEM(0);
    TEST_PEXTRD_MEM(1);
    TEST_PEXTRD_MEM(2);
    TEST_PEXTRD_MEM(3);
}

int main() {
    printf("=== Testing PEXTRD instruction variants ===\n");
    
    test_pextrd_reg();
    test_pextrd_mem();
    
    printf("\nAll PEXTRD tests passed successfully!\n");
    return 0;
}
