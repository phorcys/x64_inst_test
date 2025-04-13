#include <stdio.h>
#include <stdint.h>
#include <mmintrin.h>

void print_m64(const char* label, __m64 value) {
    uint32_t* v = (uint32_t*)&value;
    printf("%s: %08x %08x\n", label, v[0], v[1]);
}

int main() {
    // Test PADDD with various combinations
    __m64 a, b, c;
    
    // Test case 1: Basic addition
    a = _mm_set_pi32(100000, 200000);
    b = _mm_set_pi32(200000, 100000);
    c = _m_paddd(a, b);
    print_m64("Case 1 a", a);
    print_m64("Case 1 b", b);
    print_m64("Case 1 result", c);
    printf("\n");

    // Test case 2: Overflow cases
    a = _mm_set_pi32(0x7FFFFFFF, 0x7FFFFFFF);
    b = _mm_set_pi32(1, 1);
    c = _m_paddd(a, b);
    print_m64("Case 2 a", a);
    print_m64("Case 2 b", b);
    print_m64("Case 2 result", c);
    printf("\n");

    // Test case 3: Negative numbers
    a = _mm_set_pi32(-100000, -200000);
    b = _mm_set_pi32(-200000, -100000);
    c = _m_paddd(a, b);
    print_m64("Case 3 a", a);
    print_m64("Case 3 b", b);
    print_m64("Case 3 result", c);
    printf("\n");

    // Test case 4: Mixed positive and negative
    a = _mm_set_pi32(-2147483648, 2147483647);
    b = _mm_set_pi32(2147483647, -2147483648);
    c = _m_paddd(a, b);
    print_m64("Case 4 a", a);
    print_m64("Case 4 b", b);
    print_m64("Case 4 result", c);
    printf("\n");

    _mm_empty();
    return 0;
}
