#include <stdio.h>
#include <stdint.h>
#include <string.h>

void print_mmx_words(const char* name, const uint64_t value) {
    int16_t v[4];
    memcpy(v, &value, sizeof(v));
    printf("%s: [%d, %d, %d, %d]\n", name, v[3], v[2], v[1], v[0]);
}

int main() {
    printf("Testing PADDSW (Packed Add Signed Word with Saturation)\n");
    printf("======================================================\n\n");

    // Test case 1: Normal addition
    {
        uint64_t a = 0x0001000200037FFF; // [1, 2, 3, 32767]
        uint64_t b = 0x0001000100010001; // [1, 1, 1, 1]
        uint64_t result;

        printf("Test 1: Normal addition\n");
        print_mmx_words("Input A", a);
        print_mmx_words("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "paddsw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_words("Result", result);
        printf("Expected: [2, 3, 4, 32767]\n");
        printf("\n");
    }

    // Test case 2: Positive overflow (saturate to 32767)
    {
        uint64_t a = 0x7FFF7FFF7FFF7FFF; // [32767, 32767, 32767, 32767]
        uint64_t b = 0x0001000100010001; // [1, 1, 1, 1]
        uint64_t result;

        printf("Test 2: Positive overflow (saturate to 32767)\n");
        print_mmx_words("Input A", a);
        print_mmx_words("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "paddsw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_words("Result", result);
        printf("Expected: [32767, 32767, 32767, 32767]\n");
        printf("\n");
    }

    // Test case 3: Negative overflow (saturate to -32768)
    {
        uint64_t a = 0x8000800080008000; // [-32768, -32768, -32768, -32768]
        uint64_t b = 0xFFFFFFFFFFFFFFFF; // [-1, -1, -1, -1]
        uint64_t result;

        printf("Test 3: Negative overflow (saturate to -32768)\n");
        print_mmx_words("Input A", a);
        print_mmx_words("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "paddsw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_words("Result", result);
        printf("Expected: [-32768, -32768, -32768, -32768]\n");
        printf("\n");
    }

    printf("PADDSW tests completed.\n");
    return 0;
}
