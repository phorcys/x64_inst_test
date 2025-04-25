#ifndef X87_H
#define X87_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>

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
#define POS_DENORM   3.6451995318824746e-4951L  // Smallest positive denormal
#define NEG_DENORM   -3.6451995318824746e-4951L // Smallest negative denormal
#define PI           3.1415926535897932385L
#define E            2.7182818284590452354L

#endif // X87_H
