#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
    int8_t i8[8];
    int16_t i16[4];
    int32_t i32[2];
    int64_t i64;
    uint64_t u64;
} mmx_t;

void print_mmx(const char* name, mmx_t a) {
    printf("%s: [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           name, a.i8[0], a.i8[1], a.i8[2], a.i8[3],
           a.i8[4], a.i8[5], a.i8[6], a.i8[7]);
}

int main() {
    printf("Testing PABSB (Packed Absolute Value of Bytes)\n");
    
    // Test 1: Basic values
    mmx_t a = {.i8 = {10, -20, 30, -40, 0, -128, 127, -1}};
    mmx_t result;
    
    printf("\nTest 1: Basic values\n");
    print_mmx("a", a);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsb %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [10, 20, 30, 40, 0, -128, 127, 1]\n");
    
    // Test 2: Memory operand
    mmx_t mem = {.i8 = {-50, 50, -100, 100, -127, 127, -0, 0}};
    
    printf("\nTest 2: Memory operand\n");
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsb %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(mem.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [50, 50, 100, 100, 127, 127, 0, 0]\n");
    
    asm volatile ("emms");
    return 0;
}
