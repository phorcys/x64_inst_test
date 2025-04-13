#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

void print_mmx_hex(const char* name, const uint64_t value) {
    printf("%s: 0x%016" PRIx64 "\n", name, value);
}

int main() {
    printf("Testing PXOR (Packed Bitwise XOR)\n");
    printf("===============================\n\n");

    // Test case 1: Basic XOR operation
    {
        uint64_t a = 0x00FF00FF00FF00FF;
        uint64_t b = 0x0F0F0F0F0F0F0F0F;
        uint64_t result;

        printf("Test 1: Basic XOR operation\n");
        print_mmx_hex("Input A", a);
        print_mmx_hex("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pxor %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_hex("Result", result);
        printf("Expected: 0x0FF00FF00FF00FF0\n");
        printf("\n");
    }

    // Test case 2: XOR with all zeros
    {
        uint64_t a = 0x123456789ABCDEF0;
        uint64_t b = 0x0000000000000000;
        uint64_t result;

        printf("Test 2: XOR with all zeros\n");
        print_mmx_hex("Input A", a);
        print_mmx_hex("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pxor %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_hex("Result", result);
        printf("Expected: 0x123456789ABCDEF0\n");
        printf("\n");
    }

    // Test case 3: XOR with all ones
    {
        uint64_t a = 0x123456789ABCDEF0;
        uint64_t b = 0xFFFFFFFFFFFFFFFF;
        uint64_t result;

        printf("Test 3: XOR with all ones\n");
        print_mmx_hex("Input A", a);
        print_mmx_hex("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pxor %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_hex("Result", result);
        printf("Expected: 0xEDCBA9876543210F\n");
        printf("\n");
    }

    printf("PXOR tests completed.\n");
    return 0;
}
