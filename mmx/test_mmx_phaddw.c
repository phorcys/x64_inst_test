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
    printf("%s: [%d, %d, %d, %d]\n", name, a.i16[0], a.i16[1], a.i16[2], a.i16[3]);
}

int main() {
    printf("Testing PHADDW (Packed Horizontal Add Word)\n");
    
    // Test 1: Basic addition
    mmx_t a = {.i16 = {1000, 2000, -1000, -2000}};
    mmx_t b = {.i16 = {3000, 4000, -3000, -4000}};
    mmx_t result;
    
    printf("\nTest 1: Basic horizontal addition\n");
    print_mmx("a", a);
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phaddw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [3000, -3000, 7000, -7000]\n");
    
    // Test 2: Boundary values
    mmx_t c = {.i16 = {32767, 32767, -32768, -32768}};
    mmx_t d = {.i16 = {1, -1, 1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("c", c);
    print_mmx("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phaddw %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [32767+32767=overflow, -32768+-32768=overflow, 1+-1=0, 1+-1=0]\n");
    
    // Test 3: Memory operand
    mmx_t mem = {.i16 = {100, -100, 200, -200}};
    mmx_t reg = {.i16 = {50, -50, 150, -150}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("reg", reg);
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "phaddw %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(reg.u64), "m"(mem.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [50+-50=0, 150+-150=0, 100+-100=0, 200+-200=0]\n");
    
    asm volatile ("emms");
    return 0;
}
