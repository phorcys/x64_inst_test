#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void test_vpinsrb_reg() {
    __m128i dst = _mm_setzero_si128();
    uint8_t val = 0xAB;
    
    printf("Testing VPINSRB with register source\n");
    
    // Test inserting at position 0
    __m128i src = dst;
    uint32_t val32 = val;
    asm volatile ("vpinsrb $0, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val32) : "memory");
    uint8_t result = _mm_extract_epi8(dst, 0);
    printf("  Position 0: 0x%02x (expected 0xAB) - %s\n", 
           result, result == 0xAB ? "PASS" : "FAIL");
    
    // Test inserting at position 15
    dst = _mm_setzero_si128();
    src = dst;
    val32 = val;
    asm volatile ("vpinsrb $15, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val32) : "memory");
    result = _mm_extract_epi8(dst, 15);
    printf("  Position 15: 0x%02x (expected 0xAB) - %s\n", 
           result, result == 0xAB ? "PASS" : "FAIL");
}

static void test_vpinsrb_mem() {
    __m128i dst = _mm_setzero_si128();
    uint8_t mem_val = 0xCD;
    
    printf("\nTesting VPINSRB with memory source\n");
    
    // Test inserting at position 7
    __m128i src = dst;
    asm volatile ("vpinsrb $7, %2, %1, %0" : "=x"(dst) : "x"(src), "m"(mem_val) : "memory");
    uint8_t result = _mm_extract_epi8(dst, 7);
    printf("  Position 7: 0x%02x (expected 0xCD) - %s\n", 
           result, result == 0xCD ? "PASS" : "FAIL");
}

static void test_vpinsrb_boundary() {
    __m128i dst = _mm_set1_epi8(0xFF);
    uint8_t val = 0x00;
    
    printf("\nTesting VPINSRB boundary values\n");
    
    // Test inserting 0x00 into all-0xFF
    __m128i src = dst;
    uint32_t val32 = val;
    asm volatile ("vpinsrb $8, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val32) : "memory");
    uint8_t result = _mm_extract_epi8(dst, 8);
    printf("  Boundary test: 0x%02x (expected 0x00) - %s\n", 
           result, result == 0x00 ? "PASS" : "FAIL");
}

int main() {
    printf("Starting VPINSRB tests\n");
    printf("=====================\n");
    
    test_vpinsrb_reg();
    test_vpinsrb_mem();
    test_vpinsrb_boundary();
    
    printf("\nVPINSRB tests completed\n");
    return 0;
}
