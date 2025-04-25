#include <stdio.h>
#include <stdint.h>
#include <fenv.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

// Test FWAIT instruction by checking FPU control word synchronization
void test_fwait() {
    uint16_t cw1, cw2;
    uint16_t status1, status2;
    
    printf("=== Testing FWAIT instruction ===\n");
    
    // Test 1: Basic synchronization test
    printf("\nTest 1: Basic synchronization\n");
    asm volatile ("fninit");
    
    uint16_t control_word = 0x037F;
    
    // Test without FWAIT
    asm volatile (
        "fldcw %1\n\t"
        "fnstsw %0"
        : "=m" (cw1)
        : "m" (control_word)
        : "memory"
    );
    
    // Test with FWAIT
    asm volatile (
        "fldcw %1\n\t"
        "fwait\n\t"
        "fnstsw %0"
        : "=m" (cw2)
        : "m" (control_word)
        : "memory"
    );
    
    printf("Control word without FWAIT: 0x%04x\n", cw1);
    printf("Control word with FWAIT: 0x%04x\n", cw2);
    
    TEST_CASE("Control words match", cw1 == cw2);

    // Test 2: Exception state synchronization
    printf("\nTest 2: Exception state\n");
    asm volatile ("fninit");
    
    // Generate divide by zero exception
    float zero = 0.0f;
    float one = 1.0f;
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fwait\n\t"
        "fnstsw %2"
        : 
        : "m" (zero), "m" (one), "m" (status1)
        : "memory"
    );
    
    // Without FWAIT
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fnstsw %2"
        : 
        : "m" (zero), "m" (one), "m" (status2)
        : "memory"
    );
    
    printf("Status with FWAIT: 0x%04x\n", status1);
    printf("Status without FWAIT: 0x%04x\n", status2);
    
    TEST_CASE("Exception states match", (status1 & 0x3F) == (status2 & 0x3F));

    // Test 3: Multiple FWAIT instructions
    printf("\nTest 3: Multiple FWAITs\n");
    asm volatile ("fninit");
    uint16_t cw3, cw4;
    
    asm volatile (
        "fldcw %1\n\t"
        "fwait\n\t"
        "fwait\n\t"
        "fnstsw %0"
        : "=m" (cw3)
        : "m" (control_word)
        : "memory"
    );
    
    asm volatile (
        "fldcw %1\n\t"
        "fnstsw %0"
        : "=m" (cw4)
        : "m" (control_word)
        : "memory"
    );
    
    printf("Control word with double FWAIT: 0x%04x\n", cw3);
    printf("Control word without FWAIT: 0x%04x\n", cw4);
    
    TEST_CASE("Multiple FWAITs work correctly", cw3 == cw4);
}

void test_fwait_vs_fnop() {
    printf("\n==== Testing FWAIT vs FNOP ====\n");
    
    // Initialize FPU
    asm volatile ("fninit");
    
    // Get initial state
    (void)get_x87_cw();
    (void)get_x87_sw(); 
    (void)get_x87_tw();
    
    printf("Initial state:\n");
    print_x87_status();
    
    // Execute FWAIT
    asm volatile ("fwait");
    
    // Get state after FWAIT
    uint16_t cw_after_fwait = get_x87_cw();
    uint16_t sw_after_fwait = get_x87_sw();
    uint16_t tw_after_fwait = get_x87_tw();
    
    printf("\nAfter FWAIT:\n");
    print_x87_status();
    
    // Execute FNOP
    asm volatile ("fnop");
    
    // Get state after FNOP
    uint16_t cw_after_fnop = get_x87_cw();
    uint16_t sw_after_fnop = get_x87_sw();
    uint16_t tw_after_fnop = get_x87_tw();
    
    printf("\nAfter FNOP:\n");
    print_x87_status();
    
    // Compare states
    TEST_CASE("FWAIT vs FNOP control word", cw_after_fwait == cw_after_fnop);
    TEST_CASE("FWAIT vs FNOP status word", sw_after_fwait == sw_after_fnop);
    TEST_CASE("FWAIT vs FNOP tag word", tw_after_fwait == tw_after_fnop);
}

void test_fwait_timing() {
    printf("\n==== Testing FWAIT Timing ====\n");
    
    // Measure FWAIT execution time (relative)
    uint64_t start, end;
    const int iterations = 1000000;
    
    asm volatile (
        "rdtsc\n\t"
        "shl $32, %%rdx\n\t"
        "or %%rdx, %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (start)
        : 
        : "rdx", "rax"
    );
    
    for (int i = 0; i < iterations; i++) {
        asm volatile ("fwait");
    }
    
    asm volatile (
        "rdtsc\n\t"
        "shl $32, %%rdx\n\t"
        "or %%rdx, %%rax\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (end)
        : 
        : "rdx", "rax"
    );
    
    uint64_t cycles = (end - start) / iterations;
    printf("Average cycles per FWAIT: %lu\n", cycles);
    TEST_CASE("FWAIT executes quickly", cycles < 10);  // Should be very fast
}

void test_fwait_exception_handling() {
    printf("\n==== Testing FWAIT Exception Handling ====\n");
    
    // Test with unmasked divide by zero
    uint16_t cw = 0x037F & ~0x04;  // Unmask zero divide
    asm volatile ("fninit");
    asm volatile ("fldcw %0" : : "m" (cw));
    
    float zero = 0.0f;
    float one = 1.0f;
    int exception_occurred = 0;
    
    // Clear exception flags
    feclearexcept(FE_ALL_EXCEPT);
    
    printf("Before exception:\n");
    print_x87_status();
    
    // Attempt divide by zero with FWAIT
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fwait"
        : 
        : "m" (zero), "m" (one)
    );
    
    // Check if exception occurred
    exception_occurred = fetestexcept(FE_DIVBYZERO);
    TEST_CASE("Divide by zero exception triggered", exception_occurred);
    
    printf("\nAfter exception with FWAIT:\n");
    print_x87_status();
}

void test_fwait_status() {
    printf("\n==== Testing FWAIT Status Effects ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        uint16_t expected_sw;
    } tests[] = {
        {"All exceptions masked", 0x037F, 0x0000},
        {"All exceptions unmasked", 0x0300, 0x003F},
        {"Partial exceptions masked", 0x033F, 0x002A},
        {"Extended precision", 0x0300, 0x3800},
        {"Single precision", 0x0000, 0x0000}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        // Execute FWAIT and get status
        uint16_t status;
        asm volatile (
            "fwait\n\t"
            "fnstsw %0"
            : "=m" (status)
            :
            : "memory"
        );
        
        printf("Status after FWAIT: 0x%04X\n", status);
        TEST_CASE("Status word matches expected", 
                 (status & 0xFF80) == (tests[i].expected_sw & 0xFF80));
    }
}

void test_fwait_modern() {
    printf("\n==== Testing FWAIT with Modern CPU Features ====\n");
    
    // Test with SSE enabled
    printf("\n-- Test 1: With SSE --\n");
    uint32_t mxcsr_before, mxcsr_after;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_before));
    
    asm volatile ("fwait");
    
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_after));
    printf("MXCSR before: 0x%08X\n", mxcsr_before);
    printf("MXCSR after:  0x%08X\n", mxcsr_after);
    TEST_CASE("MXCSR unchanged with FWAIT", mxcsr_before == mxcsr_after);
    
    // Test with AVX enabled
    printf("\n-- Test 2: With AVX --\n");
    uint64_t xcr0_before, xcr0_after;
    asm volatile ("xgetbv" : "=a" (xcr0_before) : "c" (0) : "edx");
    
    asm volatile ("fwait");
    
    asm volatile ("xgetbv" : "=a" (xcr0_after) : "c" (0) : "edx");
    printf("XCR0 before: 0x%016lX\n", xcr0_before);
    printf("XCR0 after:  0x%016lX\n", xcr0_after);
    TEST_CASE("XCR0 unchanged with FWAIT", xcr0_before == xcr0_after);
}

void test_fwait_boundary() {
    printf("\n==== Testing FWAIT Boundary Conditions ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        int expected_exceptions;
    } tests[] = {
        {"Minimum control word (all unmasked)", 0x0000, FE_ALL_EXCEPT},
        {"Single bit set (IE unmasked)", 0x0001, FE_INVALID},
        {"Max value - 1", 0x7FFF, 0},
        {"Maximum control word", 0xFFFF, 0},
        {"Alternating bits", 0x5555, FE_INVALID|FE_UNDERFLOW|FE_OVERFLOW},
        {"Inverse bits", 0xAAAA, FE_DIVBYZERO|FE_INEXACT}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s (CW=0x%04X)\n", i+1, tests[i].desc, tests[i].cw);
        
        // Clear exceptions
        feclearexcept(FE_ALL_EXCEPT);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        // Execute FWAIT and get status
        uint16_t status;
        asm volatile (
            "fwait\n\t"
            "fnstsw %0"
            : "=m" (status)
            :
            : "memory"
        );
        
        // Check exceptions
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        
        printf("Status after FWAIT: 0x%04X\n", status);
        printf("Exceptions: 0x%X\n", exceptions);
        
        TEST_CASE("No unexpected exceptions", (status & 0x3F) == 0);
        TEST_CASE("Correct exceptions raised",
                 (exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions);
    }
    
    // Test with pending exceptions
    printf("\nTesting with pending exceptions:\n");
    uint16_t cw = 0x0300;  // All exceptions unmasked
    asm volatile ("fninit");
    asm volatile ("fldcw %0" : : "m" (cw));
    
    float zero = 0.0f;
    float one = 1.0f;
    uint16_t status;
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fwait\n\t"
        "fnstsw %2"
        : 
        : "m" (zero), "m" (one), "m" (status)
        : "memory"
    );
    
    printf("Status after FWAIT with exception: 0x%04X\n", status);
    TEST_CASE("Exception flag set", (status & 0x04) != 0);
}

void test_fwait_combinations() {
    printf("\n==== Testing FWAIT Combinations ====\n");
    
    // Test FWAIT with different FPU states
    struct {
        const char* desc;
        uint16_t cw;
    } tests[] = {
        {"All exceptions masked", 0x037F},
        {"All exceptions unmasked", 0x0300},
        {"Partial exceptions masked", 0x033F},
        {"Different rounding modes", 0x0C00},
        {"Different precision modes", 0x0200}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        // Execute FWAIT
        uint16_t status;
        asm volatile (
            "fwait\n\t"
            "fnstsw %0"
            : "=m" (status)
            :
            : "memory"
        );
        
        printf("Status after FWAIT: 0x%04X\n", status);
        TEST_CASE("No unexpected exceptions", (status & 0x3F) == 0);
    }
}

void test_fwait_extended() {
    printf("\n==== Extended FWAIT Tests ====\n");
    
    // Test FWAIT with pending exceptions
    asm volatile ("fninit");
    float zero = 0.0f;
    float one = 1.0f;
    uint16_t status;
    
    printf("Before exception:\n");
    print_x87_status();
    
    // Generate exception with FWAIT
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fwait\n\t"
        "fnstsw %2"
        : 
        : "m" (zero), "m" (one), "m" (status)
        : "memory"
    );
    
    printf("\nAfter exception with FWAIT:\n");
    print_x87_status();
    TEST_CASE("Divide by zero flag set", status & 0x04);
}

int main() {
    printf("=== Testing FWAIT instruction ===\n");
    
    test_fwait();
    test_fwait_vs_fnop();
    //test_fwait_timing();
    //test_fwait_exception_handling();
//    test_fwait_boundary();
    test_fwait_modern();
    test_fwait_status();
    test_fwait_combinations();
    test_fwait_extended();
    
    printf("\n=== FWAIT tests completed ===\n");
    return 0;
}
