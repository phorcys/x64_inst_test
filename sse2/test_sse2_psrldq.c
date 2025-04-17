#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t v128i __attribute__ ((vector_size (16)));

void print_m128i(v128i value, const char* label) {
    printf("%s: ", label);
    for (int i = 0; i < 16; i++) {
        printf("%02x ", value[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing PSRLDQ instruction (using inline assembly)\n\n");
    
    // Test case 1: Shift by 1 byte
    {
        v128i a = {0x00, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99,
                  0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psrldq $1, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 1: Shift right by 1 byte\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 2: Shift by 4 bytes
    {
        v128i a = {0x00, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99,
                  0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psrldq $4, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 2: Shift right by 4 bytes\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 3: Shift by 8 bytes
    {
        v128i a = {0x00, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99,
                  0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psrldq $8, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 3: Shift right by 8 bytes\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 4: Shift by 15 bytes (maximum)
    {
        v128i a = {0x00, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99,
                  0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psrldq $15, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 4: Shift right by 15 bytes\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }

    // Test case 4.1: Shift by 16 bytes (maximum)
    {
        v128i a = {0x00, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99,
                  0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        v128i result;
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psrldq $16, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 4.1: Shift right by 15 bytes\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }    

    // Test case 5: Memory operand
    {
        uint8_t mem_data[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                               0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        v128i a, result;
        memcpy(&a, mem_data, sizeof(a));
        
        asm volatile (
            "movdqu %1, %%xmm0\n\t"
            "psrldq $2, %%xmm0\n\t"
            "movdqu %%xmm0, %0"
            : "=m" (result)
            : "m" (a)
            : "xmm0"
        );

        printf("Test 5: Memory operand, shift by 2 bytes\n");
        print_m128i(a, "Input");
        print_m128i(result, "Result");
        printf("\n");
    }

    printf("PSRLDQ tests completed\n");
    return 0;
}
