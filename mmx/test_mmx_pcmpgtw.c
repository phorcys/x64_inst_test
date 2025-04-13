#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64(const char* label, __m64 value) {
    uint16_t* v = (uint16_t*)&value;
    printf("%s: %04x %04x %04x %04x\n", 
           label, v[0], v[1], v[2], v[3]);
}

int main() {
    printf("=== Testing PCMPGTW ===\n");
    srand(time(NULL));
    
    // Test case 1: Basic comparison
    {
        __m64 a = _mm_set_pi16(1000, 2000, 3000, 4000);
        __m64 b = _mm_set_pi16(500, 2500, 3000, 4500);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic comparison\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x0000, 0x0000, 0x0000, 0xffff};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Signed comparison
    {
        __m64 a = _mm_set_pi16(-1000, -500, 0, 500);
        __m64 b = _mm_set_pi16(-2000, -500, 0, 0);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Signed comparison\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0xFFFF, 0x0000, 0x0000, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values
    {
        __m64 a = _mm_set_pi16(0x7FFF, 0x8000, 0xFFFF, 0x0000);
        __m64 b = _mm_set_pi16(0x7FFE, 0x8001, 0xFFFE, 0x0001);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x0000, 0xFFFF, 0x0000, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi16(10000, 5000, 0, -5000);
        __m64 b = _mm_set_pi16(9999, 5000, 1, -4999);
        __m64 res;
        
        asm volatile (
            "pcmpgtw %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x0000, 0x0000, 0x0000, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed test values
    {
        uint16_t a_data[4] = {0x1234, 0x8000, 0x7FFF, 0xFFFF};
        uint16_t b_data[4] = {0x5678, 0x7FFF, 0x8000, 0x0000};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 5: Random values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4];
        for(int i=0; i<4; i++) {
            expected[i] = ((int16_t)a_data[i] > (int16_t)b_data[i]) ? 0xFFFF : 0x0000;
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
