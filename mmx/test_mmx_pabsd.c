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
    printf("Testing PABSD (Packed Absolute Value of Doublewords)\n");
    
    // Test 1: Basic values
    mmx_t a = {.i32 = {100000, -200000}};
    mmx_t result;
    
    printf("\nTest 1: Basic values\n");
    print_mmx("a", a);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsd %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [100000, 200000]\n");
    
    // Test 2: Boundary values
    mmx_t b = {.i32 = {2147483647, -2147483648}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsd %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(b.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [2147483647, -2147483648] (Note: -2147483648 remains unchanged per Intel spec)\n");
    
    // Test 3: Memory operand
    mmx_t mem = {.i32 = {-500000, 500000}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("mem", mem);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "pabsd %%mm0, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(mem.u64)
        : "mm0"
    );
    
    print_mmx("result", result);
    printf("Expected: [500000, 500000]\n");
    
    asm volatile ("emms");
    return 0;
}
