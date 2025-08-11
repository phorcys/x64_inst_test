#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

// Global jump buffer for handling exceptions
static jmp_buf env;

// Signal handler for SIGFPE (divide error)
void handle_sigfpe(int sig) {
    (void)sig;
    printf("Caught SIGFPE (Divide Error)\n");
    longjmp(env, 1);
}

// Test IDIV instruction for various operand sizes
int main() {
    printf("Testing IDIV:\n");
    printf("=============\n");
    
    // Register signal handler
    signal(SIGFPE, handle_sigfpe);
    
    // Test cases for 8-bit division
    printf("\n8-bit division:\n");
    printf("---------------\n");
    int8_t divisors_8[] = {1, 2, -3, 4, -5};
    int16_t dividends_8[] = {100, -200, 300, -400, 127, -128};
    
    for (size_t i = 0; i < sizeof(dividends_8)/sizeof(dividends_8[0]); i++) {
        for (size_t j = 0; j < sizeof(divisors_8)/sizeof(divisors_8[0]); j++) {
            int16_t dividend = dividends_8[i];
            int8_t divisor = divisors_8[j];
            int8_t quotient, remainder;
            
            // Skip cases that would cause quotient overflow
            if (dividend == -128 && divisor == -1) {
                printf("Skipping test: %d / %d (would cause quotient overflow)\n", dividend, divisor);
                continue;
            }
            
            // Skip cases where quotient would be out of 8-bit range
            if (dividend / divisor < -128 || dividend / divisor > 127) {
                printf("Skipping test: %d / %d (quotient out of 8-bit range)\n", dividend, divisor);
                continue;
            }
            
            if (setjmp(env) == 0) {
                // Perform 8-bit division
                asm volatile (
                    "movw %[dividend], %%ax\n\t"
                    "idivb %[divisor]\n\t"
                    "movb %%al, %[quotient]\n\t"
                    "movb %%ah, %[remainder]"
                    : [quotient] "=r" (quotient), [remainder] "=r" (remainder)
                    : [dividend] "r" (dividend), [divisor] "r" (divisor)
                    : "ax", "cc"
                );
                
                printf("IDIVb: %d / %d = %d (rem %d)\n", dividend, divisor, quotient, remainder);
            } else {
                printf("Caught division error: %d / %d\n", dividend, divisor);
            }
        }
    }
    
    // Test cases for 16-bit division
    printf("\n16-bit division:\n");
    printf("----------------\n");
    int16_t divisors_16[] = {1, -2, 3, -4, 5};
    int32_t dividends_16[] = {1000, -2000, 3000, -4000, 32767, -32768};
    
    for (size_t i = 0; i < sizeof(dividends_16)/sizeof(dividends_16[0]); i++) {
        for (size_t j = 0; j < sizeof(divisors_16)/sizeof(divisors_16[0]); j++) {
            int32_t dividend = dividends_16[i];
            int16_t divisor = divisors_16[j];
            int16_t quotient, remainder;
            
            // Skip cases that would cause quotient overflow
            if (dividend == -32768 && divisor == -1) {
                printf("Skipping test: %d / %d (would cause quotient overflow)\n", dividend, divisor);
                continue;
            }
            
            // Skip cases where quotient would be out of 16-bit range
            if (dividend / divisor < -32768 || dividend / divisor > 32767) {
                printf("Skipping test: %d / %d (quotient out of 16-bit range)\n", dividend, divisor);
                continue;
            }
            
            if (setjmp(env) == 0) {
                // Perform 16-bit division
                asm volatile (
                    "movl %[dividend], %%eax\n\t"
                    "cwtd\n\t"
                    "idivw %[divisor]\n\t"
                    "movw %%ax, %[quotient]\n\t"
                    "movw %%dx, %[remainder]"
                    : [quotient] "=r" (quotient), [remainder] "=r" (remainder)
                    : [dividend] "r" (dividend), [divisor] "r" (divisor)
                    : "ax", "dx", "cc"
                );
                
                printf("IDIVw: %d / %d = %d (rem %d)\n", dividend, divisor, quotient, remainder);
            } else {
                printf("Caught division error: %d / %d\n", dividend, divisor);
            }
        }
    }
    
    // Test cases for 32-bit division
    printf("\n32-bit division:\n");
    printf("----------------\n");
    int32_t divisors_32[] = {1, -2, 3, -4, 5};
    int64_t dividends_32[] = {100000, -200000, 300000, -400000, 2147483647, -2147483648};
    
    for (size_t i = 0; i < sizeof(dividends_32)/sizeof(dividends_32[0]); i++) {
        for (size_t j = 0; j < sizeof(divisors_32)/sizeof(divisors_32[0]); j++) {
            int64_t dividend = dividends_32[i];
            int32_t divisor = divisors_32[j];
            int32_t quotient, remainder;
            
            // Skip cases that would cause quotient overflow
            if (dividend == -2147483648LL && divisor == -1) {
                printf("Skipping test: %ld / %d (would cause quotient overflow)\n", dividend, divisor);
                continue;
            }
            
            // Skip cases where quotient would be out of 32-bit range
            if (dividend / divisor < -2147483648LL || dividend / divisor > 2147483647LL) {
                printf("Skipping test: %ld / %d (quotient out of 32-bit range)\n", dividend, divisor);
                continue;
            }
            
            if (setjmp(env) == 0) {
                // Perform 32-bit division
                asm volatile (
                    "movq %[dividend], %%rax\n\t"
                    "cqo\n\t"
                    "idivl %[divisor]\n\t"
                    "movl %%eax, %[quotient]\n\t"
                    "movl %%edx, %[remainder]"
                    : [quotient] "=r" (quotient), [remainder] "=r" (remainder)
                    : [dividend] "r" (dividend), [divisor] "r" (divisor)
                    : "rax", "rdx", "cc"
                );
                
                printf("IDIVl: %ld / %d = %d (rem %d)\n", dividend, divisor, quotient, remainder);
            } else {
                printf("Caught division error: %ld / %d\n", dividend, divisor);
            }
        }
    }
    
    // Test cases for 64-bit division
    printf("\n64-bit division:\n");
    printf("----------------\n");
    int64_t divisors_64[] = {1, -2, 3, -4, 5};
    int64_t dividends_64[] = {10000000000, -20000000000, 30000000000, -40000000000, 9223372036854775807LL, -9223372036854775807LL-1};
    
    for (size_t i = 0; i < sizeof(dividends_64)/sizeof(dividends_64[0]); i++) {
        for (size_t j = 0; j < sizeof(divisors_64)/sizeof(divisors_64[0]); j++) {
            int64_t dividend = dividends_64[i];
            int64_t divisor = divisors_64[j];
            int64_t quotient, remainder;
            
            // Skip cases that would cause quotient overflow
            if (dividend == -9223372036854775807LL-1 && divisor == -1) {
                printf("Skipping test: %ld / %ld (would cause quotient overflow)\n", dividend, divisor);
                continue;
            }
            
            if (setjmp(env) == 0) {
                // Perform 64-bit division
                asm volatile (
                    "movq %[dividend], %%rax\n\t"
                    "cqo\n\t"
                    "idivq %[divisor]\n\t"
                    "movq %%rax, %[quotient]\n\t"
                    "movq %%rdx, %[remainder]"
                    : [quotient] "=r" (quotient), [remainder] "=r" (remainder)
                    : [dividend] "r" (dividend), [divisor] "r" (divisor)
                    : "rax", "rdx", "cc"
                );
                
                printf("IDIVq: %ld / %ld = %ld (rem %ld)\n", dividend, divisor, quotient, remainder);
            } else {
                printf("Caught division error: %ld / %ld\n", dividend, divisor);
            }
        }
    }
    
    // Test divide by zero
    printf("\nTesting divide by zero:\n");
    printf("----------------------\n");
    if (setjmp(env) == 0) {
        int32_t dividend = 100;
        int32_t divisor = 0;
        int32_t quotient;
        
        asm volatile (
            "movl %[dividend], %%eax\n\t"
            "cwtd\n\t"
            "idivl %[divisor]\n\t"
            "movl %%eax, %[quotient]"
            : [quotient] "=r" (quotient)
            : [dividend] "r" (dividend), [divisor] "r" (divisor)
            : "ax", "dx", "cc"
        );
        
        printf("Unexpected success: %d / %d = %d\n", dividend, divisor, quotient);
    } else {
        printf("Caught division by zero as expected\n");
    }
    
    printf("\nIDIV tests completed.\n");
    return 0;
}
