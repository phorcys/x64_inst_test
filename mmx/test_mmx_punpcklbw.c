#include <stdio.h>
#include <stdint.h>
#include <mmintrin.h> // MMX intrinsics

void test_punpcklbw() {
    printf("Testing PUNPCKLBW instruction\n\n");
    
    // Test data
    uint8_t a_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t b_data[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11};
    
    // Load data into MMX registers
    __m64 a = _mm_set_pi8(a_data[7], a_data[6], a_data[5], a_data[4],
                      a_data[3], a_data[2], a_data[1], a_data[0]);
    __m64 b = _mm_set_pi8(b_data[7], b_data[6], b_data[5], b_data[4],
                      b_data[3], b_data[2], b_data[1], b_data[0]);
    
    // Test 1: Register to register
    __m64 res1 = _mm_unpacklo_pi8(a, b);
    printf("Test 1: Register to register\n");
    printf("Expected: 0x%04X%04X%04X%04X\n", 
           0xDD44, 0xCC33, 0xBB22, 0xAA11);
    printf("Got:      0x%04X%04X%04X%04X\n\n",
           ((uint16_t*)&res1)[0], ((uint16_t*)&res1)[1],
           ((uint16_t*)&res1)[2], ((uint16_t*)&res1)[3]);
    
    // Test 2: Memory to register
    __m64 res2 = _mm_unpacklo_pi8(a, *(__m64*)b_data);
    printf("Test 2: Memory to register\n");
    printf("Expected: 0x%04X%04X%04X%04X\n", 
           0xDD44, 0xCC33, 0xBB22, 0xAA11);
    printf("Got:      0x%04X%04X%04X%04X\n\n",
           ((uint16_t*)&res2)[0], ((uint16_t*)&res2)[1],
           ((uint16_t*)&res2)[2], ((uint16_t*)&res2)[3]);
    
    // Test 3: Boundary values
    uint8_t min_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t max_data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    __m64 min_val = _mm_set_pi8(min_data[7], min_data[6], min_data[5], min_data[4],
                               min_data[3], min_data[2], min_data[1], min_data[0]);
    __m64 max_val = _mm_set_pi8(max_data[7], max_data[6], max_data[5], max_data[4],
                               max_data[3], max_data[2], max_data[1], max_data[0]);
    
    __m64 res3 = _mm_unpacklo_pi8(min_val, max_val);
    printf("Test 3: Boundary values\n");
    printf("Expected: 0x%04X%04X%04X%04X\n", 
           0xFF00, 0xFF00, 0xFF00, 0xFF00);
    printf("Got:      0x%04X%04X%04X%04X\n\n",
           ((uint16_t*)&res3)[3], ((uint16_t*)&res3)[2],
           ((uint16_t*)&res3)[1], ((uint16_t*)&res3)[0]);
    
    // Clear MMX state
    _mm_empty();
}

int main() {
    test_punpcklbw();
    return 0;
}
