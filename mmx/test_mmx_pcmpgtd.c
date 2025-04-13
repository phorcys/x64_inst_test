#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>
#include <time.h>

void print_m64(const char* label, __m64 value) {
    uint32_t* v = (uint32_t*)&value;
    printf("%s: %08x %08x\n", label, v[0], v[1]);
}

int main() {
    printf("=== Testing PCMPGTD ===\n");
    // Test case 1: Basic comparison
    {
        __m64 a = _mm_set_pi32(100000, 200000);
        __m64 b = _mm_set_pi32(50000, 250000);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic comparison\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2];
        expected[1] = ((int32_t)100000 > (int32_t)50000) ? 0xFFFFFFFF : 0x00000000;
        expected[0] = ((int32_t)200000 > (int32_t)250000) ? 0xFFFFFFFF : 0x00000000;
        printf("Expected: %08x %08x\n", expected[0], expected[1]);
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Signed comparison
    {
        __m64 a = _mm_set_pi32(-100000, -50000);
        __m64 b = _mm_set_pi32(-200000, -50000);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Signed comparison\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2];
        expected[1] = ((int32_t)-100000 > (int32_t)-200000) ? 0xFFFFFFFF : 0x00000000;
        expected[0] = ((int32_t)-50000 > (int32_t)-50000) ? 0xFFFFFFFF : 0x00000000;
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Boundary values
    {
        __m64 a = _mm_set_pi32(0x7FFFFFFF, 0x80000000);
        __m64 b = _mm_set_pi32(0x7FFFFFFE, 0x80000001);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "pcmpgtd %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 3: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2];
        expected[1] = ((int32_t)0x7FFFFFFF > (int32_t)0x7FFFFFFE) ? 0xFFFFFFFF : 0x00000000;
        expected[0] = ((int32_t)0x80000000 > (int32_t)0x80000001) ? 0xFFFFFFFF : 0x00000000;
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Memory operands
    {
        __m64 a = _mm_set_pi32(1000000, -1000000);
        __m64 b = _mm_set_pi32(999999, -999999);
        __m64 res;
        
        asm volatile (
            "pcmpgtd %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 4: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        uint32_t expected[2];
        expected[1] = ((int32_t)1000000 > (int32_t)999999) ? 0xFFFFFFFF : 0x00000000;
        expected[0] = ((int32_t)-1000000 > (int32_t)-999999) ? 0xFFFFFFFF : 0x00000000;
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 4 PASSED\n");
        } else {
            printf("Test 4 FAILED\n");
        }
    }
    
    // Test case 5: Fixed test vectors
    {
        uint32_t test_vectors[][4] = {
            {0x12345678, 0x87654321, 0x00000000, 0xFFFFFFFF}, // A < B, A > B
            {0x80000000, 0x7FFFFFFF, 0x00000000, 0xFFFFFFFF}, // Min int < Max int
            {0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000}, // -1 > 0
            {0x00000000, 0x00000000, 0x00000000, 0x00000000}  // Equal
        };
        
        for(int t=0; t<4; t++) {
            uint32_t a_data[2] = {test_vectors[t][0], test_vectors[t][1]};
            uint32_t b_data[2] = {test_vectors[t][2], test_vectors[t][3]};
            
            __m64 a = *(__m64*)a_data;
            __m64 b = *(__m64*)b_data;
            __m64 res;
            
            asm volatile (
                "movq %1, %%mm0\n\t"
                "movq %2, %%mm1\n\t"
                "pcmpgtd %%mm1, %%mm0\n\t"
                "movq %%mm0, %0\n\t"
                : "=m"(res)
                : "m"(a), "m"(b)
                : "mm0", "mm1"
            );
            
            printf("\nTest 5.%d: Fixed test vector\n", t+1);
            print_m64("Input A", a);
            print_m64("Input B", b);
            print_m64("Result", res);
            
            uint32_t expected[2];
            for(int i=0; i<2; i++) {
                expected[i] = ((int32_t)a_data[i] > (int32_t)b_data[i]) ? 0xFFFFFFFF : 0x00000000;
            }
            if(memcmp(&res, expected, 8) == 0) {
                printf("Test 5.%d PASSED\n", t+1);
            } else {
                printf("Test 5.%d FAILED\n", t+1);
            }
        }
    }
    
    _mm_empty();
    return 0;
}
