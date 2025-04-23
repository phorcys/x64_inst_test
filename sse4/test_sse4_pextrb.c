#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

#define TEST_PEXTRB_REG(pos) \
    expected = ((uint8_t*)&vec)[pos]; \
    asm volatile ( \
        "movdqu %1, %%xmm0\n\t" \
        "pextrb $"#pos", %%xmm0, %%eax\n\t" \
        "mov %%al, %0\n\t" \
        : "=r"(result) \
        : "m"(vec) \
        : "%eax", "%xmm0" \
    ); \
    printf("Position %d: result=0x%02X, expected=0x%02X - %s\n", \
           pos, result, expected, \
           result == expected ? "PASS" : "FAIL"); \
    assert(result == expected)

#define TEST_PEXTRB_MEM(pos) \
    expected = ((uint8_t*)&vec)[pos]; \
    asm volatile ( \
        "movdqu %1, %%xmm0\n\t" \
        "pextrb $"#pos", %%xmm0, %0\n\t" \
        : "=m"(mem_result) \
        : "m"(vec) \
        : "%xmm0" \
    ); \
    printf("Position %d: result=0x%02X, expected=0x%02X - %s\n", \
           pos, mem_result, expected, \
           mem_result == expected ? "PASS" : "FAIL"); \
    assert(mem_result == expected)

static void test_pextrb_reg() {
    TEST_CASE("PEXTRB with registers");
    
    __m128i vec = _mm_setr_epi8(0x12, 0x34, 0x56, 0x78, 
                                0x9A, 0xBC, 0xDE, 0xF0,
                                0x11, 0x22, 0x33, 0x44,
                                0x55, 0x66, 0x77, 0x88);
    uint8_t result, expected;
    
    // Test all positions
    TEST_PEXTRB_REG(0);
    TEST_PEXTRB_REG(1);
    TEST_PEXTRB_REG(2);
    TEST_PEXTRB_REG(3);
    TEST_PEXTRB_REG(4);
    TEST_PEXTRB_REG(5);
    TEST_PEXTRB_REG(6);
    TEST_PEXTRB_REG(7);
    TEST_PEXTRB_REG(8);
    TEST_PEXTRB_REG(9);
    TEST_PEXTRB_REG(10);
    TEST_PEXTRB_REG(11);
    TEST_PEXTRB_REG(12);
    TEST_PEXTRB_REG(13);
    TEST_PEXTRB_REG(14);
    TEST_PEXTRB_REG(15);
}

static void test_pextrb_mem() {
    TEST_CASE("PEXTRB with memory dst");
    
    __m128i vec = _mm_setr_epi8(0xA1, 0xB1, 0xC2, 0xD2,
                                0xE3, 0xF3, 0x44, 0x55,
                                0x66, 0x77, 0x88, 0x99,
                                0xAA, 0xBB, 0xCC, 0xDD);
    uint8_t mem_result, expected;
    
    // Test all positions
    TEST_PEXTRB_MEM(0);
    TEST_PEXTRB_MEM(1);
    TEST_PEXTRB_MEM(2);
    TEST_PEXTRB_MEM(3);
    TEST_PEXTRB_MEM(4);
    TEST_PEXTRB_MEM(5);
    TEST_PEXTRB_MEM(6);
    TEST_PEXTRB_MEM(7);
    TEST_PEXTRB_MEM(8);
    TEST_PEXTRB_MEM(9);
    TEST_PEXTRB_MEM(10);
    TEST_PEXTRB_MEM(11);
    TEST_PEXTRB_MEM(12);
    TEST_PEXTRB_MEM(13);
    TEST_PEXTRB_MEM(14);
    TEST_PEXTRB_MEM(15);
}

int main() {
    printf("=== Testing PEXTRB instruction variants ===\n");
    
    test_pextrb_reg();
    test_pextrb_mem();
   
    printf("\nAll PEXTRB tests passed successfully!\n");
    return 0;
}
