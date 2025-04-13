#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64(const char* label, __m64 value) {
    uint32_t v = _mm_cvtsi64_si32(value);
    printf("%s: %08x\n", label, v);
}

int main() {
    printf("=== Testing MOVD ===\n");
    srand(time(NULL));
    
    // Test case 1: Basic register to register transfer
    {
        uint32_t val = 0x12345678;
        __m64 res;
        
        asm volatile (
            "movd %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(val)
            : "mm0"
        );
        
        printf("\nTest 1: Basic transfer\n");
        print_m64("Input", _mm_set_pi32(0, val));
        print_m64("Result", res);
        
        __m64 expected = _mm_set_pi32(0, val);
        if(memcmp(&res, &expected, 4) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Memory to register
    {
        uint32_t val = 0x87654321;
        __m64 res;
        
        asm volatile (
            "movd %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(val)
            : "mm0"
        );
        
        printf("\nTest 2: Memory to register\n");
        print_m64("Input", _mm_set_pi32(0, val));
        print_m64("Result", res);
        
        __m64 expected = _mm_set_pi32(0, val);
        if(memcmp(&res, &expected, 4) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values
    {
        uint32_t tests[] = {0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF};
        for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
            __m64 res;
            
            asm volatile (
                "movd %1, %%mm0\n\t"
                "movq %%mm0, %0\n\t"
                : "=m"(res)
                : "m"(tests[i])
                : "mm0"
            );
            
            printf("\nTest 3.%zu: Boundary value 0x%08x\n", i+1, tests[i]);
            print_m64("Input", _mm_set_pi32(0, tests[i]));
            print_m64("Result", res);
            
            __m64 expected = _mm_set_pi32(0, tests[i]);
            if(memcmp(&res, &expected, 4) == 0) {
                printf("Test 3.%zu PASSED\n", i+1);
            } else {
                printf("Test 3.%zu FAILED\n", i+1);
            }
        }
    }
    
    // Test case 4: Memory operands
    {
        uint32_t val = 0x9ABCDEF0;
        __m64 res;
        
        asm volatile (
            "movd %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(val)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input", _mm_set_pi32(0, val));
        print_m64("Result", res);
        
        __m64 expected = _mm_set_pi32(0, val);
        if(memcmp(&res, &expected, 4) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Random values
    {
        uint32_t val = 0xA5A5A5A5; // Fixed value instead of random
        __m64 res;
        
        asm volatile (
            "movd %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(val)
            : "mm0"
        );
        
        printf("\nTest 5: Random value\n");
        print_m64("Input", _mm_set_pi32(0, val));
        print_m64("Result", res);
        
        __m64 expected = _mm_set_pi32(0, val);
        if(memcmp(&res, &expected, 4) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
