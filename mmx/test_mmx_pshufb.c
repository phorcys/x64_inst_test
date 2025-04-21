#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static void print_bytes(const uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

static void fail_test(void) {
    printf("TEST FAILED\n");
    exit(1);
}

/* Test the PSHUFB (Pack Shuffle Bytes) instruction */
/* 0F 38 00 /r PSHUFB mm1, mm2/m64 */

static void test_pshufb_reg_reg(void) {
    uint8_t mm1[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t mm2[8] = {0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06};
    uint8_t expected[8] = {0x22, 0x11, 0x44, 0x33, 0x66, 0x55, 0x88, 0x77};
    uint8_t result[8];
    
    __asm__ volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "pshufb %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result)
        : "m"(mm1), "m"(mm2)
        : "mm0", "mm1"
    );
    
    if (memcmp(result, expected, sizeof(result)) != 0) {
        printf("PSHUFB reg/reg test failed\n");
        printf("Expected: ");
        print_bytes(expected, sizeof(expected));
        printf("Got:      ");
        print_bytes(result, sizeof(result));
        fail_test();
    } else {
        printf("PSHUFB reg/reg test passed\n");
    }
}

static void test_pshufb_reg_mem(void) {
    uint8_t mm1[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t mm2[8] = {0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
    uint8_t expected[8] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
    uint8_t result[8];
    
    __asm__ volatile (
        "movq %1, %%mm0\n\t"
        "pshufb %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result)
        : "m"(mm1), "m"(mm2)
        : "mm0"
    );
    
    if (memcmp(result, expected, sizeof(result)) != 0) {
        printf("PSHUFB reg/mem test failed\n");
        printf("Expected: ");
        print_bytes(expected, sizeof(expected));
        printf("Got:      ");
        print_bytes(result, sizeof(result));
        fail_test();
    } else {
        printf("PSHUFB reg/mem test passed\n");
    }
}

static void test_pshufb_edge_cases(void) {
    // Test with high bit set in shuffle control (should zero the byte)
    uint8_t mm1[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t mm2[8] = {0x80, 0x01, 0x82, 0x03, 0x84, 0x05, 0x86, 0x07};
    uint8_t expected[8] = {0x00, 0x22, 0x00, 0x44, 0x00, 0x66, 0x00, 0x88};
    uint8_t result[8];
    
    __asm__ volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "pshufb %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result)
        : "m"(mm1), "m"(mm2)
        : "mm0", "mm1"
    );
    
    if (memcmp(result, expected, sizeof(result)) != 0) {
        printf("PSHUFB edge case test failed\n");
        printf("Expected: ");
        print_bytes(expected, sizeof(expected));
        printf("Got:      ");
        print_bytes(result, sizeof(result));
        fail_test();
    } else {
        printf("PSHUFB edge case test passed\n");
    }
}

int main() {
    test_pshufb_reg_reg();
    test_pshufb_reg_mem();
    test_pshufb_edge_cases();
    
    return 0;
}
