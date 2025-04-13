#include <stdio.h>
#include <stdint.h>
#include <string.h>

void print_mmx_bytes(const char* name, const uint64_t value) {
    int8_t v[8];
    memcpy(v, &value, sizeof(v));
    printf("%s: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           name, v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
}

int main() {
    printf("Testing PSUBSB (Packed Subtract Signed Byte with Saturation)\n");
    printf("=========================================================\n\n");

    // Test case 1: Normal subtraction
    {
        uint64_t a = 0x050403020100FFFE; // [5,4,3,2,1,0,-1,-2]
        uint64_t b = 0x0101010101010101; // [1,1,1,1,1,1,1,1]
        uint64_t result;

        printf("Test 1: Normal subtraction\n");
        print_mmx_bytes("Input A", a);
        print_mmx_bytes("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "psubsb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_bytes("Result", result);
        printf("Expected: [4,3,2,1,0,-1,-2,-3]\n");
        printf("\n");
    }

    // Test case 2: Positive overflow (saturate to 127)
    {
        uint64_t a = 0x7F7F7F7F7F7F7F7F; // [127,127,127,127,127,127,127,127]
        uint64_t b = 0x8080808080808080; // [-128,-128,-128,-128,-128,-128,-128,-128]
        uint64_t result;

        printf("Test 2: Positive overflow (saturate to 127)\n");
        print_mmx_bytes("Input A", a);
        print_mmx_bytes("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "psubsb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_bytes("Result", result);
        printf("Expected: [127,127,127,127,127,127,127,127]\n");
        printf("\n");
    }

    // Test case 3: Negative overflow (saturate to -128)
    {
        uint64_t a = 0x8080808080808080; // [-128,-128,-128,-128,-128,-128,-128,-128]
        uint64_t b = 0x7F7F7F7F7F7F7F7F; // [127,127,127,127,127,127,127,127]
        uint64_t result;

        printf("Test 3: Negative overflow (saturate to -128)\n");
        print_mmx_bytes("Input A", a);
        print_mmx_bytes("Input B", b);

        __asm__ volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "psubsb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            "emms"
            : "=m" (result)
            : "m" (a), "m" (b)
        );

        print_mmx_bytes("Result", result);
        printf("Expected: [-128,-128,-128,-128,-128,-128,-128,-128]\n");
        printf("\n");
    }

    printf("PSUBSB tests completed.\n");
    return 0;
}
