#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
void print_m64(const char* label, __m64 value) {
    uint32_t* v = (uint32_t*)&value;
    printf("%s: %08x %08x\n", label, v[0], v[1]);
}

int main() {
    printf("=== Testing PUNPCKHDQ ===\n");
    
    // Test case 1: Basic unpack with register operands
    {
        __m64 a = _mm_set_pi32(0x11111111, 0x22222222);
        __m64 b = _mm_set_pi32(0xAAAAAAAA, 0xBBBBBBBB);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhdq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic unpack\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x11111111, 0xAAAAAAAA};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Zero values with register operands
    {
        __m64 a = _mm_set_pi32(0, 0);
        __m64 b = _mm_set_pi32(0xFFFFFFFF, 0xFFFFFFFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhdq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Zero values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0, 0xFFFFFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values with register operands
    {
        __m64 a = _mm_set_pi32(0x7FFFFFFF, 0x80000000);
        __m64 b = _mm_set_pi32(0x00000000, 0xFFFFFFFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhdq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x7FFFFFFF, 0x00000000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi32(0x12345678, 0x9ABCDEF0);
        __m64 b = _mm_set_pi32(0x01234567, 0x89ABCDEF);
        __m64 res;
        
        asm volatile (
            "punpckhdq %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x12345678, 0x01234567};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Additional boundary tests
    {
        __m64 a = _mm_set_pi32(0xFFFFFFFF, 0x7FFFFFFF);
        __m64 b = _mm_set_pi32(0x80000000, 0x00000000);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhdq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 5: Boundary values test\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0xFFFFFFFF, 0x80000000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
            print_m64("Expected", *(__m64*)expected);
        }
    }
    
    // Test case 6: Memory operand with different addressing modes
    {
        uint32_t a_data[2] = {0x11111111, 0x22222222};
        uint32_t b_data[2] = {0x33333333, 0x44444444};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "punpckhdq %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 6: Memory operand with addressing\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x22222222, 0x44444444};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 6 PASSED\n");
        } else {
            printf("Test 6 FAILED\n");
            print_m64("Expected", *(__m64*)expected);
        }
    }
    
    _mm_empty();
    return 0;
}
