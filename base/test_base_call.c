#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Function prototypes
void test_relative_call();
void test_register_indirect_call();
void test_memory_indirect_call();

// Global variables for test verification
volatile int test_step = 0;

// Target functions for call tests
void target_function1() {
    test_step++;
}

void target_function2() {
    test_step++;
}

void target_function3() {
    test_step++;
}

int main() {
    printf("Starting CALL instruction test\n");
    
    // Test 1: Near relative call
    test_step = 0;
    test_relative_call();
    if (test_step != 1) {
        printf("FAIL: Relative call test - Expected step:1, Got step:%d\n", test_step);
        return 1;
    }
    printf("PASS: Near relative call test\n");
    
    // Test 2: Near absolute indirect call via register
    test_step = 0;
    test_register_indirect_call();
    if (test_step != 1) {
        printf("FAIL: Register indirect call test - Expected step:1, Got step:%d\n", test_step);
        return 1;
    }
    printf("PASS: Near absolute indirect call (register operand)\n");
    
    // Test 3: Near absolute indirect call via memory
    test_step = 0;
    test_memory_indirect_call();
    if (test_step != 1) {
        printf("FAIL: Memory indirect call test - Expected step:1, Got step:%d\n", test_step);
        return 1;
    }
    printf("PASS: Near absolute indirect call (memory operand)\n");
    
    printf("All CALL instruction tests passed!\n");
    return 0;
}

void test_relative_call() {
    asm volatile ("call target_function1");
}

void test_register_indirect_call() {
    void (*func_ptr)() = target_function2;
    asm volatile (
        "call *%0"
        : 
        : "r"(func_ptr)
        : "memory"
    );
}

void test_memory_indirect_call() {
    void (*func_ptr)() = target_function3;
    void (*func_mem)() = func_ptr;
    asm volatile (
        "call *%0"
        : 
        : "m"(func_mem)
        : "memory"
    );
}
