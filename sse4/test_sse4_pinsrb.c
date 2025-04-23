#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== TEST: %s ===\n", name)

static void test_pinsrb_reg() {
    uint8_t src = 0xAB;
    uint8_t dst[16] = {0};
    uint8_t expected[16] = {0};
    uint8_t result[16] = {0};
    
    // Test inserting at position 0
    expected[0] = 0xAB;
    __asm__ volatile (
        "movdqu %1, %%xmm0\n\t"
        "mov %2, %%al\n\t"
        "pinsrb $0, %%eax, %%xmm0\n\t"
        "movdqu %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "r"(src)
        : "%xmm0", "%eax"
    );
    printf("Position 0: ");
    for(int i=0; i<16; i++) {
        printf("%02X ", result[i]);
        assert(result[i] == expected[i]);
    }
    printf("- PASS\n");

    // Test inserting at position 15
    memset(expected, 0, 16);
    expected[15] = 0xAB;
    __asm__ volatile (
        "movdqu %1, %%xmm0\n\t"
        "mov %2, %%al\n\t"
        "pinsrb $15, %%eax, %%xmm0\n\t"
        "movdqu %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "r"(src)
        : "%xmm0", "%eax"
    );
    printf("Position 15: ");
    for(int i=0; i<16; i++) {
        printf("%02X ", result[i]);
        assert(result[i] == expected[i]);
    }
    printf("- PASS\n");
}

static void test_pinsrb_mem() {
    uint8_t src = 0xCD;
    uint8_t dst[16] = {0};
    uint8_t expected[16] = {0};
    uint8_t result[16] = {0};
    
    // Test memory operand
    expected[7] = 0xCD;
    __asm__ volatile (
        "movdqu %1, %%xmm0\n\t"
        "pinsrb $7, %2, %%xmm0\n\t"
        "movdqu %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "m"(src)
        : "%xmm0"
    );
    printf("Position 7: ");
    for(int i=0; i<16; i++) {
        printf("%02X ", result[i]);
        assert(result[i] == expected[i]);
    }
    printf("- PASS\n");
}

static void test_pinsrb_imm() {
    uint8_t dst[16] = {0};
    uint8_t expected[16] = {0};
    uint8_t result[16] = {0};
    uint32_t imm = 0xEF;
    
    // Test immediate operand using register
    expected[3] = 0xEF;
    __asm__ volatile (
        "movdqu %1, %%xmm0\n\t"
        "mov %2, %%eax\n\t"
        "pinsrb $3, %%eax, %%xmm0\n\t"
        "movdqu %%xmm0, %0\n\t"
        : "=m"(result)
        : "m"(dst), "r"(imm)
        : "%xmm0", "%eax"
    );
    printf("Position 3: ");
    for(int i=0; i<16; i++) {
        printf("%02X ", result[i]);
        assert(result[i] == expected[i]);
    }
    printf("- PASS\n");
}

int main() {
    printf("Testing PINSRB instruction\n");
    test_pinsrb_reg();
    test_pinsrb_mem();
    test_pinsrb_imm();
    printf("All PINSRB tests passed\n");
    return 0;
}
