#include <stdio.h>
#include <stdint.h>
#include <mmintrin.h>

void print_m64(const char* label, __m64 value) {
    uint16_t* v = (uint16_t*)&value;
    printf("%s: %04x %04x %04x %04x\n",
           label, v[0], v[1], v[2], v[3]);
}

int main() {
    // Test PADDW with various combinations
    __m64 a, b, c;
    
    // Test case 1: Basic addition
    a = _mm_set_pi16(100, 200, 300, 400);
    b = _mm_set_pi16(400, 300, 200, 100);
    c = _m_paddw(a, b);
    print_m64("Case 1 a", a);
    print_m64("Case 1 b", b);
    print_m64("Case 1 result", c);
    printf("\n");

    // Test case 2: Overflow cases
    a = _mm_set_pi16(0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF);
    b = _mm_set_pi16(1, 1, 1, 1);
    c = _m_paddw(a, b);
    print_m64("Case 2 a", a);
    print_m64("Case 2 b", b);
    print_m64("Case 2 result", c);
    printf("\n");

    // Test case 3: Negative numbers
    a = _mm_set_pi16(-100, -200, -300, -400);
    b = _mm_set_pi16(-400, -300, -200, -100);
    c = _m_paddw(a, b);
    print_m64("Case 3 a", a);
    print_m64("Case 3 b", b);
    print_m64("Case 3 result", c);
    printf("\n");

    // Test case 4: Mixed positive and negative
    a = _mm_set_pi16(-32768, 32767, -16000, 16000);
    b = _mm_set_pi16(32767, -32768, 16000, -16000);
    c = _m_paddw(a, b);
    print_m64("Case 4 a", a);
    print_m64("Case 4 b", b);
    print_m64("Case 4 result", c);
    printf("\n");

    _mm_empty();
    return 0;
}
