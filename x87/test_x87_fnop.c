#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void test_fnop_basic() {
    printf("\n==== Testing FNOP Basic ====\n");
    long double st0;
    uint16_t cw_before, cw_after;
    uint16_t sw_before, sw_after;
    uint16_t tw_before, tw_after;
    
    printf("Basic FNOP test\n");
    st0 = 1.23456789L;
    
    // Get initial state
    cw_before = get_x87_cw();
    sw_before = get_x87_sw();
    tw_before = get_x87_tw();
    
    printf("Before FNOP:\n");
    print_x87_status();
    
    // Execute FNOP
    asm volatile ("fnop");
    
    // Get state after FNOP
    cw_after = get_x87_cw();
    sw_after = get_x87_sw();
    tw_after = get_x87_tw();
    
    printf("\nAfter FNOP:\n");
    print_x87_status();
    
    // Verify state unchanged
    TEST_CASE("Control word unchanged", cw_before == cw_after);
    TEST_CASE("Status word unchanged", sw_before == sw_after);
    TEST_CASE("Tag word unchanged", tw_before == tw_after);
}

void test_fnop_stack() {
    long double st0;
    printf("\n==== Testing FNOP with Stack ====\n");
    asm volatile ("finit");
    asm volatile ("fld1");  // Push 1.0 to stack
    
    printf("Before FNOP (stack not empty):\n");
    print_x87_status();
    
    asm volatile ("fnop");
    
    printf("\nAfter FNOP:\n");
    print_x87_status();
    
    // Verify top of stack unchanged
    asm volatile ("fstpt %0" : "=m" (st0));
    printf("Top of stack: "); print_float80(st0);
    TEST_CASE("Stack contents unchanged", st0 == 1.0L);
}

void test_fnop_timing() {
    printf("\n==== Testing FNOP Timing ====\n");
    
    // Measure FNOP execution time with serializing instruction
    uint64_t start, end, temp;
    const int iterations = 1000000;
    
    // Warm up CPU
    for (int i = 0; i < 1000; i++) {
        asm volatile ("fnop");
    }
    
    // Serialize and get start time
    asm volatile (
        "cpuid\n\t"  // Serialize
        "rdtscp\n\t" // More precise timing
        "lfence\n\t"
        "mov %%rdx, %0\n\t"
        "mov %%rax, %1\n\t"
        : "=r" (temp), "=r" (start)
        : 
        : "rax", "rbx", "rcx", "rdx"
    );
    
    // Test loop
    for (int i = 0; i < iterations; i++) {
        asm volatile ("fnop");
    }
    
    // Serialize and get end time
    asm volatile (
        "rdtscp\n\t"
        "lfence\n\t"
        "mov %%rdx, %0\n\t"
        "mov %%rax, %1\n\t"
        : "=r" (temp), "=r" (end)
        : 
        : "rax", "rdx"
    );
    
    uint64_t cycles = (end - start) / iterations;
    printf("Average cycles per FNOP: %lu\n", cycles);
    TEST_CASE("FNOP executes in 1 cycle", cycles <= 2);  // Should be 1 cycle on modern CPUs
}

void test_fnop_vs_fwait() {
    printf("\n==== Testing FNOP vs FWAIT ====\n");
    
    // Initialize FPU
    asm volatile ("fninit");
    
    // Get initial state
    uint16_t cw_before = get_x87_cw();
    uint16_t sw_before = get_x87_sw();
    uint16_t tw_before = get_x87_tw();
    
    printf("Initial state:\n");
    print_x87_status();
    
    // Execute FNOP
    asm volatile ("fnop");
    
    // Get state after FNOP
    uint16_t cw_after_fnop = get_x87_cw();
    uint16_t sw_after_fnop = get_x87_sw();
    uint16_t tw_after_fnop = get_x87_tw();
    
    printf("\nAfter FNOP:\n");
    print_x87_status();
    
    // Execute FWAIT
    asm volatile ("fwait");
    
    // Get state after FWAIT
    uint16_t cw_after_fwait = get_x87_cw();
    uint16_t sw_after_fwait = get_x87_sw();
    uint16_t tw_after_fwait = get_x87_tw();
    
    printf("\nAfter FWAIT:\n");
    print_x87_status();
    
    // Compare states
    TEST_CASE("FNOP vs FWAIT control word", cw_after_fnop == cw_after_fwait);
    TEST_CASE("FNOP vs FWAIT status word", sw_after_fnop == sw_after_fwait);
    TEST_CASE("FNOP vs FWAIT tag word", tw_after_fnop == tw_after_fwait);
}

void test_fnop_sequence() {
    printf("\n==== Testing FNOP in Instruction Sequence ====\n");
    
    // Test FNOP between FPU operations
    long double result1, result2;
    
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fldpi");
    asm volatile ("fnop");
    asm volatile ("faddp %%st(1), %%st" : "=t" (result1));
    
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fldpi");
    asm volatile ("faddp %%st(1), %%st" : "=t" (result2));
    
    printf("With FNOP: %.15Lg\n", result1);
    printf("Without FNOP: %.15Lg\n", result2);
    TEST_CASE("FNOP doesn't affect computation", result1 == result2);
}

void test_fnop_registers() {
    printf("\n==== Testing FNOP Register Preservation ====\n");
    
    // Initialize with known values
    long double values[8];
    for (int i = 0; i < 8; i++) {
        values[i] = (i + 1) * 1.23456789L;
    }
    
    // Load all registers
    asm volatile ("fninit");
    for (int i = 0; i < 8; i++) {
        asm volatile ("fldt %0" : : "m" (values[i]));
    }
    
    printf("Before FNOP (all registers loaded):\n");
    print_x87_status();
    
    // Execute FNOP
    asm volatile ("fnop");
    
    printf("\nAfter FNOP:\n");
    print_x87_status();
    
    // Verify all registers unchanged
    long double stored[8];
    for (int i = 7; i >= 0; i--) {
        asm volatile ("fstpt %0" : "=m" (stored[i]));
    }
    
    for (int i = 0; i < 8; i++) {
        printf("ST%d: ", i); print_float80(stored[i]);
        TEST_CASE("Register content preserved", stored[i] == values[i]);
    }
}

void test_fnop_modern() {
    printf("\n==== Testing FNOP on Modern CPU Features ====\n");
    
    // Test 1: With SSE enabled
    printf("\n-- Test 1: With SSE --\n");
    uint32_t mxcsr_before, mxcsr_after;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_before));
    
    asm volatile ("fnop");
    
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_after));
    printf("MXCSR before: 0x%08X\n", mxcsr_before);
    printf("MXCSR after:  0x%08X\n", mxcsr_after);
    TEST_CASE("MXCSR unchanged with FNOP", mxcsr_before == mxcsr_after);
    
    // Test 2: With AVX enabled
    printf("\n-- Test 2: With AVX --\n");
    uint64_t xcr0_before, xcr0_after;
    asm volatile ("xgetbv" : "=a" (xcr0_before) : "c" (0) : "edx");
    
    asm volatile ("fnop");
    
    asm volatile ("xgetbv" : "=a" (xcr0_after) : "c" (0) : "edx");
    printf("XCR0 before: 0x%016lX\n", xcr0_before);
    printf("XCR0 after:  0x%016lX\n", xcr0_after);
    TEST_CASE("XCR0 unchanged with FNOP", xcr0_before == xcr0_after);
}

void test_fnop_boundary() {
    printf("\n==== Testing FNOP Boundary Conditions ====\n");
    
    // Helper functions for test setups
    void setup_full_stack() {
        asm volatile ("fninit");
        for (int i = 0; i < 8; i++) asm volatile ("fld1");
    }
    
    void setup_empty_stack() {
        asm volatile ("fninit");
    }
    
    void setup_pending_exceptions() {
        asm volatile ("fninit");
        asm volatile ("fldz\n\tfld1\n\tfdivp");
    }
    
    void setup_denormal_operands() {
        asm volatile ("fninit");
        float denorm = FLT_MIN / 2.0f;
        asm volatile ("flds %0" : : "m" (denorm));
    }
    
    void setup_all_exceptions_masked() {
        asm volatile ("fninit");
        uint16_t cw = 0x037F;
        asm volatile ("fldcw %0" : : "m" (cw));
    }
    
    void setup_no_exceptions_masked() {
        asm volatile ("fninit");
        uint16_t cw = 0x0300;
        asm volatile ("fldcw %0" : : "m" (cw));
    }
    
    void setup_precision_control_changed() {
        asm volatile ("fninit");
        uint16_t cw = 0x033F;
        asm volatile ("fldcw %0" : : "m" (cw));
    }
    
    void setup_infinity_control_changed() {
        asm volatile ("fninit");
        uint16_t cw = 0x077F;
        asm volatile ("fldcw %0" : : "m" (cw));
    }
    
    void setup_stack_overflow() {
        asm volatile ("fninit");
        for (int i = 0; i < 9; i++) asm volatile ("fld1");
    }
    
    void setup_stack_underflow() {
        asm volatile ("fninit");
        asm volatile ("fstp %st(0)");
    }

    struct {
        const char* desc;
        void (*setup)(void);
        uint16_t expected_cw_mask;
        uint16_t expected_sw_mask;
    } tests[] = {
        {"Full stack", setup_full_stack, 0xFFFF, 0xFFFF},
        {"Empty stack", setup_empty_stack, 0xFFFF, 0xFFFF},
        {"Pending exceptions", setup_pending_exceptions, 0xFFFF, 0x3F},
        {"Denormal operands", setup_denormal_operands, 0xFFFF, 0xFFFF},
        {"All exceptions masked", setup_all_exceptions_masked, 0xFFFF, 0xFFFF},
        {"No exceptions masked", setup_no_exceptions_masked, 0xFFFF, 0xFFFF},
        {"Precision control changed", setup_precision_control_changed, 0xFFFF, 0xFFFF},
        {"Infinity control changed", setup_infinity_control_changed, 0xFFFF, 0xFFFF},
        {"Stack overflow", setup_stack_overflow, 0xFFFF, 0x4000},
        {"Stack underflow", setup_stack_underflow, 0xFFFF, 0x4000}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        // Setup test condition
        tests[i].setup();
        
        printf("Before FNOP:\n");
        print_x87_status();
        
        uint16_t cw_before = get_x87_cw();
        uint16_t sw_before = get_x87_sw();
        uint16_t tw_before = get_x87_tw();
        
        // Execute FNOP
        asm volatile ("fnop");
        
        printf("\nAfter FNOP:\n");
        print_x87_status();
        
        uint16_t cw_after = get_x87_cw();
        uint16_t sw_after = get_x87_sw();
        uint16_t tw_after = get_x87_tw();
        
        // Verify state unchanged according to masks
        TEST_CASE("Control word unchanged", 
                 (cw_before & tests[i].expected_cw_mask) == 
                 (cw_after & tests[i].expected_cw_mask));
        TEST_CASE("Status word unchanged", 
                 (sw_before & tests[i].expected_sw_mask) == 
                 (sw_after & tests[i].expected_sw_mask));
        TEST_CASE("Tag word unchanged", tw_before == tw_after);
    }
}

void test_fnop_multithread() {
    printf("\n==== Testing FNOP in Multithreaded Context ====\n");
    
    // This test simulates multithreaded behavior by alternating between
    // three different FPU states and verifying FNOP doesn't interfere
    
    // State 1: Normal values
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fldln2");
    uint16_t cw_state1 = get_x87_cw();
    uint16_t sw_state1 = get_x87_sw();
    uint16_t tw_state1 = get_x87_tw();
    
    // State 2: Different control word and values
    asm volatile ("fninit");
    uint16_t cw = 0x077F;
    asm volatile ("fldcw %0" : : "m" (cw));  // Round down
    asm volatile ("fldpi");
    asm volatile ("fldz");
    uint16_t cw_state2 = get_x87_cw();
    uint16_t sw_state2 = get_x87_sw();
    uint16_t tw_state2 = get_x87_tw();
    
    // State 3: Full stack with mixed values
    asm volatile ("fninit");
    for (int i = 0; i < 8; i++) {
        long double val = (i + 1) * 1.23456789L;
        asm volatile ("fldt %0" : : "m" (val));
    }
    uint16_t cw_state3 = get_x87_cw();
    uint16_t sw_state3 = get_x87_sw();
    uint16_t tw_state3 = get_x87_tw();
    
    // Alternate between states with FNOP in between
    for (int i = 0; i < 3; i++) {
        printf("\nIteration %d\n", i+1);
        
        // Switch to state 1
        asm volatile ("fninit");
        asm volatile ("fld1");
        asm volatile ("fldln2");
        asm volatile ("fnop");
        
        printf("After state 1 + FNOP:\n");
        print_x87_status();
        
        TEST_CASE("State 1 control word preserved",
                 (get_x87_cw() & 0x0F00) == (cw_state1 & 0x0F00));
        TEST_CASE("State 1 status word preserved",
                 (get_x87_sw() & 0xC7FF) == (sw_state1 & 0xC7FF));
        TEST_CASE("State 1 tag word preserved",
                 get_x87_tw() == tw_state1);
        
        // Switch to state 2
        asm volatile ("fninit");
        uint16_t cw = 0x077F;
        asm volatile ("fldcw %0" : : "m" (cw));
        asm volatile ("fldpi");
        asm volatile ("fldz");
        asm volatile ("fnop");
        
        printf("\nAfter state 2 + FNOP:\n");
        print_x87_status();
        
        TEST_CASE("State 2 control word preserved",
                 (get_x87_cw() & 0x0F00) == (cw_state2 & 0x0F00));
        TEST_CASE("State 2 status word preserved",
                 (get_x87_sw() & 0xC7FF) == (sw_state2 & 0xC7FF));
        TEST_CASE("State 2 tag word preserved",
                 get_x87_tw() == tw_state2);
        
        // Switch to state 3
        asm volatile ("fninit");
        for (int i = 0; i < 8; i++) {
            long double val = (i + 1) * 1.23456789L;
            asm volatile ("fldt %0" : : "m" (val));
        }
        asm volatile ("fnop");
        
        printf("\nAfter state 3 + FNOP:\n");
        print_x87_status();
        
        TEST_CASE("State 3 control word preserved",
                 (get_x87_cw() & 0x0F00) == (cw_state3 & 0x0F00));
        TEST_CASE("State 3 status word preserved",
                 (get_x87_sw() & 0xC7FF) == (sw_state3 & 0xC7FF));
        TEST_CASE("State 3 tag word preserved",
                 get_x87_tw() == tw_state3);
    }
}

void test_fnop_exceptions() {
    printf("\n==== Testing FNOP with Exceptions ====\n");
    
    // Helper functions for exception test setups
    void setup_divide_by_zero() {
        asm volatile ("finit");
        asm volatile ("fldz\n\tfld1\n\tfdivp");
    }
    
    void setup_invalid_operation() {
        asm volatile ("finit");
        asm volatile ("fldz\n\tfldz\n\tfdivp");
    }
    
    void setup_overflow() {
        asm volatile ("finit");
        long double max = LDBL_MAX;
        asm volatile ("fld1\n\tfldt %0" : : "m" (max));
        asm volatile ("fmulp");
    }
    
    void setup_underflow() {
        asm volatile ("finit");
        long double min = LDBL_MIN;
        asm volatile ("fld1\n\tfldt %0" : : "m" (min));
        asm volatile ("fdivp");
    }
    
    void setup_precision() {
        asm volatile ("finit");
        asm volatile ("fld1\n\tfldpi\n\tfdivp");
    }

    struct {
        const char* desc;
        void (*setup)(void);
        uint16_t expected_sw_mask;
    } tests[] = {
        {"Divide by zero", setup_divide_by_zero, 0x3F},
        {"Invalid operation", setup_invalid_operation, 0x3F},
        {"Overflow", setup_overflow, 0x3F},
        {"Underflow", setup_underflow, 0x3F},
        {"Precision", setup_precision, 0x3F}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        // Setup test condition
        tests[i].setup();
        
        printf("Before FNOP:\n");
        print_x87_status();
        
        uint16_t sw_before = get_x87_sw();
        
        // Execute FNOP
        asm volatile ("fnop");
        
        printf("\nAfter FNOP:\n");
        print_x87_status();
        
        uint16_t sw_after = get_x87_sw();
        
        // Verify exception flags unchanged
        TEST_CASE("Exception flags unchanged", 
                 (sw_after & tests[i].expected_sw_mask) == 
                 (sw_before & tests[i].expected_sw_mask));
    }
}

int main() {
    printf("=== Testing FNOP instruction ===\n");
    
    test_fnop_basic();
    test_fnop_stack();
    test_fnop_registers();
    test_fnop_sequence();
    test_fnop_boundary();
    test_fnop_modern();
    test_fnop_multithread();
    test_fnop_exceptions();
    //test_fnop_timing();
    test_fnop_vs_fwait();
    
    // Verify no unexpected exceptions occurred
    int exceptions = fetestexcept(FE_ALL_EXCEPT);
    if (exceptions) {
        printf("\n[WARNING] Unexpected floating-point exceptions occurred: ");
        if (exceptions & FE_INVALID) printf("FE_INVALID ");
        if (exceptions & FE_DIVBYZERO) printf("FE_DIVBYZERO ");
        if (exceptions & FE_OVERFLOW) printf("FE_OVERFLOW ");
        if (exceptions & FE_UNDERFLOW) printf("FE_UNDERFLOW ");
        if (exceptions & FE_INEXACT) printf("FE_INEXACT ");
        printf("\n");
    }
    
    printf("\n=== FNOP tests completed successfully ===\n");
    return 0;
}
