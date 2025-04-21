#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
    int32_t i32[2];
    int64_t i64;
    uint64_t u64;
} mmx_t;

void print_mmx_dwords(const char* name, mmx_t a) {
    printf("%s: [%d, %d]\n", name, a.i32[0], a.i32[1]);
}

int main() {
    printf("Testing PHSUBD (Packed Horizontal Subtract Dword)\n");
    
    // Test 1: Basic subtraction
    mmx_t a = {.i32 = {100000, -100000}};
    mmx_t b = {.i32 = {200000, -200000}};
    mmx_t result;
    
    printf("\nTest 1: Basic subtraction\n");
    print_mmx_dwords("a", a);
    print_mmx_dwords("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phsubd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx_dwords("result", result);
    printf("Expected: [200000, 400000]\n"); // (100000 - (-100000)) = 200000, (200000 - (-200000)) = 400000
    
    // Test 2: Boundary values
    mmx_t c = {.i32 = {2147483647, -2147483648}};
    mmx_t d = {.i32 = {1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx_dwords("c", c);
    print_mmx_dwords("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "phsubd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx_dwords("result", result);
    printf("Expected: [-1, 2]\n"); // Actual hardware behavior
    
    // Test 3: Memory operand
    mmx_t mem = {.i32 = {10000, 20000}};
    mmx_t reg = {.i32 = {5000, 15000}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx_dwords("reg", reg);
    print_mmx_dwords("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "phsubd %2, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(reg.u64), "m"(mem.u64)
    );
    
    print_mmx_dwords("result", result);
    printf("Expected: [-10000, -10000]\n");
    
    asm volatile ("emms");
    return 0;
}
