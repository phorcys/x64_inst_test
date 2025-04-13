#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64_words(const char* label, __m64 value) {
    uint16_t* words = (uint16_t*)&value;
    printf("%s: ", label);
    for(int i=0; i<4; i++) {
        printf("%04x ", words[i]);
    }
    printf("\n");
}

int main() {
    printf("=== Testing PCMPEQW ===\n");
    
    // Test case 1: All words equal
    {
        __m64 a = _mm_set_pi16(0x1111, 0x2222, 0x3333, 0x4444);
        __m64 b = _mm_set_pi16(0x1111, 0x2222, 0x3333, 0x4444);
        __m64 res;
        
        asm volatile (
            "pcmpeqw %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 1: All words equal\n");
        print_m64_words("A", a);
        print_m64_words("B", b);
        print_m64_words("Result", res);
        
        uint16_t expected[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: No words equal
    {
        __m64 a = _mm_set_pi16(0x1111, 0x2222, 0x3333, 0x4444);
        __m64 b = _mm_set_pi16(0x0000, 0x0000, 0x0000, 0x0000);
        __m64 res;
        
        asm volatile (
            "pcmpeqw %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 2: No words equal\n");
        print_m64_words("A", a);
        print_m64_words("B", b);
        print_m64_words("Result", res);
        
        uint16_t expected[4] = {0x0000, 0x0000, 0x0000, 0x0000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Some words equal
    {
        __m64 a = _mm_set_pi16(0x1111, 0x2222, 0x3333, 0x4444);
        __m64 b = _mm_set_pi16(0x1111, 0x0000, 0x3333, 0x0000);
        __m64 res;
        
        asm volatile (
            "pcmpeqw %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 3: Some words equal\n");
        print_m64_words("A", a);
        print_m64_words("B", b);
        print_m64_words("Result", res);
        
        uint16_t expected[4] = {0x0000, 0xFFFF, 0x0000, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Register to register
    {
        __m64 a = _mm_set_pi16(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD);
        __m64 b = _mm_set_pi16(0xAAAA, 0x0000, 0xCCCC, 0x0000);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpeqw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 4: Register to register\n");
        print_m64_words("A", a);
        print_m64_words("B", b);
        print_m64_words("Result", res);
        
        uint16_t expected[4] = {0x0000, 0xFFFF, 0x0000, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed values
    {
        uint16_t a_data[4] = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        uint16_t b_data[4] = {0x1234, 0x0000, 0x9ABC, 0x0000};
        uint16_t expected[4] = {0xFFFF, 0x0000, 0xFFFF, 0x0000};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "pcmpeqw %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 5: Random values\n");
        print_m64_words("A", a);
        print_m64_words("B", b);
        print_m64_words("Result", res);
        print_m64_words("Expected", *(__m64*)expected);
        
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
