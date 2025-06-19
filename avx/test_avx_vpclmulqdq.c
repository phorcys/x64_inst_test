#include <stdio.h>
#include <stdint.h>
#include "avx.h"

static void verify_result(__m128i result, __m128i expected) {
    if (_mm_test_all_zeros(_mm_xor_si128(result, expected), _mm_set1_epi32(-1))) {
        printf("[PASS]\n");
    } else {
        printf("[FAIL] - Result mismatch\n");
        print_m128i_hex(expected, "Expected");
    }
}

int main() {
    printf("Comprehensive VPCLMULQDQ instruction test\n");
    printf("=======================================\n");

    // Test 1: All 1s multiply (imm8=0x00)
    {
        __m128i a = _mm_set1_epi64x(~0ULL);
        __m128i b = _mm_set1_epi64x(~0ULL);
        
        __m128i result;
        asm volatile("vpclmulqdq $0x00, %2, %1, %0" : "=x"(result) : "x"(a), "x"(b));
        
        printf("\nTest 1: All 1s multiply (imm8=0x00)\n");
        print_m128i_hex(a, "Operand A");
        print_m128i_hex(b, "Operand B");
        print_m128i_hex(result, "Result");
        
        __m128i expected = _mm_set1_epi8(0x55); // Polynomial multiplication result
        verify_result(result, expected);
    }

    // Test 2: High*Low (imm8=0x01)
    {
        __m128i a = _mm_setr_epi64x(0x1, 0x2);
        __m128i b = _mm_setr_epi64x(0x3, 0x4);
        
        __m128i result;
        asm volatile("vpclmulqdq $0x01, %2, %1, %0" : "=x"(result) : "x"(a), "x"(b));
        
        printf("\nTest 2: High*Low (imm8=0x01)\n");
        print_m128i_hex(a, "Operand A");
        print_m128i_hex(b, "Operand B");
        print_m128i_hex(result, "Result");
        
        __m128i expected = _mm_setr_epi64x(0, 0x4);
        verify_result(result, expected);
    }

    // Test 3: Low*High (imm8=0x10)
    {
        __m128i a = _mm_setr_epi64x(0x1, 0x2);
        __m128i b = _mm_setr_epi64x(0x3, 0x4);
        
        __m128i result;
        asm volatile("vpclmulqdq $0x10, %2, %1, %0" : "=x"(result) : "x"(a), "x"(b));
        
        printf("\nTest 3: Low*High (imm8=0x10)\n");
        print_m128i_hex(a, "Operand A");
        print_m128i_hex(b, "Operand B");
        print_m128i_hex(result, "Result");
        
        __m128i expected = _mm_setr_epi64x(0, 0x6);
        verify_result(result, expected);
    }

    // Test 4: High*High (imm8=0x11)
    {
        __m128i a = _mm_setr_epi64x(0x1, 0x2);
        __m128i b = _mm_setr_epi64x(0x3, 0x4);
        
        __m128i result;
        asm volatile("vpclmulqdq $0x11, %2, %1, %0" : "=x"(result) : "x"(a), "x"(b));
        
        printf("\nTest 4: High*High (imm8=0x11)\n");
        print_m128i_hex(a, "Operand A");
        print_m128i_hex(b, "Operand B");
        print_m128i_hex(result, "Result");
        
        __m128i expected = _mm_setr_epi64x(0, 0x3);
        verify_result(result, expected);
    }

    // Test 5: Boundary values (MSB set)
    {
        __m128i a = _mm_setr_epi64x(0x8000000000000000, 0);
        __m128i b = _mm_setr_epi64x(0x8000000000000000, 0);
        
        __m128i result;
        asm volatile("vpclmulqdq $0x00, %2, %1, %0" : "=x"(result) : "x"(a), "x"(b));
        
        printf("\nTest 5: MSB boundary test (imm8=0x00)\n");
        print_m128i_hex(a, "Operand A");
        print_m128i_hex(b, "Operand B");
        print_m128i_hex(result, "Result");
        
        __m128i expected = _mm_setr_epi64x(0, 0);
        verify_result(result, expected);
    }

    printf("\nVPCLMULQDQ tests completed.\n");
    return 0;
}
