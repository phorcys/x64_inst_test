#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TEST_VPEXTRQ_REG(index, expected) do { \
    uint64_t r; \
    asm volatile ("vpextrq %1, %2, %0" : "=r"(r) : "i"(index), "x"(xmm) : "memory"); \
    printf("  Index %d: Extracted=0x%016lx, Expected=0x%016lx - %s\n", \
           index, r, expected, r == expected ? "PASS" : "FAIL"); \
} while(0)

static void test_vpextrq_reg() {
    __m128i xmm = _mm_setr_epi64x(0x0123456789ABCDEF, 0xFEDCBA9876543210);
    
    printf("Testing VPEXTRQ reg64, xmm, imm8\n");
    
    // Test index 0
    TEST_VPEXTRQ_REG(0, 0xFEDCBA9876543210);
    
    // Test index 1
    TEST_VPEXTRQ_REG(1, 0x0123456789ABCDEF);
    
    // Test invalid index (should wrap around)
    TEST_VPEXTRQ_REG(2, 0xFEDCBA9876543210);
}

static void test_vpextrq_mem() {
    // Ensure memory is properly aligned and initialized
    uint64_t __attribute__((aligned(16))) mem[3] = {0xDEADBEEFDEADBEEF, 0xCAFEBABECAFEBABE, 0xBADF00DBADF00D};
    __m128i xmm = _mm_setr_epi64x(0x1122334455667788, 0x8877665544332211);
    
    printf("Testing VPEXTRQ mem64, xmm, imm8\n");
    
    // Test index 0
    asm volatile ("vpextrq %1, %2, %0" : "=m"(mem[0]) : "i"(0), "x"(xmm) : "memory");
    printf("  Index 0: Extracted=0x%016lx, Expected=0x8877665544332211 - %s\n", 
           mem[0], mem[0] == 0x8877665544332211 ? "PASS" : "FAIL");
    
    // Test index 1
    asm volatile ("vpextrq %1, %2, %0" : "=m"(mem[1]) : "i"(1), "x"(xmm) : "memory");
    printf("  Index 1: Extracted=0x%016lx, Expected=0x1122334455667788 - %s\n", 
           mem[1], mem[1] == 0x1122334455667788 ? "PASS" : "FAIL");
    
    // Verify memory wasn't corrupted
    if (mem[2] != 0xBADF00DBADF00D) {
        printf("  Memory corruption detected!\n");
    }
}

static void test_vpextrq_boundary() {
    uint64_t r = 0;
    __m128i xmm = _mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000);
    
    printf("Testing VPEXTRQ boundary values\n");
    
    // Test max value
    asm volatile ("vpextrq %1, %2, %0" : "=r"(r) : "i"(0), "x"(xmm) : "memory");
    printf("  Max value: Extracted=0x%016lx, Expected=0x0000000000000000 - %s\n", 
           r, r == 0x0000000000000000 ? "PASS" : "FAIL");
    
    // Test min value
    asm volatile ("vpextrq %1, %2, %0" : "=r"(r) : "i"(1), "x"(xmm) : "memory");
    printf("  Min value: Extracted=0x%016lx, Expected=0xFFFFFFFFFFFFFFFF - %s\n", 
           r, r == 0xFFFFFFFFFFFFFFFF ? "PASS" : "FAIL");
}

int main() {
    printf("Starting VPEXTRQ tests\n");
    printf("----------------------\n");
    
    test_vpextrq_reg();
    test_vpextrq_mem();
    test_vpextrq_boundary();
    
    printf("\nVPEXTRQ tests completed\n");
    return 0;
}
