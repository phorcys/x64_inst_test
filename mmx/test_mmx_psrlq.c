#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

void print_mmx(__m64 value, const char* name) {
    uint64_t v = *(uint64_t*)&value;
    printf("%s: [%016lx]\n", name, v);
}

int main() {
    // Test data
    __m64 a = _mm_set_pi64x(0x8000000000000000ULL);
    __m64 b = _mm_set_pi64x(1);
    uint64_t mem = 2;
    
    printf("Testing MMX PSRLQ instruction\n\n");
    
    // Test 1: mm, mm
    printf("Test 1: PSRLQ mm, mm\n");
    __m64 res1 = _mm_srl_si64(a, b);
    print_mmx(a, "a");
    print_mmx(b, "b");
    print_mmx(res1, "result");
    printf("\n");
    
    // Test 2: mm, imm8
    printf("Test 2: PSRLQ mm, imm8\n");
    __m64 res2 = _mm_srli_si64(a, 3);
    print_mmx(a, "a");
    print_mmx(res2, "result (shift by 3)");
    printf("\n");
    
    // Test 3: mm, m64
    printf("Test 3: PSRLQ mm, m64\n");
    __m64 res3 = _mm_srl_si64(a, *(__m64*)&mem);
    print_mmx(a, "a");
    printf("mem: [%016lx]\n", mem);
    print_mmx(res3, "result");
    printf("\n");
    
    // Edge cases
    printf("Edge case tests:\n");
    __m64 max = _mm_set_pi64x(0xFFFFFFFFFFFFFFFFULL);
    __m64 shift = _mm_set_pi64x(64);
    
    // Test 4: edge values
    printf("Test 4: Edge values\n");
    __m64 res4 = _mm_srl_si64(max, shift);
    print_mmx(max, "max");
    print_mmx(shift, "shift");
    print_mmx(res4, "result");
    
    _mm_empty();
    return 0;
}
