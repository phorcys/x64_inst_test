#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64(const char* label, __m64 value) {
    int8_t* v = (int8_t*)&value;
    printf("%s: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
           label, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

int main() {
    printf("=== Testing PACKSSWB ===\n");
    
    // Test case 1: Basic packing with register operands
    {
        __m64 a = _mm_set_pi16(0x0034, 0x00FF, 0x0000, 0x00CD);
        __m64 b = _mm_set_pi16(0x0078, 0x0080, 0x007F, 0x0034);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "packsswb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic packing\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x7f, 0x00, 0x7f, 0x34, 
                             0x34, 0x7f, 0x7f, 0x78};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Boundary values with register operands
    {
        __m64 a = _mm_set_pi16(0x0000, 0x7F80, 0x807F, 0xFFFF);
        __m64 b = _mm_set_pi16(0x7FFF, 0x8000, 0x0000, 0x7F80);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "packsswb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0xff, 0x80, 0x7f, 0x00,
                             0x7f, 0x00, 0x80, 0x7F};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Memory operands
    {
        __m64 a = _mm_set_pi16(0x1234, 0x5678, 0x9ABC, 0xDEF0);
        __m64 b = _mm_set_pi16(0x0123, 0x4567, 0x89AB, 0xCDEF);
        __m64 res;
        
        asm volatile (
            "packsswb %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 3: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x80, 0x80, 0x7f, 0x7f,
                               0x80, 0x80, 0x7f, 0x7f};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Fixed values
    {
        int16_t a_data[4] = {0x7F80, 0x8000, 0x1234, 0x5678};
        int16_t b_data[4] = {0x0000, 0x7FFF, 0x9ABC, 0xDEF0};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "packsswb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 4: Random values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        int8_t expected[8] = {0x7f, 0x80, 0x7f, 0x7f,
                             0x00, 0x7f, 0x80, 0x80};
        
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
