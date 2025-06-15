#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static void test_vshufps_128() {
    printf("=== Testing vshufps (128-bit) ===\n");
    
    float a[4] ALIGNED(16) = {1.0f, 2.0f, 3.0f, 4.0f};
    float b[4] ALIGNED(16) = {5.0f, 6.0f, 7.0f, 8.0f};
    float dst[4] ALIGNED(16) = {0};
    float expected[4] ALIGNED(16) = {0};
    
    // Test case 1: Basic shuffle (select a[0], a[1], b[0], b[1])
    __m128 v_a = _mm_load_ps(a);
    __m128 v_b = _mm_load_ps(b);
    __m128 v_dst;
    asm volatile(
        "vshufps $0b01000100, %2, %1, %0"  // imm8 = 0x44
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm_store_ps(dst, v_dst);
    expected[0] = a[0]; expected[1] = a[1];
    expected[2] = b[0]; expected[3] = b[1];
    print_float_vec("Result", dst, 4);
    print_float_vec("Expected", expected, 4);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(float)*4) == 0 ? "PASS" : "FAIL");

    // Test case 2: Shuffle with special values
    a[0] = NAN; a[1] = INFINITY; a[2] = -INFINITY; a[3] = 0.0f;
    b[0] = -0.0f; b[1] = 1.0f/0.0f; b[2] = -1.0f/0.0f; b[3] = 0.0f/0.0f;
    
    v_a = _mm_load_ps(a);
    v_b = _mm_load_ps(b);
    asm volatile(
        "vshufps $0b11011101, %2, %1, %0"  // imm8 = 0xDD
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm_store_ps(dst, v_dst);
    expected[0] = a[1]; expected[1] = a[3];
    expected[2] = b[1]; expected[3] = b[3];
    print_float_vec("Result", dst, 4);
    print_float_vec("Expected", expected, 4);
    printf("Test 2: %s\n", memcmp(dst, expected, sizeof(float)*4) == 0 ? "PASS" : "FAIL");
}

static void test_vshufps_256() {
    printf("=== Testing vshufps (256-bit) ===\n");
    
    float a[8] ALIGNED(32) = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float b[8] ALIGNED(32) = {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
    float dst[8] ALIGNED(32) = {0};
    float expected[8] ALIGNED(32) = {0};
    
    // Test case 1: Basic 256-bit shuffle
    __m256 v_a = _mm256_load_ps(a);
    __m256 v_b = _mm256_load_ps(b);
    __m256 v_dst;
    asm volatile(
        "vshufps $0b01000100, %2, %1, %0"  // imm8 = 0x44
        : "=x"(v_dst)
        : "x"(v_a), "x"(v_b)
    );
    _mm256_store_ps(dst, v_dst);
    // Low lane: a[0], a[1], b[0], b[1]
    // High lane: a[4], a[5], b[4], b[5]
    expected[0] = a[0]; expected[1] = a[1]; expected[2] = b[0]; expected[3] = b[1];
    expected[4] = a[4]; expected[5] = a[5]; expected[6] = b[4]; expected[7] = b[5];
    print_float_vec("Result", dst, 8);
    print_float_vec("Expected", expected, 8);
    printf("Test 1: %s\n", memcmp(dst, expected, sizeof(float)*8) == 0 ? "PASS" : "FAIL");

    // Test case 2: Boundary values and rounding modes
    // Set MXCSR to different rounding modes
    uint32_t orig_mxcsr = get_mxcsr();
    
    // Test with different rounding modes
    const uint32_t rounding_modes[] = {
        orig_mxcsr & ~0x6000,  // Round to nearest
        (orig_mxcsr & ~0x6000) | 0x2000,  // Round down
        (orig_mxcsr & ~0x6000) | 0x4000,  // Round up
        (orig_mxcsr & ~0x6000) | 0x6000   // Round toward zero
    };
    
    const char* mode_names[] = {"Nearest", "Down", "Up", "Zero"};
    
    for (int i = 0; i < 4; i++) {
        set_mxcsr(rounding_modes[i]);
        printf("\nTesting with rounding mode: %s\n", mode_names[i]);
        print_mxcsr(get_mxcsr());
        
        // Reinitialize arrays
        for (int j = 0; j < 8; j++) {
            a[j] = (j + 1) * 0.1f;
            b[j] = (j + 1) * 0.3f;
        }
        
        v_a = _mm256_load_ps(a);
        v_b = _mm256_load_ps(b);
        
        asm volatile(
            "vshufps $0b10101010, %2, %1, %0"  // imm8 = 0xAA
            : "=x"(v_dst)
            : "x"(v_a), "x"(v_b)
        );
        _mm256_store_ps(dst, v_dst);
        
        // Expected values:
        // Low lane: a[2], a[2], b[2], b[2]
        // High lane: a[6], a[6], b[6], b[6]
        expected[0] = a[2]; expected[1] = a[2];
        expected[2] = b[2]; expected[3] = b[2];
        expected[4] = a[6]; expected[5] = a[6];
        expected[6] = b[6]; expected[7] = b[6];
        
        print_float_vec("Result", dst, 8);
        print_float_vec("Expected", expected, 8);
        
        // Compare with tolerance
        int pass = 1;
        for (int j = 0; j < 8; j++) {
            if (!float_equal(dst[j], expected[j], 0.0001f)) {
                pass = 0;
                break;
            }
        }
        printf("Test 2.%d: %s\n", i+1, pass ? "PASS" : "FAIL");
    }
    
    // Restore original MXCSR
    set_mxcsr(orig_mxcsr);
}

int main() {
    test_vshufps_128();
    test_vshufps_256();
    return 0;
}
