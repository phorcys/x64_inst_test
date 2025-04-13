#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64_dwords(const char* label, __m64 value) {
    uint32_t* dwords = (uint32_t*)&value;
    printf("%s: %08x %08x\n", label, dwords[0], dwords[1]);
}

int main() {
    printf("=== Testing PCMPEQD ===\n");
    
    // Test case 1: All dwords equal
    {
        __m64 a = _mm_set_pi32(0x11111111, 0x22222222);
        __m64 b = _mm_set_pi32(0x11111111, 0x22222222);
        __m64 res;
        
        asm volatile (
            "pcmpeqd %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 1: All dwords equal\n");
        print_m64_dwords("A", a);
        print_m64_dwords("B", b);
        print_m64_dwords("Result", res);
        
        uint32_t expected[2] = {0xFFFFFFFF, 0xFFFFFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: No dwords equal
    {
        __m64 a = _mm_set_pi32(0x11111111, 0x22222222);
        __m64 b = _mm_set_pi32(0x00000000, 0x00000000);
        __m64 res;
        
        asm volatile (
            "pcmpeqd %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 2: No dwords equal\n");
        print_m64_dwords("A", a);
        print_m64_dwords("B", b);
        print_m64_dwords("Result", res);
        
        uint32_t expected[2] = {0x00000000, 0x00000000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: One dword equal
    {
        __m64 a = _mm_set_pi32(0x11111111, 0x22222222);
        __m64 b = _mm_set_pi32(0x11111111, 0x00000000);
        __m64 res;
        
        asm volatile (
            "pcmpeqd %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 3: One dword equal\n");
        print_m64_dwords("A", a);
        print_m64_dwords("B", b);
        print_m64_dwords("Result", res);
        
        uint32_t expected[2] = {0x00000000, 0xFFFFFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Register to register
    {
        __m64 a = _mm_set_pi32(0xBBBBBBBB, 0xAAAAAAAA);
        __m64 b = _mm_set_pi32(0x00000000, 0xAAAAAAAA);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpeqd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 4: Register to register\n");
        print_m64_dwords("A", a);
        print_m64_dwords("B", b);
        print_m64_dwords("Result", res);
        
        uint32_t expected[2] = {0xFFFFFFFF, 0x00000000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed values
    {
        uint32_t a_data[2] = {0x12345678, 0x9ABCDEF0};
        uint32_t b_data[2] = {0x12345678, 0x00000000};
        uint32_t expected[2] = {0xFFFFFFFF, 0x00000000};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "pcmpeqd %1, %0\n\t"
            : "=y"(res)
            : "y"(a), "0"(b)
        );
        
        printf("\nTest 5: Random values\n");
        print_m64_dwords("A", a);
        print_m64_dwords("B", b);
        print_m64_dwords("Result", res);
        print_m64_dwords("Expected", *(__m64*)expected);
        
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
