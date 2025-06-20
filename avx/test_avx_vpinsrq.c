#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void test_vpinsrq_reg() {
    __m128i dst = _mm_setzero_si128();
    uint64_t val = 0x123456789ABCDEF0;
    
    printf("Testing VPINSRQ with register source\n");
    
    // Test inserting at position 0
    __m128i src = dst;
    asm volatile ("vpinsrq $0, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val) : "memory");
    uint64_t result = _mm_extract_epi64(dst, 0);
    printf("  Position 0: 0x%016lx (expected 0x123456789ABCDEF0) - %s\n", 
           result, result == 0x123456789ABCDEF0 ? "PASS" : "FAIL");
    
    // Test inserting at position 1
    dst = _mm_setzero_si128();
    src = dst;
    asm volatile ("vpinsrq $1, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val) : "memory");
    result = _mm_extract_epi64(dst, 1);
    printf("  Position 1: 0x%016lx (expected 0x123456789ABCDEF0) - %s\n", 
           result, result == 0x123456789ABCDEF0 ? "PASS" : "FAIL");
}

static void test_vpinsrq_mem() {
    __m128i dst = _mm_setzero_si128();
    uint64_t mem_val = 0xFEDCBA9876543210;
    
    printf("\nTesting VPINSRQ with memory source\n");
    
    // Test inserting at position 0
    __m128i src = dst;
    asm volatile ("vpinsrq $0, %2, %1, %0" : "=x"(dst) : "x"(src), "m"(mem_val) : "memory");
    uint64_t result = _mm_extract_epi64(dst, 0);
    printf("  Position 0: 0x%016lx (expected 0xFEDCBA9876543210) - %s\n", 
           result, result == 0xFEDCBA9876543210 ? "PASS" : "FAIL");
}

static void test_vpinsrq_boundary() {
    __m128i dst = _mm_set1_epi64x(0xFFFFFFFFFFFFFFFF);
    uint64_t val = 0x0000000000000000;
    
    printf("\nTesting VPINSRQ boundary values\n");
    
    // Test inserting 0x0000000000000000 into all-0xFFFFFFFFFFFFFFFF
    __m128i src = dst;
    asm volatile ("vpinsrq $1, %2, %1, %0" : "=x"(dst) : "x"(src), "r"(val) : "memory");
    uint64_t result = _mm_extract_epi64(dst, 1);
    printf("  Boundary test: 0x%016lx (expected 0x0000000000000000) - %s\n", 
           result, result == 0x0000000000000000 ? "PASS" : "FAIL");
}

int main() {
    printf("Starting VPINSRQ tests\n");
    printf("=====================\n");
    
    test_vpinsrq_reg();
    test_vpinsrq_mem();
    test_vpinsrq_boundary();
    
    printf("\nVPINSRQ tests completed\n");
    return 0;
}
