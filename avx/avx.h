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

static inline void print_uint16_vec(const char* name, uint16_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%u ", vec[i]);
    }
    printf("\n");
}
static inline void print_int16_vec(const char* name, int16_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("%d ", vec[i]);
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

// 打印32位整数向量
static inline void print_int32_vec_hex(const char* name, int32_t* vec, int count) {
    printf("%s: ", name);
    for(int i=0; i<count; i++) {
        printf("0x%x ", vec[i]);
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
    (void)mxcsr;  // 明确标记参数为未使用
    // 输出MXCSR状态, 注释掉因为box64 暂时不实现mxcsr各位域正确。
    // printf("MXCSR: 0x%08X\n", mxcsr);
    // printf("  [ ] DAZ - Denormals Are Zero: %d\n", (mxcsr >> 6) & 1);
    // printf("  [ ] FTZ - Flush To Zero: %d\n", (mxcsr >> 15) & 1);
    // printf("Flags: I:%d D:%d Z:%d O:%d U:%d P:%d\n",
    //        (mxcsr >> 0) & 1,  // Invalid
    //        (mxcsr >> 1) & 1,  // Denormal
    //        (mxcsr >> 2) & 1,  // Divide-by-zero
    //        (mxcsr >> 3) & 1,  // Overflow
    //        (mxcsr >> 4) & 1,  // Underflow
    //        (mxcsr >> 5) & 1); // Precision
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
#define X_CF (1<<0)
#define X_PF (1<<2)
#define X_AF (1<<4)
#define X_ZF (1<<6)
#define X_SF (1<<7)
#define X_TF (1<<8)
#define X_IF (1<<9)
#define X_DF (1<<10)
#define X_OF (1<<11)
#define EFLAGS_MASK (0xFFFFFFFFFFFCFAFF) // 保留所有定义标志位，清除未定义位
// Print EFLAGS register
static inline void print_eflags(uint64_t eflags) {
    printf("EFLAGS: 0x%08X\n", (uint32_t)eflags);
    printf("  [%c] CF - Carry Flag\n", (eflags & X_CF) ? 'X' : ' ');
    printf("  [%c] PF - Parity Flag\n", (eflags & X_PF) ? 'X' : ' ');
    printf("  [%c] AF - Auxiliary Flag\n", (eflags & X_AF) ? 'X' : ' ');
    printf("  [%c] ZF - Zero Flag\n", (eflags & X_ZF) ? 'X' : ' ');
    printf("  [%c] SF - Sign Flag\n", (eflags & X_SF) ? 'X' : ' ');
    printf("  [%c] TF - Trap Flag\n", (eflags & X_TF) ? 'X' : ' ');
    printf("  [%c] IF - Interrupt Flag\n", (eflags & X_IF) ? 'X' : ' ');
    printf("  [%c] DF - Direction Flag\n", (eflags & X_DF) ? 'X' : ' ');
    printf("  [%c] OF - Overflow Flag\n", (eflags & X_OF) ? 'X' : ' ');
}

// Print EFLAGS register with condition mask
static inline void print_eflags_cond(uint32_t eflags, uint32_t cond) {
    if(cond & (1<<0)) printf("  [%c] CF - Carry Flag\n", (eflags & (1 << 0)) ? 'X' : ' ');
    if(cond & (1<<2)) printf("  [%c] PF - Parity Flag\n", (eflags & (1 << 2)) ? 'X' : ' ');
    if(cond & (1<<4)) printf("  [%c] AF - Auxiliary Flag\n", (eflags & (1 << 4)) ? 'X' : ' ');
    if(cond & (1<<6)) printf("  [%c] ZF - Zero Flag\n", (eflags & (1 << 6)) ? 'X' : ' ');
    if(cond & (1<<7)) printf("  [%c] SF - Sign Flag\n", (eflags & (1 << 7)) ? 'X' : ' ');
    if(cond & (1<<8)) printf("  [%c] TF - Trap Flag\n", (eflags & (1 << 8)) ? 'X' : ' ');
    if(cond & (1<<9)) printf("  [%c] IF - Interrupt Flag\n", (eflags & (1 << 9)) ? 'X' : ' ');
    if(cond & (1<<10)) printf("  [%c] DF - Direction Flag\n", (eflags & (1 << 10)) ? 'X' : ' ');
    if(cond & (1<<11)) printf("  [%c] OF - Overflow Flag\n", (eflags & (1 << 11)) ? 'X' : ' ');
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
// Helper to print 128-bit vector (double precision)
static void __attribute__((unused)) print_vector128d(const char* name, __m128d vec) {
    double d[2];
    _mm_storeu_pd(d, vec);
    printf("%s: [%.6f, %.6f]\n", 
           name, d[0], d[1]);
}
// Helper to print 256-bit vector (single precision)
static void __attribute__((unused)) print_vector256(const char* name, __m256 vec) {
    float f[8];
    _mm256_storeu_ps(f, vec);
    printf("%s: [%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f]\n", 
           name, f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
}

// Helper to print 256-bit vector (double precision)
static void __attribute__((unused)) print_vector256d(const char* name, __m256d vec) {
    double d[4];
    _mm256_storeu_pd(d, vec);
    printf("%s: [%.6f, %.6f, %.6f, %.6f]\n", 
           name, d[0], d[1], d[2], d[3]);
}

// 清除标志寄存器（EFLAGS）的宏
#define CLEAR_FLAGS \
    asm volatile ("push $0\n\tpopfq\n\tpush $0\n\tpopfq" : : : "cc", "memory")

// 打印128位整型向量（十六进制）
static inline void print_m128i_hex(__m128i xmm, const char* name) {
    uint8_t buf[16];
    memcpy(buf, &xmm, 16);
    printf("%s: ", name);
    for(int i=0; i<16; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 打印256位整型向量（十六进制）
static inline void print_m256i_hex(__m256i ymm, const char* name) {
    uint8_t buf[32];
    memcpy(buf, &ymm, 32);
    printf("%s: ", name);
    for(int i=0; i<32; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

// 兼容性宏定义
#ifndef _mm_setr_epi64x
#define _mm_setr_epi64x(e1, e0) \
    _mm_set_epi64x((long long)(e1), (long long)(e0))
#endif

// 打印128位整型向量的字节内容
static inline void print_m128i_bytes(__m128i xmm) {
    uint8_t buf[16];
    memcpy(buf, &xmm, 16);
    for(int i=0; i<16; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

// 打印128位双精度向量
static inline void print_xmmd(const char* name, __m128d xmm) {
    double d[2];
    _mm_storeu_pd(d, xmm);
    printf("%s: [%.6f, %.6f]\n", name, d[0], d[1]);
}

// 打印双精度向量控制字
static inline void print_control_bits(const char* name, __m128d ctrl) {
    uint64_t *vals = (uint64_t*)&ctrl;
    printf("%s control bits: [%lu, %lu] (hex: 0x%lx, 0x%lx)\n", 
           name, vals[0] & 1, vals[1] & 1, vals[0], vals[1]);
}

static inline void print_control_bits_ymm(const char* name, __m256d ctrl) {
    uint64_t *vals = (uint64_t*)&ctrl;
    printf("%s control bits: [%lu, %lu, %lu, %lu] (hex: 0x%lx, 0x%lx, 0x%lx, 0x%lx)\n", 
           name, vals[0] & 1, vals[1] & 1, vals[2] & 1, vals[3] & 1,
           vals[0], vals[1], vals[2], vals[3]);
}

// 打印256位双精度向量
static inline void print_ymmd(const char* name, __m256d ymm) {
    double d[4];
    _mm256_storeu_pd(d, ymm);
    printf("%s: [%.6f, %.6f, %.6f, %.6f]\n", name, d[0], d[1], d[2], d[3]);
}

// 比较两个128位双精度向量是否相等
static inline int cmp_xmmd(__m128d a, __m128d b) {
    double da[2], db[2];
    _mm_storeu_pd(da, a);
    _mm_storeu_pd(db, b);
    return (fabs(da[0] - db[0]) < 1e-9) && (fabs(da[1] - db[1]) < 1e-9);
}

// 比较两个256位双精度向量是否相等
static inline int cmp_ymmd(__m256d a, __m256d b) {
    double da[4], db[4];
    _mm256_storeu_pd(da, a);
    _mm256_storeu_pd(db, b);
    return (fabs(da[0] - db[0]) < 1e-9) && 
           (fabs(da[1] - db[1]) < 1e-9) &&
           (fabs(da[2] - db[2]) < 1e-9) &&
           (fabs(da[3] - db[3]) < 1e-9);
}

#endif // AVX_H
