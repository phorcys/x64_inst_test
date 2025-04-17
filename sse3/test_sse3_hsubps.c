#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include <math.h>

#define TEST_CASE(title, a, b) \
    do { \
        __m128 res = _mm_hsub_ps(a, b); \
        int passed = 1; \
        for (int i = 0; i < 4; i++) { \
            if (!isnan(res[i])) { \
                double expected = (i < 2) ? (a[i*2] - a[i*2+1]) : (b[(i-2)*2] - b[(i-2)*2+1]); \
                double diff = fabs(res[i] - expected); \
                passed = passed && (diff < 0.001 || (isinf(res[i]) && isinf(expected))); \
            } \
        } \
        printf("Test: %s\n", title); \
        printf("Input A: [%.2f, %.2f, %.2f, %.2f]\n", a[0], a[1], a[2], a[3]); \
        printf("Input B: [%.2f, %.2f, %.2f, %.2f]\n", b[0], b[1], b[2], b[3]); \
        printf("Result: [%.2f, %.2f, %.2f, %.2f]\n", res[0], res[1], res[2], res[3]); \
        printf("Expected: [%.2f, %.2f, %.2f, %.2f]\n", \
              a[0] - a[1], a[2] - a[3], b[0] - b[1], b[2] - b[3]); \
        printf("Test %s\n\n", passed ? "PASSED" : "FAILED"); \
    } while(0)

void test_reg_reg() {
    __m128 a = _mm_set_ps(1.0, 2.0, 3.0, 4.0);
    __m128 b = _mm_set_ps(5.0, 6.0, 7.0, 8.0);
    TEST_CASE("Register to Register", a, b);
}

void test_mem_reg() {
    float mem[4] = {9.0, 10.0, 11.0, 12.0};
    __m128 a = _mm_load_ps(mem);
    __m128 b = _mm_set_ps(13.0, 14.0, 15.0, 16.0);
    TEST_CASE("Memory to Register", a, b);
}

void test_special_values() {
    // Test with zeros
    __m128 zero = _mm_set_ps(0.0, 0.0, 0.0, 0.0);
    TEST_CASE("Zero values", zero, zero);

    // Test with negative values
    __m128 neg = _mm_set_ps(-1.0, -2.0, -3.0, -4.0);
    TEST_CASE("Negative values", neg, neg);

    // Test with mixed signs
    __m128 mixed = _mm_set_ps(1.0, -1.0, 2.0, -2.0);
    TEST_CASE("Mixed signs", mixed, mixed);

    // Test with large values
    __m128 large = _mm_set_ps(1.0e20, 1.0e20, 1.0e20, 1.0e20);
    TEST_CASE("Large values", large, large);

    // Test with small values
    __m128 small = _mm_set_ps(1.0e-20, 1.0e-20, 1.0e-20, 1.0e-20);
    TEST_CASE("Small values", small, small);

    // Test with NaN and Inf
    __m128 special = _mm_set_ps(NAN, INFINITY, -INFINITY, 0.0);
    TEST_CASE("Special values (NaN/Inf)", special, special);
}

int main() {
    printf("=== HSUBPS Instruction Test ===\n\n");
    
    test_reg_reg();
    test_mem_reg();
    test_special_values();

    printf("=== All tests completed ===\n");
    return 0;
}
