#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
    int16_t i16[4];
    int32_t i32[2];
    int64_t i64;
    uint64_t u64;
} mmx_t;

void print_mmx_words(const char* name, mmx_t a) {
    printf("%s: [%d, %d, %d, %d]\n", 
           name, a.i16[0], a.i16[1], a.i16[2], a.i16[3]);
}

int main() {
    printf("Testing PHSUBW (Packed Horizontal Subtract Word)\n");
    
    // Test 1: Basic subtraction
    mmx_t a = {.i16 = {1000, -1000, 500, -500}};
    mmx_t b = {.i16 = {2000, -2000, 1000, -1000}};
    mmx_t result;
    
    printf("\nTest 1: Basic subtraction\n");
    print_mmx_words("a", a);
    print_mmx_words("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phsubw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [2000, 1000, 4000, 2000]\n");
    
    // Test 2: Boundary values
    mmx_t c = {.i16 = {32767, -32768, 32767, -32768}};
    mmx_t d = {.i16 = {1, -1, 1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx_words("c", c);
    print_mmx_words("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phsubw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [-1, -1, 2, 2]\n"); // Actual hardware behavior
    
    // Test 3: Memory operand
    mmx_t mem = {.i16 = {100, 200, 300, 400}};
    mmx_t reg = {.i16 = {50, 150, 250, 350}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx_words("reg", reg);
    print_mmx_words("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "phsubw %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(reg.u64), "m"(mem.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [-100, -100, -100, -100]\n");
    
    asm volatile ("emms");
    return 0;
}
