#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

void test_pinsrb_reg() {
    TEST_CASE("PINSRB with XMM registers");
    
    __m128i vec = _mm_setzero_si128();
    uint8_t values[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
                        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    
    // Test inserting into all positions using macros
    #define TEST_POS(pos) \
        do { \
            __m128i result = _mm_insert_epi8(vec, values[pos], pos); \
            uint8_t inserted = ((uint8_t*)&result)[pos]; \
            printf("Position %2d: inserted=0x%02X, expected=0x%02X - %s\n", \
                   pos, inserted, values[pos], \
                   inserted == values[pos] ? "PASS" : "FAIL"); \
            assert(inserted == values[pos]); \
        } while(0)
    
    TEST_POS(0);
    TEST_POS(1);
    TEST_POS(2);
    TEST_POS(3);
    TEST_POS(4);
    TEST_POS(5);
    TEST_POS(6);
    TEST_POS(7);
    TEST_POS(8);
    TEST_POS(9);
    TEST_POS(10);
    TEST_POS(11);
    TEST_POS(12);
    TEST_POS(13);
    TEST_POS(14);
    TEST_POS(15);
    
    #undef TEST_POS
}

void test_pinsrb_mem() {
    TEST_CASE("PINSRB with memory operand");
    
    __m128i vec = _mm_setzero_si128();
    uint8_t mem_values[] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x07, 0x18,
                            0x29, 0x3A, 0x4B, 0x5C, 0x6D, 0x7E, 0x8F, 0x90};
    
    #define TEST_MEM_POS(pos) \
        do { \
            __m128i result = _mm_insert_epi8(vec, mem_values[pos], pos); \
            uint8_t inserted = ((uint8_t*)&result)[pos]; \
            printf("Mem pos %2d: inserted=0x%02X, expected=0x%02X - %s\n", \
                   pos, inserted, mem_values[pos], \
                   inserted == mem_values[pos] ? "PASS" : "FAIL"); \
            assert(inserted == mem_values[pos]); \
        } while(0)
    
    TEST_MEM_POS(0);
    TEST_MEM_POS(1);
    TEST_MEM_POS(2);
    TEST_MEM_POS(3);
    TEST_MEM_POS(4);
    TEST_MEM_POS(5);
    TEST_MEM_POS(6);
    TEST_MEM_POS(7);
    TEST_MEM_POS(8);
    TEST_MEM_POS(9);
    TEST_MEM_POS(10);
    TEST_MEM_POS(11);
    TEST_MEM_POS(12);
    TEST_MEM_POS(13);
    TEST_MEM_POS(14);
    TEST_MEM_POS(15);
    
    #undef TEST_MEM_POS
}

void test_pinsrb_boundary() {
    TEST_CASE("PINSRB boundary cases");
    
    __m128i vec = _mm_set1_epi8(0xFF);
    uint8_t test_val = 0x55;
    
    // Test first and last positions
    __m128i first = _mm_insert_epi8(vec, test_val, 0);
    __m128i last = _mm_insert_epi8(vec, test_val, 15);
    
    printf("First pos: result=0x%02X, expected=0x55 - %s\n",
           ((uint8_t*)&first)[0], 
           ((uint8_t*)&first)[0] == test_val ? "PASS" : "FAIL");
    printf("Last pos: result=0x%02X, expected=0x55 - %s\n",
           ((uint8_t*)&last)[15],
           ((uint8_t*)&last)[15] == test_val ? "PASS" : "FAIL");
    
    assert(((uint8_t*)&first)[0] == test_val);
    assert(((uint8_t*)&last)[15] == test_val);
}

int main() {
    printf("=== Testing PINSRB instruction variants ===\n");
    
    test_pinsrb_reg();
    test_pinsrb_mem();
    test_pinsrb_boundary();
    
    printf("\nAll PINSRB tests passed successfully!\n");
    return 0;
}
