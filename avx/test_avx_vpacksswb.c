#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// VPACKSSWB (128-bit version)
static void test_vpacksswb128() {
    printf("Testing VPACKSSWB (128-bit)\n");
    
    // Test case 1: Normal values
    {
        int16_t a[8] = {100, 200, 300, 400, -100, -200, -300, -400};
        int16_t b[8] = {50, 150, 250, 350, -50, -150, -250, -350};
        int8_t expected[16] = {100, 127, 127, 127, -100, -128, -128, -128,
                              50, 127, 127, 127, -50, -128, -128, -128};
        
        __m128i va = _mm_loadu_si128((__m128i*)a);
        __m128i vb = _mm_loadu_si128((__m128i*)b);
        __m128i result;
        
        // Inline assembly implementation
        __asm__ __volatile__ (
            "vpacksswb %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (va), [b] "x" (vb)
        );
        
        int8_t res[16];
        _mm_storeu_si128((__m128i*)res, result);
        
        printf("Test case 1:\n");
        print_xmm("Input A", va);
        print_xmm("Input B", vb);
        print_xmm("Result", result);
        
        int passed = 1;
        for(int i=0; i<16; i++) {
            if(res[i] != expected[i]) {
                printf("Mismatch at position %d: got %d, expected %d\n", 
                      i, res[i], expected[i]);
                passed = 0;
            }
        }
        printf("Test case 1: %s\n\n", passed ? "PASSED" : "FAILED");
    }
    
    // Test case 2: Boundary values
    {
        int16_t a[8] = {32767, 32767, -32768, -32768, 0, 0, 127, -128};
        int16_t b[8] = {127, -128, 0, 0, 32767, -32768, 32767, -32768};
        int8_t expected[16] = {127, 127, -128, -128, 0, 0, 127, -128,
                              127, -128, 0, 0, 127, -128, 127, -128};
        
        __m128i va = _mm_loadu_si128((__m128i*)a);
        __m128i vb = _mm_loadu_si128((__m128i*)b);
        __m128i result;
        
        __asm__ __volatile__ (
            "vpacksswb %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (va), [b] "x" (vb)
        );
        
        int8_t res[16];
        _mm_storeu_si128((__m128i*)res, result);
        
        printf("Test case 2:\n");
        print_xmm("Input A", va);
        print_xmm("Input B", vb);
        print_xmm("Result", result);
        
        int passed = 1;
        for(int i=0; i<16; i++) {
            if(res[i] != expected[i]) {
                printf("Mismatch at position %d: got %d, expected %d\n", 
                      i, res[i], expected[i]);
                passed = 0;
            }
        }
        printf("Test case 2: %s\n\n", passed ? "PASSED" : "FAILED");
    }
}

// VPACKSSWB (256-bit version)
static void test_vpacksswb256() {
    printf("Testing VPACKSSWB (256-bit)\n");
    
    // Test case 1: Normal values
    {
        int16_t a[16] = {100, 200, 300, 400, -100, -200, -300, -400,
                         150, 250, 350, 450, -150, -250, -350, -450};
        int16_t b[16] = {50, 150, 250, 350, -50, -150, -250, -350,
                        75, 175, 275, 375, -75, -175, -275, -375};
        int8_t expected[32] = {100, 127, 127, 127, -100, -128, -128, -128,
                              50, 127, 127, 127, -50, -128, -128, -128,
                              127, 127, 127, 127, -128, -128, -128, -128,
                              75, 127, 127, 127, -75, -128, -128, -128};
        
        __m256i va = _mm256_loadu_si256((__m256i*)a);
        __m256i vb = _mm256_loadu_si256((__m256i*)b);
        __m256i result;
        
        // Inline assembly implementation
        __asm__ __volatile__ (
            "vpacksswb %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (va), [b] "x" (vb)
        );
        
        int8_t res[32];
        _mm256_storeu_si256((__m256i*)res, result);
        
        printf("Test case 1:\n");
        print_ymm("Input A", va);
        print_ymm("Input B", vb);
        print_ymm("Result", result);
        
        int passed = 1;
        for(int i=0; i<32; i++) {
            if(res[i] != expected[i]) {
                printf("Mismatch at position %d: got %d, expected %d\n", 
                      i, res[i], expected[i]);
                passed = 0;
            }
        }
        printf("Test case 1: %s\n\n", passed ? "PASSED" : "FAILED");
    }
    
    // Test case 2: Boundary values
    {
        int16_t a[16] = {32767, 32767, -32768, -32768, 0, 0, 127, -128,
                         32767, -32768, 32767, -32768, 127, -128, 127, -128};
        int16_t b[16] = {127, -128, 0, 0, 32767, -32768, 32767, -32768,
                        0, 0, 127, -128, 32767, -32768, 32767, -32768};
        int8_t expected[32] = {127, 127, -128, -128, 0, 0, 127, -128,
                              127, -128, 0, 0, 127, -128, 127, -128,
                              127, -128, 127, -128, 127, -128, 127, -128,
                              0, 0, 127, -128, 127, -128, 127, -128};
        
        __m256i va = _mm256_loadu_si256((__m256i*)a);
        __m256i vb = _mm256_loadu_si256((__m256i*)b);
        __m256i result;
        
        __asm__ __volatile__ (
            "vpacksswb %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (va), [b] "x" (vb)
        );
        
        int8_t res[32];
        _mm256_storeu_si256((__m256i*)res, result);
        
        printf("Test case 2:\n");
        print_ymm("Input A", va);
        print_ymm("Input B", vb);
        print_ymm("Result", result);
        
        int passed = 1;
        for(int i=0; i<32; i++) {
            if(res[i] != expected[i]) {
                printf("Mismatch at position %d: got %d, expected %d\n", 
                      i, res[i], expected[i]);
                passed = 0;
            }
        }
        printf("Test case 2: %s\n\n", passed ? "PASSED" : "FAILED");
    }
}

int main() {
    test_vpacksswb128();
    test_vpacksswb256();
    return 0;
}
