#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
    int16_t i16[4];
    int32_t i32[2];
    int64_t i64;
    uint64_t u64;
} mmx_t;

void print_mmx(const char* name, mmx_t a) {
    printf("%s: [%d, %d, %d, %d]\n", 
           name, a.i16[0], a.i16[1], a.i16[2], a.i16[3]);
}

int main() {
    printf("Testing PABSW (Packed Absolute Value of Words)\n");
    
    // Test 1: Basic values
    mmx_t a = {.i16 = {1000, -2000, 3000, -4000}};
    mmx_t result;
    
    printf("\nTest 1: Basic values\n");
    print_mmx("a", a);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsw %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [1000, 2000, 3000, 4000]\n");
    
    // Test 2: Boundary values
    mmx_t b = {.i16 = {32767, -32768, 0, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsw %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(b.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [32767, -32768, 0, 1] (Note: -32768 remains unchanged per Intel spec)\n");
    
    // Test 3: Memory operand
    mmx_t mem = {.i16 = {-5000, 5000, -32767, 32767}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsw %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(mem.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [5000, 5000, 32767, 32767]\n");
    
    asm volatile ("emms");
    return 0;
}
