#ifndef BASE_H
#define BASE_H

// 内存对齐宏
#define ALIGNED(n) __attribute__((aligned(n)))

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

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

// 清除标志寄存器（EFLAGS）的宏
#define CLEAR_FLAGS \
    asm volatile ("push $0\n\tpopfq\n\tpush $0\n\tpopfq" : : : "cc", "memory")

// Function to set EFLAGS register
static inline void set_eflags(uint64_t eflags) {
    asm volatile ("push %0\n\tpopfq" : : "r"(eflags) : "cc", "memory");
}

// Function to get EFLAGS register
static inline uint64_t get_eflags() {
    uint64_t eflags;
    asm volatile ("pushfq\n\tpop %0" : "=r"(eflags) : : "memory");
    return eflags;
}

#endif // BASE_H
