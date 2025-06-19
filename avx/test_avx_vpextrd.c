#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void print_results(const char* desc, uint32_t res, uint32_t exp) {
    printf("%-15s: res=0x%08x exp=0x%08x %s\n", 
           desc, res, exp, (res == exp) ? "PASSED" : "FAILED");
}

void test_vpextrd() {
    printf("--- Testing VPEXTRD ---\n");
    
    // Test data
    __m128i vec = _mm_setr_epi32(0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00);
    uint32_t expected[4] = {0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00};
    
    // Test all 4 positions (0-3)
    for (int i = 0; i < 4; i++) {
        uint32_t res;
        switch(i) {
            case 0: res = _mm_extract_epi32(vec, 0); break;
            case 1: res = _mm_extract_epi32(vec, 1); break;
            case 2: res = _mm_extract_epi32(vec, 2); break;
            case 3: res = _mm_extract_epi32(vec, 3); break;
        }
        print_results("Register", res, expected[i]);
    }
    
    // Test memory destination
    for (int i = 0; i < 4; i++) {
        uint32_t mem_res = 0;
        switch(i) {
            case 0: mem_res = _mm_extract_epi32(vec, 0); break;
            case 1: mem_res = _mm_extract_epi32(vec, 1); break;
            case 2: mem_res = _mm_extract_epi32(vec, 2); break;
            case 3: mem_res = _mm_extract_epi32(vec, 3); break;
        }
        print_results("Memory", mem_res, expected[i]);
    }
    
    // Edge case testing
    __m128i edge_vec = _mm_setr_epi32(0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0x00000000);
    uint32_t edge_expected[4] = {0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0x00000000};
    
    for (int i = 0; i < 4; i++) {
        uint32_t edge_res;
        switch(i) {
            case 0: edge_res = _mm_extract_epi32(edge_vec, 0); break;
            case 1: edge_res = _mm_extract_epi32(edge_vec, 1); break;
            case 2: edge_res = _mm_extract_epi32(edge_vec, 2); break;
            case 3: edge_res = _mm_extract_epi32(edge_vec, 3); break;
        }
        print_results("Edge Case", edge_res, edge_expected[i]);
    }
}

int main() {
    test_vpextrd();
    return 0;
}
