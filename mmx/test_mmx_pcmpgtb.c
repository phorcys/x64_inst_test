#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64(const char* label, __m64 value) {
    uint8_t* v = (uint8_t*)&value;
    printf("%s: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
           label, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

int main() {
    printf("=== Testing PCMPGTB ===\n");
    
    // Test case 1: Basic comparison
    {
        __m64 a = _mm_set_pi8(80, 70, 60, 50, 40, 30, 20, 10);
        __m64 b = _mm_set_pi8(85, 70, 50, 55, 45, 30, 25, 5);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic comparison\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Signed comparison
    {
        __m64 a = _mm_set_pi8(0, -128, 127, 10, 5, 0, -5, -10);
        __m64 b = _mm_set_pi8(-1, -127, 126, 15, 0, 0, -5, -20);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Signed comparison\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values
    {
        __m64 a = _mm_set_pi8(0x80, 0x7F, 0xFE, 0x01, 0x00, 0xFF, 0x80, 0x7F);
        __m64 b = _mm_set_pi8(0x7F, 0x80, 0xFF, 0x00, 0x01, 0xFE, 0x81, 0x7E);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi8(0, -128, 127, -100, -50, 0, 50, 100);
        __m64 b = _mm_set_pi8(-1, -127, 126, -99, -49, 1, 50, 99);
        __m64 res;
        
        asm volatile (
            "pcmpgtb %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed values
    {
        uint8_t a_data[8] = {0x10, 0x80, 0xFF, 0x7F, 0x00, 0x01, 0xFE, 0x55};
        uint8_t b_data[8] = {0x20, 0x7F, 0xFE, 0x80, 0x01, 0x00, 0xFF, 0x55};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 5: Random values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8];
        for(int i=0; i<8; i++) {
            expected[i] = ((int8_t)a_data[i] > (int8_t)b_data[i]) ? 0xFF : 0x00;
        }
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
