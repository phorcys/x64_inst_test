#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void test_vphminposuw_basic() {
    __m128i result;
    __m128i src = _mm_setr_epi16(0x1234, 0x5678, 0x9ABC, 0xDEF0, 
                               0x0000, 0xFFFF, 0x1111, 0x2222);
    
    printf("Testing VPHMINPOSUW basic functionality\n");
    
    asm volatile ("vphminposuw %1, %0" : "=x"(result) : "x"(src) : "memory");
    
    uint16_t min_val = _mm_extract_epi16(result, 0);
    uint16_t min_idx = _mm_extract_epi16(result, 1);
    print_uint16_vec("  Source: ", (uint16_t*)&src, 8);
    printf("  Min value: 0x%04x (expected 0x0000)\n", min_val);
    printf("  Min index: %d (expected 4)\n", min_idx);
    print_uint16_vec("  Result: ", (uint16_t*)&result, 8);
    printf("  Test %s\n", (min_val == 0x0000 && min_idx == 4) ? "PASSED" : "FAILED");
}

static void test_vphminposuw_duplicate_min() {
    __m128i result; 
    __m128i src = _mm_setr_epi16(0x1111, 0x1111, 0x2222, 0x3333,
                               0x1111, 0x4444, 0x5555, 0x1111);
    
    printf("\nTesting VPHMINPOSUW with duplicate minimum values\n");
    
    asm volatile ("vphminposuw %1, %0" : "=x"(result) : "x"(src) : "memory");
    
    uint16_t min_val = _mm_extract_epi16(result, 0);
    uint16_t min_idx = _mm_extract_epi16(result, 1);
    print_uint16_vec("  Source: ", (uint16_t*)&src, 8);
    printf("  Min value: 0x%04x (expected 0x1111)\n", min_val);
    printf("  Min index: %d (should be first occurrence)\n", min_idx);
    print_uint16_vec("  Result: ", (uint16_t*)&result, 8);
    printf("  Test %s\n", (min_val == 0x1111 && min_idx == 0) ? "PASSED" : "FAILED");
}

static void test_vphminposuw_boundary() {
    __m128i result;
    __m128i src = _mm_setr_epi16(0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
                               0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
    
    printf("\nTesting VPHMINPOSUW boundary values\n");
    
    asm volatile ("vphminposuw %1, %0" : "=x"(result) : "x"(src) : "memory");
    
    uint16_t min_val = _mm_extract_epi16(result, 0);
    uint16_t min_idx = _mm_extract_epi16(result, 1);
    print_uint16_vec("  Source: ", (uint16_t*)&src, 8);
    printf("  Min value: 0x%04x (expected 0x0000)\n", min_val);
    printf("  Min index: %d (expected 3)\n", min_idx);
    print_uint16_vec("  Result: ", (uint16_t*)&result, 8);
    printf("  Test %s\n", (min_val == 0x0000 && min_idx == 3) ? "PASSED" : "FAILED");
}

int main() {
    printf("Starting VPHMINPOSUW tests\n");
    printf("=========================\n");
    
    test_vphminposuw_basic();
    test_vphminposuw_duplicate_min();
    test_vphminposuw_boundary();
    
    printf("\nVPHMINPOSUW tests completed\n");
    return 0;
}
