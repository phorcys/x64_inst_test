#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint16_t v128i __attribute__ ((vector_size (16)));

void print_m128i(v128i value, const char* label) {
    printf("%s: ", label);
    for (int i = 0; i < 8; i++) {
        printf("%04x ", value[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing PSRAW instruction (SSE2 inline assembly)\n\n");
    
    // Test case 1: Register shift by register
    {
        v128i a = {0x8000, 0x4000, 0x1234, 0x5678, 0x7FFF, 0x0001, 0xFFFF, 0x0000};
        v128i shift = {4, 4, 4, 4, 4, 4, 4, 4};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "psraw %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a), "m" (shift)
            : "xmm0", "xmm1"
        );

        printf("Test 1: Register shift by register\n");
        print_m128i(a, "Input");
        print_m128i(shift, "Shift");
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 2: Immediate shift
    {
        v128i a = {0x8000, 0x4000, 0x1234, 0x5678, 0x7FFF, 0x0001, 0xFFFF, 0x0000};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psraw $3, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 2: Immediate shift by 3\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 3: Memory operand
    {
        v128i a = {0x8000, 0x4000, 0x1234, 0x5678, 0x7FFF, 0x0001, 0xFFFF, 0x0000};
        uint64_t mem_shift = 5;
        v128i shift_vec = {5,5,5,5,5,5,5,5};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "psraw %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a), "m" (shift_vec)
            : "xmm0", "xmm1"
        );

        printf("Test 3: Memory operand shift by 5\n");
        print_m128i(a, "Input");
        printf("Shift value: %lu\n", mem_shift);
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 4: Edge cases
    {
        v128i a = {0x8000, 0x7FFF, 0x0000, 0xFFFF, 0x0001, 0xFFFE, 0x4000, 0xC000};
        v128i shift = {16, 16, 16, 16, 16, 16, 16, 16};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "psraw %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a), "m" (shift)
            : "xmm0", "xmm1"
        );

        printf("Test 4: Edge cases (shift by 16)\n");
        print_m128i(a, "Input");
        print_m128i(shift, "Shift");
        print_m128i(result, "Result");
        printf("\n");
    }
    // Test case 5: Edge Case Immediate shift
    {
        v128i a = {0x8000, 0x4000, 0x1234, 0x5678, 0x7FFF, 0x0001, 0xFFFF, 0x0000};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psraw $18, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 5: Edge Case Immediate shift by 18\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }


    printf("PSRAW tests completed\n");
    return 0;
}
