#ifndef AVX_H
#define AVX_H

// 内存对齐宏
#define ALIGNED(n) __attribute__((aligned(n)))

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 打印256位向量
static inline void print_ymm(const char* name, __m256i ymm) {
    uint8_t buf[32];
    memcpy(buf, &ymm, 32);
    printf("%s: ", name);
    for(int i=0; i<32; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印128位向量
static inline void print_xmm(const char* name, __m128i xmm) {
    uint8_t buf[16];
    memcpy(buf, &xmm, 16);
    printf("%s: ", name);
    for(int i=0; i<16; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印浮点向量
static inline void print_float_vec(const char* name, float* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%f ", vec[i]);
    }
    printf("\n");
}

// 打印双精度浮点向量
static inline void print_double_vec(const char* name, double* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%f ", vec[i]);
    }
    printf("\n");
}

// 以十六进制形式打印双精度浮点向量
static inline void print_double_vec_hex(const char* name, double* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        uint64_t *val = (uint64_t*)&vec[i];
        printf("0x%016lx ", *val);
    }
    printf("\n");
}

// 以十六进制形式打印单精度浮点向量
static inline void print_hex_float_vec(const char* name, float* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        uint32_t *val = (uint32_t*)&vec[i];
        printf("0x%08X ", *val);
    }
    printf("\n");
}

// 打印32位整数向量
static inline void print_int32_vec(const char* name, int32_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}

// 打印64位整数向量
static inline void print_int64_vec(const char* name, int64_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%ld ", vec[i]);
    }
    printf("\n");
}

// 比较两个256位向量是否相等
static inline int cmp_ymm(__m256i a, __m256i b) {
    uint8_t buf1[32], buf2[32];
    memcpy(buf1, &a, 32);
    memcpy(buf2, &b, 32);
    return memcmp(buf1, buf2, 32) == 0;
}

// 比较两个128位向量是否相等
static inline int cmp_xmm(__m128i a, __m128i b) {
    uint8_t buf1[16], buf2[16];
    memcpy(buf1, &a, 16);
    memcpy(buf2, &b, 16);
    return memcmp(buf1, buf2, 16) == 0;
}

// Function to print MXCSR register with flag details
static void __attribute__((unused)) print_mxcsr(uint32_t mxcsr) {
    // 输出MXCSR状态
    printf("MXCSR: 0x%08X\n", mxcsr);
    printf("  [ ] DAZ - Denormals Are Zero: %d\n", (mxcsr >> 6) & 1);
    printf("  [ ] FTZ - Flush To Zero: %d\n", (mxcsr >> 15) & 1);
    printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
           (mxcsr >> 0) & 1,  // Invalid
           (mxcsr >> 1) & 1,  // Denormal
           (mxcsr >> 2) & 1,  // Divide-by-zero
           (mxcsr >> 3) & 1,  // Overflow
           (mxcsr >> 4) & 1,  // Underflow
           (mxcsr >> 5) & 1); // Precision
}

// Set MXCSR register value
static void __attribute__((unused)) set_mxcsr(uint32_t mxcsr) {
    __asm__ __volatile__("ldmxcsr %0" : : "m"(mxcsr));
}

// Get MXCSR register value
static uint32_t __attribute__((unused)) get_mxcsr() {
    uint32_t mxcsr;
    __asm__ __volatile__("stmxcsr %0" : "=m"(mxcsr));
    return mxcsr;
}

// Floating point comparison with tolerance and ULPs
static int __attribute__((unused)) float_equal_ulp(float a, float b, float tolerance, int max_ulps) {
    if (isnan(a) && isnan(b)) return 1;
    if (fabsf(a - b) <= tolerance) return 1;
    
    int32_t ai = *(int32_t*)&a;
    int32_t bi = *(int32_t*)&b;
    if ((ai < 0) != (bi < 0)) return 0; // Different signs
    return abs(ai - bi) <= max_ulps;
}

// Floating point comparison with tolerance (default 4 ULPs)
static int __attribute__((unused)) float_equal(float a, float b, float tolerance) {
    return float_equal_ulp(a, b, tolerance, 4);
}

// Double precision comparison with tolerance
static int __attribute__((unused)) double_equal(double a, double b, double tolerance) {
    return (fabs(a - b) <= tolerance);
}

// Helper to print 128-bit vector
static void __attribute__((unused)) print_vector128(const char* name, __m128 vec) {
    float f[4];
    _mm_storeu_ps(f, vec);
    printf("%s: [%.6f, %.6f, %.6f, %.6f]\n", name, f[0], f[1], f[2], f[3]);
}

// Helper to print 256-bit vector
static void __attribute__((unused)) print_vector256(const char* name, __m256 vec) {
    float f[8];
    _mm256_storeu_ps(f, vec);
    printf("%s: [%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f]\n", 
           name, f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
}

#endif // AVX_H
