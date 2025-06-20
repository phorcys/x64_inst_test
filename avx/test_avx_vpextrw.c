#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TEST_VPEXTRW_REG(index, expected) do { \
    uint16_t r; \
    asm volatile ("vpextrw $" #index ", %x1, %k0" : "=r"(r) : "x"(xmm)); \
    printf("  Index %d: Extracted=0x%04x, Expected=0x%04x - %s\n", \
           index, r, expected, r == expected ? "PASS" : "FAIL"); \
} while(0)

static void test_vpextrw_reg() {
    __m128i xmm = _mm_setr_epi16(0x0123, 0x4567, 0x89AB, 0xCDEF,
                                0xFEDC, 0xBA98, 0x7654, 0x3210);
    
    printf("Testing VPEXTRW reg16, xmm, imm8\n");
    
    // Test valid indices (0-7)
    TEST_VPEXTRW_REG(0, 0x0123);
    TEST_VPEXTRW_REG(1, 0x4567);
    TEST_VPEXTRW_REG(2, 0x89AB);
    TEST_VPEXTRW_REG(3, 0xCDEF);
    TEST_VPEXTRW_REG(4, 0xFEDC);
    TEST_VPEXTRW_REG(5, 0xBA98);
    TEST_VPEXTRW_REG(6, 0x7654);
    TEST_VPEXTRW_REG(7, 0x3210);
    
    // Test invalid index (should wrap around)
    TEST_VPEXTRW_REG(8, 0x0123);
}

static void test_vpextrw_mem() {
    // Ensure memory is properly aligned and initialized
    uint16_t __attribute__((aligned(16))) mem[9] = {0x1111, 0x2222, 0x3333, 0x4444,
                                                  0x5555, 0x6666, 0x7777, 0x8888,
                                                  0x9999};
    __m128i xmm = _mm_setr_epi16(0x1122, 0x3344, 0x5566, 0x7788,
                                0x9900, 0xAABB, 0xCCDD, 0xEEFF);
    
    printf("Testing VPEXTRW mem16, xmm, imm8\n");
    
    // Test valid indices (0-7)
    for (int i = 0; i < 8; ++i) {
        uint16_t extracted;
        switch(i) {
            case 0: asm volatile ("vpextrw $0, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 1: asm volatile ("vpextrw $1, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 2: asm volatile ("vpextrw $2, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 3: asm volatile ("vpextrw $3, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 4: asm volatile ("vpextrw $4, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 5: asm volatile ("vpextrw $5, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 6: asm volatile ("vpextrw $6, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
            case 7: asm volatile ("vpextrw $7, %x1, %k0" : "=r"(extracted) : "x"(xmm)); break;
        }
        mem[i] = extracted;
        uint16_t expected = ((uint16_t*)&xmm)[i];
        printf("  Index %d: Extracted=0x%04x, Expected=0x%04x - %s\n",
               i, mem[i], expected,
               mem[i] == expected ? "PASS" : "FAIL");
    }
    
    // Verify memory wasn't corrupted
    if (mem[8] != 0x9999) {
        printf("  Memory corruption detected!\n");
    }
}

static void test_vpextrw_boundary() {
    uint16_t r;
    __m128i xmm = _mm_setr_epi16(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                                0x0000, 0x0000, 0x0000, 0x0000);
    
    printf("Testing VPEXTRW boundary values\n");
    
    // Test max value
    asm volatile ("vpextrw $0, %x1, %k0" : "=r"(r) : "x"(xmm));
    printf("  Max value: Extracted=0x%04x, Expected=0xFFFF - %s\n",
           r, r == 0xFFFF ? "PASS" : "FAIL");
    
    // Test min value
    asm volatile ("vpextrw $4, %x1, %k0" : "=r"(r) : "x"(xmm));
    printf("  Min value: Extracted=0x%04x, Expected=0x0000 - %s\n",
           r, r == 0x0000 ? "PASS" : "FAIL");
}

int main() {
    printf("Starting VPEXTRW tests\n");
    printf("----------------------\n");
    
    test_vpextrw_reg();
    test_vpextrw_mem();
    test_vpextrw_boundary();
    
    printf("\nVPEXTRW tests completed\n");
    return 0;
}
