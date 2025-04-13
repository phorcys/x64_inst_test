#include <stdio.h>
#include <stdint.h>
#include <mmintrin.h>

void print_m64(const char* label, __m64 value) {
    uint8_t* v = (uint8_t*)&value;
    printf("%s: %02x %02x %02x %02x %02x %02x %02x %02x\n",
           label, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

int main() {
    // Test PADDB with various combinations
    __m64 a, b, c;
    
    // Test case 1: Basic addition
    a = _mm_set_pi8(1, 2, 3, 4, 5, 6, 7, 8);
    b = _mm_set_pi8(8, 7, 6, 5, 4, 3, 2, 1);
    c = _m_paddb(a, b);
    print_m64("Case 1 a", a);
    print_m64("Case 1 b", b);
    print_m64("Case 1 result", c);
    printf("\n");

    // Test case 2: Overflow cases
    a = _mm_set_pi8(0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F);
    b = _mm_set_pi8(1, 1, 1, 1, 1, 1, 1, 1);
    c = _m_paddb(a, b);
    print_m64("Case 2 a", a);
    print_m64("Case 2 b", b);
    print_m64("Case 2 result", c);
    printf("\n");

    // Test case 3: Negative numbers
    a = _mm_set_pi8(-1, -2, -3, -4, -5, -6, -7, -8);
    b = _mm_set_pi8(-8, -7, -6, -5, -4, -3, -2, -1);
    c = _m_paddb(a, b);
    print_m64("Case 3 a", a);
    print_m64("Case 3 b", b);
    print_m64("Case 3 result", c);
    printf("\n");

    // Test case 4: Mixed positive and negative
    a = _mm_set_pi8(-128, 127, -50, 50, -25, 25, -10, 10);
    b = _mm_set_pi8(127, -128, 50, -50, 25, -25, 10, -10);
    c = _m_paddb(a, b);
    print_m64("Case 4 a", a);
    print_m64("Case 4 b", b);
    print_m64("Case 4 result", c);
    printf("\n");

    _mm_empty();
    return 0;
}
