#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>

void print_m64(const char* label, __m64 value) {
    uint32_t* v = (uint32_t*)&value;
    printf("%s: %08x %08x\n", label, v[0], v[1]);
}

int main() {
    printf("=== Testing MOVQ ===\n");
    // Test case 1: Basic register to register transfer
    {
        __m64 a = _mm_set_pi32(0x12345678, 0x9ABCDEF0);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a)
            : "mm0"
        );
        
        printf("\nTest 1: Basic transfer\n");
        print_m64("Input", a);
        print_m64("Result", res);
        
        if(memcmp(&a, &res, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Memory to register
    {
        __m64 a = _mm_set_pi32(0x87654321, 0xFEDCBA98);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a)
            : "mm0"
        );
        
        printf("\nTest 2: Memory to register\n");
        print_m64("Input", a);
        print_m64("Result", res);
        
        if(memcmp(&a, &res, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values
    {
        __m64 tests[] = {
            _mm_set_pi32(0x00000000, 0x00000000),
            _mm_set_pi32(0xFFFFFFFF, 0xFFFFFFFF),
            _mm_set_pi32(0x80000000, 0x7FFFFFFF),
            _mm_set_pi32(0x7FFFFFFF, 0x80000000)
        };
        
        for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
            __m64 res;
            
            asm volatile (
                "movq %1, %%mm0\n\t"
                "movq %%mm0, %0\n\t"
                : "=m"(res)
                : "m"(tests[i])
                : "mm0"
            );
            
            printf("\nTest 3.%zu: Boundary values\n", i+1);
            print_m64("Input", tests[i]);
            print_m64("Result", res);
            
            if(memcmp(&tests[i], &res, 8) == 0) {
                printf("Test 3.%zu PASSED\n", i+1);
            } else {
                printf("Test 3.%zu FAILED\n", i+1);
            }
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi32(0x13579BDF, 0x2468ACE0);
        __m64 res;
        
        asm volatile (
            "movq %1, %0\n\t"
            : "=y"(res)
            : "m"(a)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input", a);
        print_m64("Result", res);
        
        if(memcmp(&a, &res, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed pattern values
    {
        __m64 tests[] = {
            _mm_set_pi32(0xAAAAAAAA, 0x55555555),
            _mm_set_pi32(0x12345678, 0x9ABCDEF0),
            _mm_set_pi32(0xF0F0F0F0, 0x0F0F0F0F)
        };
        
        for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
            __m64 a = tests[i];
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a)
            : "mm0"
        );
        
            printf("\nTest 5.%zu: Fixed pattern\n", i+1);
            print_m64("Input", a);
            print_m64("Result", res);
            
            if(memcmp(&a, &res, 8) == 0) {
                printf("Test 5.%zu PASSED\n", i+1);
            } else {
                printf("Test 5.%zu FAILED\n", i+1);
            }
        }
    }
    
    _mm_empty();
    return 0;
}
