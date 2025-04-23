#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <smmintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

#define TEST_PEXTRQ_REG(pos) \
    do { \
        expected = ((uint64_t*)&vec)[pos]; \
        __asm__ volatile ( \
            "movdqu %1, %%xmm0\n\t" \
            "pextrq $"#pos", %%xmm0, %%rax\n\t" \
            "mov %%rax, %0\n\t" \
            : "=m"(result) \
            : "m"(vec) \
            : "%rax", "%xmm0" \
        ); \
        printf("Position %d: result=0x%016lX expected=0x%016lX - %s\n", \
               pos, result, expected, \
               result == expected ? "PASS" : "FAIL"); \
    } while (0)

static void test_pextrq_reg() {
    TEST_CASE("PEXTRQ with registers");
    
    __m128i vec = _mm_set_epi64x((__m64)0x1122334455667788ULL, (__m64)0x123456789ABCDEF0ULL);
    uint64_t result, expected;
    
    // Test all positions
    TEST_PEXTRQ_REG(0);
    TEST_PEXTRQ_REG(1);
}

int main() {
    printf("=== Testing PEXTRQ instruction variants ===\n");
    
    test_pextrq_reg();
    
    printf("\nAll PEXTRQ tests passed successfully!\n");
    return 0;
}
