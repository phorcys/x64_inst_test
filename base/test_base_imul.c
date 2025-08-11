#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Helper function to print flags - only CF and OF are defined for IMUL
void print_flags(uint64_t flags, const char* prefix) {
    printf("%sCF=%d, OF=%d\n", 
           prefix,
           (int)(flags & 1),          // CF
           (int)((flags >> 11) & 1)   // OF
    );
}

int main() {
    printf("Testing IMUL instruction:\n");
    printf("=========================\n");
    
    // Variables for results
    uint64_t flags;
    
    // Test values
    int8_t i8_vals[] = {0, 1, -1, 2, -2, 64, -64, 127, -128};
    int16_t i16_vals[] = {0, 1, -1, 2, -2, 256, -256, 32767, -32768};
    int32_t i32_vals[] = {0, 1, -1, 2, -2, 65536, -65536, 2147483647, -2147483648};
    int64_t i64_vals[] = {0, 1, -1, 2, -2, 4294967296LL, -4294967296LL, 9223372036854775807LL, (-9223372036854775807LL - 1)};
    
    // Test one-operand form (8-bit)
    printf("\n8-bit one-operand form:\n");
    printf("------------------------\n");
    for (size_t i = 0; i < sizeof(i8_vals)/sizeof(i8_vals[0]); i++) {
        int8_t i8_operand = i8_vals[i];
        for (size_t j = 0; j < sizeof(i8_vals)/sizeof(i8_vals[0]); j++) {
            int8_t al_val = i8_vals[j];
            int16_t ax_result;
            
            asm volatile (
                "movb %[al_val], %%al\n\t"
                "imulb %[operand]\n\t"
                "movw %%ax, %[ax_result]\n\t"
                "pushfq\n\t"
                "popq %[flags]"
                : [ax_result] "=m" (ax_result), [flags] "=r" (flags)
                : [al_val] "r" (al_val), [operand] "r" (i8_operand)
                : "ax", "cc"
            );
            
            printf("IMULb: AL=%d * %d = %d (AX=0x%04X)\n", 
                   al_val, i8_operand, (int8_t)(ax_result & 0xFF), ax_result);
            print_flags(flags, "  Flags: ");
        }
    }
    
    // Test one-operand form (16-bit)
    printf("\n16-bit one-operand form:\n");
    printf("-------------------------\n");
    for (size_t i = 0; i < sizeof(i16_vals)/sizeof(i16_vals[0]); i++) {
        int16_t i16_operand = i16_vals[i];
        for (size_t j = 0; j < sizeof(i16_vals)/sizeof(i16_vals[0]); j++) {
            int16_t ax_val = i16_vals[j];
            int32_t result;
            uint16_t dx, ax;
            
            asm volatile (
                "movw %[ax_val], %%ax\n\t"
                "imulw %[operand]\n\t"
                "movw %%dx, %[dx]\n\t"
                "movw %%ax, %[ax]\n\t"
                "pushfq\n\t"
                "popq %[flags]"
                : [dx] "=r" (dx), [ax] "=r" (ax), [flags] "=r" (flags)
                : [ax_val] "r" (ax_val), [operand] "r" (i16_operand)
                : "ax", "dx", "cc"
            );
            
            result = (dx << 16) | ax;
            printf("IMULw: AX=%d * %d = %d (DX:AX=0x%08X)\n", 
                   ax_val, i16_operand, (int16_t)result, result);
            print_flags(flags, "  Flags: ");
        }
    }
    
    // Test two-operand form (32-bit)
    printf("\n32-bit two-operand form:\n");
    printf("-------------------------\n");
    for (size_t i = 0; i < sizeof(i32_vals)/sizeof(i32_vals[0]); i++) {
        int32_t i32_operand = i32_vals[i];
        for (size_t j = 0; j < sizeof(i32_vals)/sizeof(i32_vals[0]); j++) {
            int32_t reg_val = i32_vals[j];
            int32_t result;
            
            asm volatile (
                "movl %[reg_val], %%eax\n\t"
                "imull %[operand], %%eax\n\t"
                "movl %%eax, %[result]\n\t"
                "pushfq\n\t"
                "popq %[flags]"
                : [result] "=r" (result), [flags] "=r" (flags)
                : [reg_val] "r" (reg_val), [operand] "r" (i32_operand)
                : "eax", "cc"
            );
            
            printf("IMULl: EAX=%d * %d = %d\n", reg_val, i32_operand, result);
            print_flags(flags, "  Flags: ");
        }
    }
    
    // Test three-operand form (64-bit with immediate)
    printf("\n64-bit three-operand form:\n");
    printf("---------------------------\n");
    for (size_t j = 0; j < sizeof(i64_vals)/sizeof(i64_vals[0]); j++) {
        int64_t src_val = i64_vals[j];
        int64_t result;
        
        // Test with 8-bit immediate
        asm volatile (
            "movq %[src_val], %%rax\n\t"
            "imulq $2, %%rax\n\t"
            "movq %%rax, %[result]\n\t"
            "pushfq\n\t"
            "popq %[flags]"
            : [result] "=r" (result), [flags] "=r" (flags)
            : [src_val] "r" (src_val)
            : "rax", "cc"
        );
        printf("IMULq: %ld * 2 = %ld\n", src_val, result);
        print_flags(flags, "  Flags (imm8): ");
        
        // Test with 32-bit immediate
        asm volatile (
            "movq %[src_val], %%rax\n\t"
            "imulq $4, %%rax\n\t"
            "movq %%rax, %[result]\n\t"
            "pushfq\n\t"
            "popq %[flags]"
            : [result] "=r" (result), [flags] "=r" (flags)
            : [src_val] "r" (src_val)
            : "rax", "cc"
        );
        printf("IMULq: %ld * 4 = %ld\n", src_val, result);
        print_flags(flags, "  Flags (imm32): ");
    }
    
    printf("\nIMUL tests completed.\n");
    return 0;
}
