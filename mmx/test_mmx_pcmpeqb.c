#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64_bytes(const char* label, __m64 value) {
    uint8_t* bytes = (uint8_t*)&value;
    printf("%s: ", label);
    for(int i=0; i<8; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

int main() {
    printf("=== Testing PCMPEQB ===\n");
    
    // Test case 1: All bytes equal
    {
        __m64 a = _mm_set_pi8(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88);
        __m64 b = _mm_set_pi8(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88);
        __m64 res;
        
        asm volatile (
            "pcmpeqb %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 1: All bytes equal\n");
        print_m64_bytes("A", a);
        print_m64_bytes("B", b);
        print_m64_bytes("Result", res);
        
        uint8_t expected[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: No bytes equal
    {
        __m64 a = _mm_set_pi8(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88);
        __m64 b = _mm_set_pi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        __m64 res;
        
        asm volatile (
            "pcmpeqb %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 2: No bytes equal\n");
        print_m64_bytes("A", a);
        print_m64_bytes("B", b);
        print_m64_bytes("Result", res);
        
        uint8_t expected[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Some bytes equal
    {
        __m64 a = _mm_set_pi8(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88);
        __m64 b = _mm_set_pi8(0x11, 0x00, 0x33, 0x00, 0x55, 0x00, 0x77, 0x00);
        __m64 res;
        
        asm volatile (
            "pcmpeqb %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 3: Some bytes equal\n");
        print_m64_bytes("A", a);
        print_m64_bytes("B", b);
        print_m64_bytes("Result", res);
        
        uint8_t expected[8] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Register to register
    {
        __m64 a = _mm_set_pi8(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11);
        __m64 b = _mm_set_pi8(0xAA, 0x00, 0xCC, 0x00, 0xEE, 0x00, 0x00, 0x11);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpeqb %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 4: Register to register\n");
        print_m64_bytes("A", a);
        print_m64_bytes("B", b);
        print_m64_bytes("Result", res);
        
        uint8_t expected[8] = {0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed values
    {
        uint8_t a_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        uint8_t b_data[8] = {0x11, 0x00, 0x33, 0xFF, 0x55, 0xAA, 0x77, 0x88};
        uint8_t expected[8] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "pcmpeqb %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 5: Random values\n");
        print_m64_bytes("A", a);
        print_m64_bytes("B", b);
        print_m64_bytes("Result", res);
        print_m64_bytes("Expected", *(__m64*)expected);
        
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
