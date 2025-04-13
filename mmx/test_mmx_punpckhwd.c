#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
void print_m64(const char* label, __m64 value) {
    uint16_t* v = (uint16_t*)&value;
    printf("%s: %04x %04x %04x %04x\n", 
           label, v[0], v[1], v[2], v[3]);
}

int main() {
    printf("=== Testing PUNPCKHWD ===\n");
    
    // Test case 1: Basic unpack with register operands
    {
        __m64 a = _mm_set_pi16(0x1111, 0x2222, 0x3333, 0x4444);
        __m64 b = _mm_set_pi16(0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic unpack\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x2222, 0xBBBB, 0x1111, 0xAAAA};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Zero values with register operands
    {
        __m64 a = _mm_set_pi16(0, 0, 0, 0);
        __m64 b = _mm_set_pi16(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Zero values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0, 0xFFFF, 0, 0xFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values with register operands
    {
        __m64 a = _mm_set_pi16(0x7FFF, 0x8000, 0xFFFF, 0x0000);
        __m64 b = _mm_set_pi16(0x0000, 0xFFFF, 0x8000, 0x7FFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x8000, 0xFFFF, 0x7FFF, 0x0000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi16(0x1234, 0x5678, 0x9ABC, 0xDEF0);
        __m64 b = _mm_set_pi16(0x0123, 0x4567, 0x89AB, 0xCDEF);
        __m64 res;
        
        asm volatile (
            "punpckhwd %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x5678, 0x4567, 0x1234, 0x0123};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Additional boundary tests
    {
        __m64 a = _mm_set_pi16(0xFFFF, 0x7FFF, 0x8000, 0x0000);
        __m64 b = _mm_set_pi16(0x0000, 0x8000, 0x7FFF, 0xFFFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhwd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 5: Boundary values test\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x7FFF, 0x8000, 0xFFFF, 0x0000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    // Test case 6: Memory operand with different addressing modes
    {
        uint16_t a_data[4] = {0x1111, 0x2222, 0x3333, 0x4444};
        uint16_t b_data[4] = {0x5555, 0x6666, 0x7777, 0x8888};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "punpckhwd %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 6: Memory operand with addressing\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint16_t expected[4] = {0x3333, 0x7777, 0x4444, 0x8888};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 6 PASSED\n");
        } else {
            printf("Test 6 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
