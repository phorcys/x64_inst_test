#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

void print_mmx(__m64 value, const char* name) {
    uint16_t* v = (uint16_t*)&value;
    printf("%s: [%04x %04x %04x %04x]\n", name, v[3], v[2], v[1], v[0]);
}

int main() {
    // Test data
    __m64 a = _mm_set_pi16(0x0001, 0x0002, 0x0004, 0x0008);
    __m64 b = _mm_set_pi16(1, 2, 3, 4);
    uint64_t mem = 0x0002000100040003ULL;
    
    printf("Testing MMX PSLLW instruction\n\n");
    
    // Test 1: mm, mm
    printf("Test 1: PSLLW mm, mm\n");
    __m64 res1 = _mm_sll_pi16(a, b);
    print_mmx(a, "a");
    print_mmx(b, "b");
    print_mmx(res1, "result");
    printf("\n");
    
    // Test 2: mm, imm8
    printf("Test 2: PSLLW mm, imm8\n");
    __m64 res2 = _mm_slli_pi16(a, 3);
    print_mmx(a, "a");
    print_mmx(res2, "result (shift by 3)");
    printf("\n");
    
    // Test 3: mm, m64
    printf("Test 3: PSLLW mm, m64\n");
    __m64 res3 = _mm_sll_pi16(a, *(__m64*)&mem);
    print_mmx(a, "a");
    printf("mem: [%016lx]\n", mem);
    print_mmx(res3, "result");
    printf("\n");
    
    // Edge cases
    printf("Edge case tests:\n");
    __m64 max = _mm_set_pi16(0x0001, 0x0001, 0x0001, 0x0001);
    __m64 shift = _mm_set_pi16(16, 15, 1, 0);
    
    // Test 4: edge values
    printf("Test 4: Edge values\n");
    __m64 res4 = _mm_sll_pi16(max, shift);
    print_mmx(max, "max");
    print_mmx(shift, "shift");
    print_mmx(res4, "result");
    
    _mm_empty();
    return 0;
}
