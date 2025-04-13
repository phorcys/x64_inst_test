#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
void print_m64(const char* label, __m64 value) {
    uint8_t* v = (uint8_t*)&value;
    printf("%s: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
           label, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

int main() {
    printf("=== Testing PUNPCKHBW ===\n");
    
    // Test case 1: Basic unpack with register operands
    {
        __m64 a = _mm_set_pi8(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88);
        __m64 b = _mm_set_pi8(0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x07, 0x18);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhbw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic unpack\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x44, 0xD4, 0x33, 0xC3, 0x22, 0xB2, 0x11, 0xA1};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Zero values with register operands
    {
        __m64 a = _mm_set_pi8(0, 0, 0, 0, 0, 0, 0, 0);
        __m64 b = _mm_set_pi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhbw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Zero values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values with register operands
    {
        __m64 a = _mm_set_pi8(0x80, 0x7F, 0xFF, 0x00, 0x01, 0xFE, 0x7F, 0x80);
        __m64 b = _mm_set_pi8(0x7F, 0x80, 0x00, 0xFF, 0xFE, 0x01, 0x80, 0x7F);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhbw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x00, 0xFF, 0xFF, 0x00, 0x7F, 0x80, 0x80, 0x7F};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi8(0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0);
        __m64 b = _mm_set_pi8(0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF);
        __m64 res;
        
        asm volatile (
            "punpckhbw %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x78, 0x67, 0x56, 0x45, 0x34, 0x23, 0x12, 0x01};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Additional boundary tests
    {
        // Test with alternating 0x00 and 0xFF patterns
        __m64 a = _mm_set_pi8(0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF);
        __m64 b = _mm_set_pi8(0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhbw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 5: Alternating pattern test\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
            print_m64("Expected", *(__m64*)expected);
        }
    }
    
    // Test case 6: Memory operand with different addressing modes
    {
        uint8_t a_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        uint8_t b_data[8] = {0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "punpckhbw %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 6: Memory operand with addressing\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x55, 0xDD, 0x66, 0xEE, 0x77, 0xFF, 0x88, 0x00};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 6 PASSED\n");
        } else {
            printf("Test 6 FAILED\n");
            print_m64("Expected", *(__m64*)expected);
        }
    }

    // Test case 7: Signed values test
    {
        __m64 a = _mm_set_pi8(0x80, 0x7F, 0xFF, 0x00, 0x01, 0xFE, 0x40, 0xC0);
        __m64 b = _mm_set_pi8(0x7F, 0x80, 0x00, 0xFF, 0xFE, 0x01, 0xC0, 0x40);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckhbw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 7: Signed values test\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint8_t expected[8] = {0x00, 0xFF, 0xFF, 0x00, 0x7F, 0x80, 0x80, 0x7F};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 7 PASSED\n");
        } else {
            printf("Test 7 FAILED\n");
            print_m64("Expected", *(__m64*)expected);
        }
    }
    
    
    _mm_empty();
    return 0;
}
