#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include "base.h"

static sigjmp_buf jmpbuf;

void handle_sigsegv(int sig) {
    siglongjmp(jmpbuf, 1);
}

int main() {
    printf("RDPMC instruction test\n");
    printf("======================\n");

    int errors = 0;

    signal(SIGSEGV, handle_sigsegv);

    uint32_t ecx = 0; // Counter number (0 for cycles)

    if (sigsetjmp(jmpbuf, 1) == 0) {
        uint64_t eax, edx;
        __asm__ __volatile__ (
            "rdpmc"
            : "=a"(eax), "=d"(edx)
            : "c"(ecx)
        );
        printf("RDPMC did not cause SIGSEGV (unexpected): FAIL\n");
        errors++;
    } else {
        printf("RDPMC caused SIGSEGV (expected in user mode): PASS\n");
    }

    printf("\nRDPMC test completed\n");
    printf("====================\n");
    printf("Total tests: 1\n");
    printf("Passed: %d\n", 1 - errors);
    printf("Failed: %d\n", errors);

    return errors ? 1 : 0;
}
