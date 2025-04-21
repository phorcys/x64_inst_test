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
void print_mmxu(const char* name, mmx_t a) {
    printf("%s: [%u, %u, %u, %u, %u, %u, %u, %u]\n", 
           name, a.i8[0], a.i8[1], a.i8[2], a.i8[3],
           a.i8[4], a.i8[5], a.i8[6], a.i8[7]);
}
void print_mmx_words(const char* name, mmx_t a) {
    printf("%s: [%d, %d, %d, %d]\n", 
           name, a.i16[0], a.i16[1], a.i16[2], a.i16[3]);
}

int main() {
    printf("Testing PMADDUBSW (Packed Multiply and Add Unsigned Byte to Signed Word with Saturation)\n");
    
    // Test 1: Basic multiplication and addition
    mmx_t a = {.i8 = {100, 100, 50, 50, 127, 127, 64, 64}};
    mmx_t b = {.i8 = {1, -1, 2, -2, 1, -1, 1, -1}};
    mmx_t result;
    
    printf("\nTest 1: Basic multiplication and addition\n");
    print_mmxu("a", a);
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "pmaddubsw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [0, 0, 0, 0]\n"); // (100*1 + 100*-1) = 0, (50*2 + 50*-2) = 0, etc.
    
    // Test 2: Boundary values with saturation
    mmx_t c = {.u64 = 0xFFFFFFFFFFFFFFFF}; // All bytes = 255 (unsigned)
    mmx_t d = {.i8 = {127, -128, 127, -128, 127, -128, 127, -128}};
    
    printf("\nTest 2: Boundary values with saturation\n");
    print_mmxu("c", c);
    print_mmx("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "pmaddubsw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [-255, -255, -255, -255]\n"); // (255*127 + 255*-128) = -255
    
    // Test 3: Memory operand
    mmx_t mem = {.i8 = {10, 20, 30, 40, 50, 60, 70, 80}}; // Unsigned
    mmx_t reg = {.i8 = {1, -1, 2, -2, 3, -3, 4, -4}}; // Signed
    
    printf("\nTest 3: Memory operand\n");
    print_mmxu("reg", reg);
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pmaddubsw %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(reg.u64), "m"(mem.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [5110, 10220, 15330, 20440]\n"); // (1*10 + 255*20) = 5110, etc.
    
    asm volatile ("emms");
    return 0;
}
