#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include <math.h>

#define TEST_CASE(title, a, b) \
    do { \
        __m128d res = _mm_hsub_pd(a, b); \
        int passed = 1; \
        for (int i = 0; i < 2; i++) { \
            if (!isnan(res[i])) { \
                double expected = (i == 0) ? (a[0] - a[1]) : (b[0] - b[1]); \
                double diff = fabs(res[i] - expected); \
                passed = passed && (diff < 0.001 || (isinf(res[i]) && isinf(expected))); \
            } \
        } \
        printf("Test: %s\n", title); \
        printf("Input A: [%.2f, %.2f]\n", a[0], a[1]); \
        printf("Input B: [%.2f, %.2f]\n", b[0], b[1]); \
        printf("Result: [%.2f, %.2f]\n", res[0], res[1]); \
        printf("Expected: [%.2f, %.2f]\n", a[0] - a[1], b[0] - b[1]); \
        printf("Test %s\n\n", passed ? "PASSED" : "FAILED"); \
    } while(0)

void test_reg_reg() {
    __m128d a = _mm_set_pd(1.0, 2.0);
    __m128d b = _mm_set_pd(3.0, 4.0);
    TEST_CASE("Register to Register", a, b);
}

void test_mem_reg() {
    double mem[2] = {5.0, 6.0};
    __m128d a = _mm_load_pd(mem);
    __m128d b = _mm_set_pd(7.0, 8.0);
    TEST_CASE("Memory to Register", a, b);
}

void test_special_values() {
    // Test with zeros
    __m128d zero = _mm_set_pd(0.0, 0.0);
    TEST_CASE("Zero values", zero, zero);

    // Test with negative values
    __m128d neg = _mm_set_pd(-1.0, -2.0);
    TEST_CASE("Negative values", neg, neg);

    // Test with mixed signs
    __m128d mixed = _mm_set_pd(1.0, -1.0);
    TEST_CASE("Mixed signs", mixed, mixed);

    // Test with large values
    __m128d large = _mm_set_pd(1.0e20, 1.0e20);
    TEST_CASE("Large values", large, large);

    // Test with small values
    __m128d small = _mm_set_pd(1.0e-20, 1.0e-20);
    TEST_CASE("Small values", small, small);

    // Test with NaN and Inf
    __m128d special = _mm_set_pd(NAN, INFINITY);
    TEST_CASE("Special values (NaN/Inf)", special, special);
}

int main() {
    printf("=== HSUBPD Instruction Test ===\n\n");
    
    test_reg_reg();
    test_mem_reg();
    test_special_values();

    printf("=== All tests completed ===\n");
    return 0;
}
