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

int main() {
    printf("Testing PSIGNB (Packed SIGN Byte)\n");
    
    // Test 1: Basic sign operations
    mmx_t a = {.i8 = {10, -20, 30, -40, 50, -60, 70, -80}};
    mmx_t b = {.i8 = {1, -1, 0, 1, -1, 0, 1, -1}};
    mmx_t result;
    
    printf("\nTest 1: Basic sign operations\n");
    print_mmx("a", a);
    print_mmx("b", b);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignb %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(a.u64), "m"(b.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [10, 20, 0, -40, -50, 0, 70, 80]\n");
    
    // Test 2: Boundary values
    mmx_t c = {.i8 = {127, -128, 0, 127, -128, 0, 127, -128}};
    mmx_t d = {.i8 = {1, -1, 1, -1, 1, -1, 0, 0}};
    
    printf("\nTest 2: Boundary values\n");
    print_mmx("c", c);
    print_mmx("d", d);
    
    asm volatile (
        "movq %1, %%mm0\n\t"
        "movq %2, %%mm1\n\t"
        "psignb %%mm1, %%mm0\n\t"
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(c.u64), "m"(d.u64)
    );
    
    print_mmx("result", result);
    printf("Expected: [127, -128, 0, -127, -128, 0, 0, 0]\n"); // -128 remains -128 after sign change
    
    // Test 3: Memory operand
    mmx_t mem = {.i8 = {5, -5, 10, -10, 15, -15, 20, -20}};
    mmx_t reg = {.i8 = {1, -1, 0, 1, -1, 0, 1, -1}};
    
    printf("\nTest 3: Memory operand\n");
    print_mmx("reg", reg);
    print_mmx("mem", mem);
    
    // Test memory operand by loading mem to mm0 and applying sign from reg
    asm volatile (
        "movq %1, %%mm0\n\t"  // Load mem to mm0
        "movq %2, %%mm1\n\t"  // Load reg to mm1
        "psignb %%mm1, %%mm0\n\t"  // Apply sign from reg to mem
        "movq %%mm0, %0\n\t"
        : "=m"(result.u64)
        : "m"(mem.u64), "m"(reg.u64)
        : "mm0", "mm1"
    );
    
    print_mmx("result", result);
    printf("Expected: [5, 5, 0, -10, -15, 0, 20, 20]\n"); // mem with signs from reg
    
    asm volatile ("emms");
    return 0;
}
