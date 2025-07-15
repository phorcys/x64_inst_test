#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void test_vshufpd_128() {
    printf("=== Testing vshufpd (128-bit) ===\n");
    
    double a[2] ALIGNED(16) = {1.0, 2.0};
    double b[2] ALIGNED(16) = {3.0, 4.0};
    double dst[2] ALIGNED(16) = {0};
    double expected[2] ALIGNED(16) = {0};
    
    // Test case 1: Select a[0] and b[0]
    __m128d v_a = _mm_load_pd(a);
    __m128d v_b = _mm_load_pd(b);
    __m128d v_dst;
#define TESTVSHUFPS(imm) \
    asm volatile( \
        "vshufpd $" #imm ", %2, %1, %0"  \
        : "=x"(v_dst) \
        : "x"(v_a), "x"(v_b) \
    );   \
    _mm_store_pd(dst, v_dst); \
    printf("Shuffle pattern: 0x%X\n", imm); \
    print_double_vec("Input A", a, 2); \
    print_double_vec("Input B", b, 2); \
    print_double_vec("Result", dst, 2); \
    print_double_vec("Expected", (double[]){ \
        (imm & 0x1) ? a[1] : a[0], \
        (imm & 0x2) ? b[1] : b[0], \
    }, 2); \
    printf("Test %d: %s\n", imm, memcmp(dst, (double[]){ \
        (imm & 0x1) ? a[1] : a[0], \
        (imm & 0x2) ? b[1] : b[0], \
    }, sizeof(double)*2) == 0 ? "PASS\n\n" : "FAIL\n\n");

    TESTVSHUFPS(0x0)  // 00 pattern (low:00, high:00)
    TESTVSHUFPS(0x1)  // 01 pattern (low:01, high:01)
    TESTVSHUFPS(0x2)  // 10 pattern (low:10, high:10)
    TESTVSHUFPS(0x3)  // 11 pattern (low:11, high:11)
    TESTVSHUFPS(0x4)  // 00 pattern (low:00, high:10)
    TESTVSHUFPS(0x5)  // 01 pattern (low:01, high:10)
    TESTVSHUFPS(0x6)  // 10 pattern (low:10, high:01)
    TESTVSHUFPS(0x7)  // 11 pattern (low:11, high:01)
    TESTVSHUFPS(0x8)  // 00 pattern (low:00, high:01)
    TESTVSHUFPS(0x9)  // 01 pattern (low:01, high:01)
    TESTVSHUFPS(0xA)  // 10 pattern (low:10, high:00)
    TESTVSHUFPS(0xB)  // 11 pattern (low:11, high:00)
    TESTVSHUFPS(0xC)  // 00 pattern (low:00, high:11)
    TESTVSHUFPS(0xD)  // 01 pattern (low:01, high:11)
    TESTVSHUFPS(0xE)  // 10 pattern (low:10, high:11)
    TESTVSHUFPS(0xF)  // 11 pattern (low:11, high:11)
}

static void test_vshufpd_256() {
    printf("=== Testing vshufpd (256-bit) ===\n");
    
    double a[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
    double b[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0};
    double dst[4] ALIGNED(32) = {0};
    
    // Test case 1: Complex shuffle pattern
    __m256d v_a = _mm256_load_pd(a);
    __m256d v_b = _mm256_load_pd(b);
    __m256d v_dst;
#define TESTVSHUFPD(imm) \
    asm volatile( \
        "vshufpd $" #imm ", %2, %1, %0"  \
        : "=x"(v_dst) \
        : "x"(v_a), "x"(v_b) \
    );   \
    _mm256_store_pd(dst, v_dst); \
    printf("Shuffle pattern: 0x%X\n", imm); \
    print_double_vec("Input A", a, 4); \
    print_double_vec("Input B", b, 4); \
    print_double_vec("Result", dst, 4); \
    print_double_vec("Expected", (double[]){ \
        (imm & 0x1) ? a[1] : a[0], \
        (imm & 0x2) ? b[1] : b[0], \
        (imm & 0x4) ? a[3] : a[2], \
        (imm & 0x8) ? b[3] : b[2] \
    }, 4); \
    printf("Test %d: %s\n", imm, memcmp(dst, (double[]){ \
        (imm & 0x1) ? a[1] : a[0], \
        (imm & 0x2) ? b[1] : b[0], \
        (imm & 0x4) ? a[3] : a[2], \
        (imm & 0x8) ? b[3] : b[2] \
    }, sizeof(double)*4) == 0 ? "PASS\n\n" : "FAIL\n\n");

    TESTVSHUFPD(0x0)  // 0000 pattern (low:00, high:00)
    TESTVSHUFPD(0x1)  // 0001 pattern (low:01, high:01)
    TESTVSHUFPD(0x2)  // 0010 pattern (low:10, high:10)
    TESTVSHUFPD(0x3)  // 0011 pattern (low:11, high:11)
    TESTVSHUFPD(0x4)  // 0100 pattern (low:00, high:10)
    TESTVSHUFPD(0x5)  // 0101 pattern (low:01, high:10)
    TESTVSHUFPD(0x6)  // 0110 pattern (low:10, high:01)
    TESTVSHUFPD(0x7)  // 0111 pattern (low:11, high:01)
    TESTVSHUFPD(0x8)  // 1000 pattern (low:00, high:01)
    TESTVSHUFPD(0x9)  // 1001 pattern (low:01, high:01)
    TESTVSHUFPD(0xA)  // 1010 pattern (low:10, high:00)
    TESTVSHUFPD(0xB)  // 1011 pattern (low:11, high:00)
    TESTVSHUFPD(0xC)  // 1100 pattern (low:00, high:11)
    TESTVSHUFPD(0xD)  // 1101 pattern (low:01, high:11)
    TESTVSHUFPD(0xE)  // 1110 pattern (low:10, high:11)
    TESTVSHUFPD(0xF)  // 1111 pattern (low:11, high:11)
    
}

int main() {
    test_vshufpd_128();
    test_vshufpd_256();
    return 0;
}
