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
    printf("Testing PHSUBSW (Packed Horizontal Subtract Word with Saturation)\n");
    
    // Test 1: Basic subtraction with saturation
    mmx_t a = {.i16 = {30000, -30000, 20000, -20000}};
    mmx_t b = {.i16 = {10000, -10000, 15000, -15000}};
    mmx_t result;
    
    printf("\nTest 1: Basic subtraction with saturation\n");
    print_mmx_words("a", a);
    print_mmx_words("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phsubsw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [32767, 32767, 20000, 30000]\n"); // (30000-(-30000)) saturated to 32767, (20000-(-15000))=35000 saturated to 32767
    
    // Test 2: Boundary values
    mmx_t c = {.i16 = {32767, -32768, 32767, -32768}};
    mmx_t d = {.i16 = {1, -1, 1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx_words("c", c);
    print_mmx_words("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phsubsw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [32767, 32767, 2, 2]\n"); // Actual hardware behavior with saturation
    
    // Test 3: Memory operand
    mmx_t mem = {.i16 = {1000, 2000, 3000, 4000}};
    mmx_t reg = {.i16 = {500, 1500, 2500, 3500}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx_words("reg", reg);
    print_mmx_words("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "phsubsw %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(reg.u64), "m"(mem.u64)
    );
    
    print_mmx_words("result", result);
    printf("Expected: [-1000, -1000, -1000, -1000]\n");
    
    asm volatile ("emms");
    return 0;
}
