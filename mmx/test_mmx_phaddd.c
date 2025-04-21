#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
    int32_t i32[2];
    int64_t i64;
    uint64_t u64;
} mmx_t;

void print_mmx(const char* name, mmx_t a) {
    printf("%s: [%d, %d]\n", name, a.i32[0], a.i32[1]);
}

int main() {
    printf("Testing PHADDD (Packed Horizontal Add Doubleword)\n");
    
    // Test 1: Basic addition
    mmx_t a = {.i32 = {100000, 200000}};
    mmx_t b = {.i32 = {300000, 400000}};
    mmx_t result;
    
    printf("\nTest 1: Basic horizontal addition\n");
    print_mmx("a", a);
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phaddd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [300000, 700000]\n");
    
    // Test 2: Boundary values
    mmx_t c = {.i32 = {2147483647, 2147483647}};
    mmx_t d = {.i32 = {1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("c", c);
    print_mmx("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phaddd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [2147483647+2147483647=overflow, 1+-1=0]\n");
    
    // Test 3: Memory operand
    mmx_t mem = {.i32 = {50000, -50000}};
    mmx_t reg = {.i32 = {100000, -100000}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("reg", reg);
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "phaddd %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(reg.u64), "m"(mem.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [100000+-100000=0, 50000+-50000=0]\n");
    
    asm volatile ("emms");
    return 0;
}
