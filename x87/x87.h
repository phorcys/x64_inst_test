#ifndef X87_H
#define X87_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>


// x87控制字定义
#define X87_CW_IM        0x0001  // 无效操作异常
#define X87_CW_DM        0x0002  // 非规格化异常  
#define X87_CW_ZM        0x0004  // 除零异常
#define X87_CW_OM        0x0008  // 溢出异常
#define X87_CW_UM        0x0010  // 下溢异常
#define X87_CW_PM        0x0020  // 精度异常
#define X87_CW_PC_MASK   0x0300  // 精度控制掩码
#define X87_CW_PC_SINGLE 0x0000  // 单精度(24位)
#define X87_CW_PC_DOUBLE 0x0200  // 双精度(53位)
#define X87_CW_PC_EXTEND 0x0300  // 扩展精度(64位)
#define X87_CW_RC_MASK   0x0C00  // 舍入控制掩码
#define X87_CW_RC_NEAR   0x0000  // 舍入到最近
#define X87_CW_RC_DOWN   0x0400  // 向下舍入
#define X87_CW_RC_UP     0x0800  // 向上舍入
#define X87_CW_RC_ZERO   0x0C00  // 向零舍入

// x87状态字定义
#define X87_SW_IE        0x0001  // 无效操作异常
#define X87_SW_DE        0x0002  // 非规格化异常
#define X87_SW_ZE        0x0004  // 除零异常
#define X87_SW_OE        0x0008  // 溢出异常
#define X87_SW_UE        0x0010  // 下溢异常
#define X87_SW_PE        0x0020  // 精度异常
#define X87_SW_SF        0x0040  // 堆栈错误
#define X87_SW_ES        0x0080  // 错误状态
#define X87_SW_C0        0x0100  // 条件码0
#define X87_SW_C1        0x0200  // 条件码1
#define X87_SW_C2        0x0400  // 条件码2
#define X87_SW_C3        0x4000  // 条件码3
#define X87_SW_TOP_MASK  0x3800  // TOP位掩码
#define X87_SW_TOP_SHIFT 11      // TOP位移量


// Print 80-bit float value
static inline void print_float80(long double val) {
    printf("%.20Lf (0x%04x%016llx)\n", val,
           ((unsigned short *)&val)[4], *(unsigned long long *)&val);
}

// Get x87 Control Word
static inline uint16_t get_x87_cw() {
    uint16_t cw;
    asm volatile ("fnstcw %0" : "=m" (cw));
    return cw;
}

// Get x87 Status Word
static inline uint16_t get_x87_sw() {
    uint16_t sw;
    asm volatile ("fnstsw %0" : "=m" (sw));
    return sw;
}

// Get x87 Tag Word
static inline uint16_t get_x87_tw() {
    uint16_t tw;
    asm volatile ("fnstsw %0" : "=m" (tw));
    return tw;
}

// Initialize x87 environment
static inline void init_x87_env() {
    // Set default control word (mask all exceptions, double precision, round to nearest)
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR;
    asm volatile ("fldcw %0" : : "m" (cw));
    
    // Clear x87 stack
    asm volatile ("fninit");
}

// Print x87 status
static inline void print_x87_status() {
    uint16_t cw = get_x87_cw();
    uint16_t sw = get_x87_sw();
    uint16_t tw = get_x87_tw();
    
    printf("Control Word: 0x%04x\n", cw);
    printf("Status Word:  0x%04x\n", sw);
    printf("Tag Word:     0x%04x\n", tw);
    
    // Print individual status flags
    printf("Top: %d\n", (sw >> 11) & 0x7);
    printf("C0: %d, C1: %d, C2: %d, C3: %d\n",
           (sw >> 8) & 1, (sw >> 9) & 1, 
           (sw >> 10) & 1, (sw >> 14) & 1);
    
    // Print exception flags
    printf("Exceptions: ");
    if (sw & 0x01) printf("IE ");
    if (sw & 0x02) printf("DE ");
    if (sw & 0x04) printf("ZE ");
    if (sw & 0x08) printf("OE ");
    if (sw & 0x10) printf("UE ");
    if (sw & 0x20) printf("PE ");
    if (sw & 0x40) printf("SF ");
    if (sw & 0x80) printf("ES ");
    printf("\n");
}

// Common test values
#define POS_ZERO     0.0L
#define NEG_ZERO     -0.0L
#define POS_ONE      1.0L
#define NEG_ONE      -1.0L
#define POS_INF      INFINITY
#define NEG_INF      -INFINITY
#define POS_NAN      NAN
#define NEG_NAN      -NAN
#define POS_DENORM   (3.6451995318824746e-4951L)  // Smallest positive denormal
#define NEG_DENORM   (-3.6451995318824746e-4951L) // Smallest negative denormal
#define PI           3.1415926535897932385L
#define E            2.7182818284590452354L

// EFLAGS标志位定义
#define X86_EFLAGS_CF   0x00000001  // 进位标志
#define X86_EFLAGS_PF   0x00000004  // 奇偶标志  
#define X86_EFLAGS_ZF   0x00000040  // 零标志
#define X86_EFLAGS_NONE 0x00000000  // 无标志

#endif // X87_H
