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
    printf("Testing PSIGND (Packed SIGN Doubleword)\n");
    
    // Test 1: Basic sign operations
    mmx_t a = {.i32 = {100000, -200000}};
    mmx_t b = {.i32 = {1, -1}};
    mmx_t result;
    
    printf("\nTest 1: Basic sign operations\n");
    print_mmx("a", a);
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [100000, 200000]\n");
    
    // Test 2: Boundary values
    mmx_t c = {.i32 = {2147483647, -2147483648}};
    mmx_t d = {.i32 = {1, -1}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("c", c);
    print_mmx("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [2147483647, -2147483648]\n"); // -2147483648 remains due to overflow
    
    // Test 3: Memory operand
    mmx_t mem = {.i32 = {50000, -50000}};
    mmx_t reg = {.i32 = {1, -1}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("reg", reg);
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignd %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(mem.u64), "m"(reg.u64)
        : "mm0", "mm1"
    );
    
    print_mmx("result", result);
    printf("Expected: [50000, 50000]\n");
    
    asm volatile ("emms");
    return 0;
}
