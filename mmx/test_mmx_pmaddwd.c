#include <stdio.h>
#include <stdint.h>
#include <string.h>

void print_mmx_dwords(const char* name, const uint64_t value) {
    int32_t v[2];
    memcpy(v, &value, sizeof(v));
    printf("%s: [%d, %d]\n", name, v[1], v[0]);
}

int main() {
    printf("Testing PMADDWD (Packed Multiply and Add Words to Doublewords)\n");
    printf("============================================================\n\n");

    // Test case 1: Positive numbers
    {
        uint64_t a = 0x0002000300040005; // [2, 3, 4, 5]
        uint64_t b = 0x0001000200030004; // [1, 2, 3, 4]
        uint64_t result;

        printf("Test 1: Positive numbers\n");
        printf("Input A: [2, 3, 4, 5]\n");
        printf("Input B: [1, 2, 3, 4]\n");

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pmaddwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_dwords("Result", result);
        printf("Expected: [8, 32]\n"); // 2*1 + 3*2 = 8, 4*3 + 5*4 = 32
        printf("\n");
    }

    // Test case 2: Mixed positive and negative
    {
        uint64_t a = 0xFFFE00030004FFFF; // [-2, 3, 4, -1]
        uint64_t b = 0x00010002FFFD0004; // [1, 2, -3, 4]
        uint64_t result;

        printf("Test 2: Mixed positive and negative\n");
        printf("Input A: [-2, 3, 4, -1]\n");
        printf("Input B: [1, 2, -3, 4]\n");

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pmaddwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_dwords("Result", result);
        printf("Expected: [4, -16]\n"); // -2*1 + 3*2 = 4, 4*-3 + -1*4 = -16
        printf("\n");
    }

    // Test case 3: Large numbers (potential overflow)
    {
        uint64_t a = 0x7FFF7FFF7FFF7FFF; // [32767, 32767, 32767, 32767]
        uint64_t b = 0x7FFF7FFF7FFF7FFF; // [32767, 32767, 32767, 32767]
        uint64_t result;

        printf("Test 3: Large numbers (potential overflow)\n");
        printf("Input A: [32767, 32767, 32767, 32767]\n");
        printf("Input B: [32767, 32767, 32767, 32767]\n");

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pmaddwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_dwords("Result", result);
        printf("Expected: [2147352578, 2147352578]\n"); // 32767*32767 + 32767*32767 = 2147352578
        printf("\n");
    }

    printf("PMADDWD tests completed.\n");
    return 0;
}
