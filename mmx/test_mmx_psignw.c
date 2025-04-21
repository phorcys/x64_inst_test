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
    printf("Testing PSIGNW (Packed SIGN Word)\n");
    
    // Test 1: Basic sign operations
    mmx_t a = {.i16 = {1000, -2000, 3000, -4000}};
    mmx_t b = {.i16 = {1, -1, 0, 1}};
    mmx_t result;
    
    printf("\nTest 1: Basic sign operations\n");
    print_mmx("a", a);
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [1000, 2000, 0, -4000]\n");
    
    // Test 2: Boundary values
    mmx_t c = {.i16 = {32767, -32768, 0, 32767}};
    mmx_t d = {.i16 = {1, -1, 1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("c", c);
    print_mmx("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [32767, -32768, 0, -32767]\n"); // -32768 remains -32768 due to overflow
    
    // Test 3: Memory operand
    mmx_t mem = {.i16 = {500, -500, 1000, -1000}};
    mmx_t reg = {.i16 = {1, -1, 0, 1}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("reg", reg);
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(mem.u64), "m"(reg.u64)
        : "mm0", "mm1"
    );
    
    print_mmx("result", result);
    printf("Expected: [500, 500, 0, -1000]\n");
    
    asm volatile ("emms");
    return 0;
}
