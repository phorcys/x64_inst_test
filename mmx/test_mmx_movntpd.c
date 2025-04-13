#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <emmintrin.h>
#include <math.h>

void test_movntpd() {
    printf("Testing MOVNTPD (Ex, Gx) SSE instruction\n\n");
    
    // Align memory to 16-byte boundary
    double dest[2] __attribute__((aligned(16))) = {0};
    double src[2] __attribute__((aligned(16))) = {1.23456789, -9.87654321};
    
    // Test 1: Basic MOVNTPD operation
    printf("Test 1: Basic MOVNTPD operation\n");
    printf("Source: [%f, %f]\n", src[0], src[1]);
    
    __m128d v = _mm_load_pd(src);
    _mm_stream_pd(dest, v);
    
    printf("Dest:   [%f, %f]\n", dest[0], dest[1]);
    printf("Test %s\n\n", 
           (dest[0] == src[0] && dest[1] == src[1]) ? "PASSED" : "FAILED");
    
    // Test 2: Special floating-point values
    double special_values[][2] = {
        {0.0, -0.0},
        {__builtin_inf(), -__builtin_inf()},
        {__builtin_nan(""), -__builtin_nan("")},
        {1.0e-300, -1.0e300}
    };
    
    for (size_t i = 0; i < sizeof(special_values)/sizeof(special_values[0]); i++) {
        memcpy(src, special_values[i], sizeof(src));
        memset(dest, 0, sizeof(dest));
        
        printf("Test 2.%zu: MOVNTPD with special values [%f, %f]\n", 
               i+1, src[0], src[1]);
        
        v = _mm_load_pd(src);
        _mm_stream_pd(dest, v);
        
        printf("Dest:   [%f, %f]\n", dest[0], dest[1]);
        
        int passed = 1;
        for (int j = 0; j < 2; j++) {
            if (isnan(src[j])) {
                if (!isnan(dest[j])) passed = 0;
            } else if (dest[j] != src[j]) {
                passed = 0;
            }
        }
        printf("Test %s\n\n", passed ? "PASSED" : "FAILED");
    }
    
    // Test 3: Verify non-temporal behavior with large buffer
    {
        const int SIZE = 1024;
        double* big_buf = (double*)aligned_alloc(16, SIZE * sizeof(double));
        double test_val = 3.141592653589793;
        
        // Fill buffer with test value using MOVNTPD
        __m128d val = _mm_set1_pd(test_val);
        for (int i = 0; i < SIZE; i += 2) {
            _mm_stream_pd(big_buf + i, val);
        }
        
        printf("Test 3: Verify MOVNTPD with large buffer\n");
        
        // Verify all values were written correctly
        int passed = 1;
        for (int i = 0; i < SIZE; i++) {
            if (big_buf[i] != test_val) {
                passed = 0;
                break;
            }
        }
        
        printf("Verified %d elements, all %s\n", 
               SIZE, passed ? "correct" : "incorrect");
        printf("Test %s\n\n", passed ? "PASSED" : "FAILED");
        
        free(big_buf);
    }
}

int main() {
    test_movntpd();
    return 0;
}
