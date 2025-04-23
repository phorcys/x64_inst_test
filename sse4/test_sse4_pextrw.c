#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

void test_pextrw_reg() {
    TEST_CASE("PEXTRW with XMM registers");
    
    __m128i vec = _mm_setr_epi16(0x1234, 0x5678, 0x9ABC, 0xDEF0,
                                 0x1122, 0x3344, 0x5566, 0x7788);
    uint16_t result, expected;
    
    // Test all positions using macros
    #define TEST_POS(pos) \
        result = _mm_extract_epi16(vec, pos); \
        expected = ((uint16_t*)&vec)[pos]; \
        printf("Position %d: result=0x%04X, expected=0x%04X - %s\n", \
               pos, result, expected, \
               result == expected ? "PASS" : "FAIL"); \
        assert(result == expected)
    
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

void test_pextrw_mem() {
    TEST_CASE("PEXTRW with memory dst");
    
    __m128i vec = _mm_setr_epi16(0xA1B1, 0xC2D2, 0xE3F3, 0x4455,
                                 0x6677, 0x8899, 0xAABB, 0xCCDD);
    uint16_t mem_result, expected;
    
    // Test all positions using macros
    #define TEST_POS(pos) \
        asm volatile ( "pextrw $" #pos ", %1, %0\n\t" : "=m" (mem_result): "x" (vec): ); \
        expected = ((uint16_t*)&vec)[pos]; \
        printf("Position %d: result=0x%04X, expected=0x%04X - %s\n", \
               pos, mem_result, expected, \
               mem_result == expected ? "PASS" : "FAIL"); \
        assert(mem_result == expected)
    
    TEST_POS(0);
    TEST_POS(1);
    TEST_POS(2); 
    TEST_POS(3);
    TEST_POS(4);
    TEST_POS(5);
    TEST_POS(6);
    TEST_POS(7);
}

int main() {
    printf("=== Testing PEXTRW instruction variants ===\n");
    
    test_pextrw_reg();
    test_pextrw_mem();
   
    printf("\nAll PEXTRW tests passed successfully!\n");
    return 0;
}
