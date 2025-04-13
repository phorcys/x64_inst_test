#include <stdio.h>
#include <stdint.h>
#include <string.h>

void print_mmx(const char* name, const uint64_t value) {
    uint16_t v[4];
    memcpy(v, &value, sizeof(v));
    printf("%s: [%d, %d, %d, %d]\n", name, v[3], v[2], v[1], v[0]);
}

int main() {
    printf("Testing PMULHW (Packed Multiply High Signed Word)\n");
    printf("===============================================\n\n");

    // Test case 1: Positive numbers
    {
        uint64_t a = 0x0002000300040005; // [2, 3, 4, 5]
        uint64_t b = 0x0001000200030004; // [1, 2, 3, 4]
        uint64_t result;

        printf("Test 1: Positive numbers\n");
        print_mmx("Input A", a);
        print_mmx("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pmulhw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx("Result", result);
        printf("Expected: [0, 0, 0, 0]\n"); // 2*1=2 (high word=0), 3*2=6 (high word=0), etc.
        printf("\n");
    }

    // Test case 2: Mixed positive and negative
    {
        uint64_t a = 0xFFFE00030004FFFF; // [-2, 3, 4, -1]
        uint64_t b = 0x00010002FFFD0004; // [1, 2, -3, 4]
        uint64_t result;

        printf("Test 2: Mixed positive and negative\n");
        print_mmx("Input A", a);
        print_mmx("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pmulhw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx("Result", result);
        printf("Expected: [-1, 0, -1, -1]\n"); // -2*1=-2 (high word=-1), 3*2=6 (high word=0), etc.
        printf("\n");
    }

    // Test case 3: Large numbers (potential overflow)
    {
        uint64_t a = 0x7FFF7FFF7FFF7FFF; // [32767, 32767, 32767, 32767]
        uint64_t b = 0x7FFF7FFF7FFF7FFF; // [32767, 32767, 32767, 32767]
        uint64_t result;

        printf("Test 3: Large numbers (potential overflow)\n");
        print_mmx("Input A", a);
        print_mmx("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pmulhw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx("Result", result);
        printf("Expected: [16383, 16383, 16383, 16383]\n"); // 32767*32767=1073676289 (high word=16383)
        printf("\n");
    }

    printf("PMULHW tests completed.\n");
    return 0;
}
