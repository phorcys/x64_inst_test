#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

void print_mmx(__m64 value, const char* name) {
    uint32_t* v = (uint32_t*)&value;
    printf("%s: [%08x %08x]\n", name, v[1], v[0]);
}

int main() {
    // Test data
    __m64 a = _mm_set_pi32(0x00000001, 0x00000002);
    __m64 b = _mm_set_pi32(1, 2);
    uint64_t mem = 0x0000000200000001ULL;
    
    printf("Testing MMX PSLLD instruction\n\n");
    
    // Test 1: mm, mm
    printf("Test 1: PSLLD mm, mm\n");
    __m64 res1 = _mm_sll_pi32(a, b);
    print_mmx(a, "a");
    print_mmx(b, "b");
    print_mmx(res1, "result");
    printf("\n");
    
    // Test 2: mm, imm8
    printf("Test 2: PSLLD mm, imm8\n");
    __m64 res2 = _mm_slli_pi32(a, 3);
    print_mmx(a, "a");
    print_mmx(res2, "result (shift by 3)");
    printf("\n");
    
    // Test 3: mm, m64
    printf("Test 3: PSLLD mm, m64\n");
    __m64 res3 = _mm_sll_pi32(a, *(__m64*)&mem);
    print_mmx(a, "a");
    printf("mem: [%016lx]\n", mem);
    print_mmx(res3, "result");
    printf("\n");
    
    // Edge cases
    printf("Edge case tests:\n");
    __m64 max = _mm_set_pi32(0x00000001, 0x00000001);
    __m64 shift = _mm_set_pi32(32, 31);
    
    // Test 4: edge values
    printf("Test 4: Edge values\n");
    __m64 res4 = _mm_sll_pi32(max, shift);
    print_mmx(max, "max");
    print_mmx(shift, "shift");
    print_mmx(res4, "result");
    
    _mm_empty();
    return 0;
}
