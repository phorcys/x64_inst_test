#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void test_vpinsrd_reg() {
    __m128i dst = _mm_setzero_si128();
    uint32_t val = 0x12345678;
    
    printf("Testing VPINSRD with register source\n");
    
    // Test inserting at position 0
    __m128i src = dst;
    asm volatile ("vpinsrd $0, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val) : "memory");
    uint32_t result = _mm_extract_epi32(dst, 0);
    printf("  Position 0: 0x%08x (expected 0x12345678) - %s\n", 
           result, result == 0x12345678 ? "PASS" : "FAIL");
    
    // Test inserting at position 3
    dst = _mm_setzero_si128();
    src = dst;
    asm volatile ("vpinsrd $3, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val) : "memory");
    result = _mm_extract_epi32(dst, 3);
    printf("  Position 3: 0x%08x (expected 0x12345678) - %s\n", 
           result, result == 0x12345678 ? "PASS" : "FAIL");
}

static void test_vpinsrd_mem() {
    __m128i dst = _mm_setzero_si128();
    uint32_t mem_val = 0x87654321;
    
    printf("\nTesting VPINSRD with memory source\n");
    
    // Test inserting at position 1
    __m128i src = dst;
    asm volatile ("vpinsrd $1, %2, %1, %0" : "=x"(dst) : "x"(src), "m"(mem_val) : "memory");
    uint32_t result = _mm_extract_epi32(dst, 1);
    printf("  Position 1: 0x%08x (expected 0x87654321) - %s\n", 
           result, result == 0x87654321 ? "PASS" : "FAIL");
}

static void test_vpinsrd_boundary() {
    __m128i dst = _mm_set1_epi32(0xFFFFFFFF);
    uint32_t val = 0x00000000;
    
    printf("\nTesting VPINSRD boundary values\n");
    
    // Test inserting 0x00000000 into all-0xFFFFFFFF
    __m128i src = dst;
    asm volatile ("vpinsrd $2, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val) : "memory");
    uint32_t result = _mm_extract_epi32(dst, 2);
    printf("  Boundary test: 0x%08x (expected 0x00000000) - %s\n", 
           result, result == 0x00000000 ? "PASS" : "FAIL");
}

int main() {
    printf("Starting VPINSRD tests\n");
    printf("=====================\n");
    
    test_vpinsrd_reg();
    test_vpinsrd_mem();
    test_vpinsrd_boundary();
    
    printf("\nVPINSRD tests completed\n");
    return 0;
}
