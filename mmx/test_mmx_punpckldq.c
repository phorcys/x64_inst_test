#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>

void print_m64(const char* label, __m64 value) {
    uint32_t* v = (uint32_t*)&value;
    printf("%-15s: [0]=%08x [1]=%08x\n", label, v[0], v[1]);
}

void print_flags() {
    unsigned long flags;
    asm volatile ("pushf\n\tpop %0" : "=r"(flags));
    printf("Flags: %08lx\n", flags);
}

int main() {
    printf("=== Testing PUNPCKLDQ (Unpack Low Doublewords) ===\n");
    
    // Test case 1: Basic unpack with register operands
    {
        __m64 a = _mm_set_pi32(0x11223344, 0x55667788);
        __m64 b = _mm_set_pi32(0xA1B2C3D4, 0xE5F60718);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckldq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic unpack\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x55667788, 0xE5F60718};
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
            "punpckldq %%mm1, %%mm0\n\t"
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
        __m64 a = _mm_set_pi32(0x80000000, 0x7FFFFFFF);
        __m64 b = _mm_set_pi32(0x7FFFFFFF, 0x80000000);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckldq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x7FFFFFFF, 0x80000000};
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
            "punpckldq %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2] = {0x9ABCDEF0, 0x89ABCDEF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Mixed values with memory operands
    {
        uint32_t a_data[2] = {0x12345678, 0x9ABCDEF0};
        uint32_t b_data[2] = {0x01234567, 0x89ABCDEF};
        
        __m64 a = *(__m64*)a_data;
        __m64 b = *(__m64*)b_data;
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "punpckldq %2, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0"
        );
        
        printf("\nTest 5: Mixed values with memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        uint32_t expected[2] = {a_data[0], b_data[0]};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 5 PASSED\n");
        } else {
            printf("Test 5 FAILED\n");
        }
    }
    
    // Test case 6: Max/Min values
    {
        __m64 a = _mm_set_pi32(0xFFFFFFFF, 0x00000000);
        __m64 b = _mm_set_pi32(0x00000000, 0xFFFFFFFF);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckldq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 6: Max/Min values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        uint32_t expected[2] = {0x00000000, 0xFFFFFFFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 6 PASSED\n");
        } else {
            printf("Test 6 FAILED\n");
        }
    }
    
    // Test case 7: Same values
    {
        __m64 a = _mm_set_pi32(0xAAAAAAAA, 0x55555555);
        __m64 b = _mm_set_pi32(0xAAAAAAAA, 0x55555555);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "punpckldq %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 7: Same values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        uint32_t expected[2] = {0x55555555, 0x55555555};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 7 PASSED\n");
        } else {
            printf("Test 7 FAILED\n");
        }
    }
    
    _mm_empty();
    return 0;
}
