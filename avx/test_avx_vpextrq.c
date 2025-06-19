#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void test_vpextrq_reg() {
    uint64_t r = 0;
    __m128i xmm = _mm_setr_epi64x(0x0123456789ABCDEF, 0xFEDCBA9876543210);
    
    printf("Testing VPEXTRQ reg64, xmm, imm8\n");
    
    // Test index 0
    asm volatile ("vpextrq %q0, %1, 0" : "=r"(r) : "x"(xmm));
    printf("  Index 0: Extracted=0x%016lx, Expected=0x0123456789ABCDEF - %s\n", 
           r, r == 0x0123456789ABCDEF ? "PASS" : "FAIL");
    
    // Test index 1
    asm volatile ("vpextrq %q0, %1, 1" : "=r"(r) : "x"(xmm));
    printf("  Index 1: Extracted=0x%016lx, Expected=0xFEDCBA9876543210 - %s\n", 
           r, r == 0xFEDCBA9876543210 ? "PASS" : "FAIL");
}

static void test_vpextrq_mem() {
    uint64_t mem[2] = {0};
    __m128i xmm = _mm_setr_epi64x(0x1122334455667788, 0x8877665544332211);
    
    printf("Testing VPEXTRQ mem64, xmm, imm8\n");
    
    // Test index 0
    asm volatile ("vpextrq %q0, %1, 0" : "=m"(mem[0]) : "x"(xmm));
    printf("  Index 0: Extracted=0x%016lx, Expected=0x1122334455667788 - %s\n", 
           mem[0], mem[0] == 0x1122334455667788 ? "PASS" : "FAIL");
    
    // Test index 1
    asm volatile ("vpextrq %q0, %1, 1" : "=m"(mem[1]) : "x"(xmm));
    printf("  Index 1: Extracted=0x%016lx, Expected=0x8877665544332211 - %s\n", 
           mem[1], mem[1] == 0x8877665544332211 ? "PASS" : "FAIL");
}

static void test_vpextrq_boundary() {
    uint64_t r = 0;
    __m128i xmm = _mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0x0000000000000000);
    
    printf("Testing VPEXTRQ boundary values\n");
    
    // Test max value
    asm volatile ("vpextrq %q0, %1, 0" : "=r"(r) : "x"(xmm));
    printf("  Max value: Extracted=0x%016lx, Expected=0xFFFFFFFFFFFFFFFF - %s\n", 
           r, r == 0xFFFFFFFFFFFFFFFF ? "PASS" : "FAIL");
    
    // Test min value
    asm volatile ("vpextrq %q0, %1, 1" : "=r"(r) : "x"(xmm));
    printf("  Min value: Extracted=0x%016lx, Expected=0x0000000000000000 - %s\n", 
           r, r == 0x0000000000000000 ? "PASS" : "FAIL");
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
