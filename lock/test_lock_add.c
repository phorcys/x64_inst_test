#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

#define TEST_COUNT 10000

// Test lock add with different operand sizes
static void test_lock_add_size() {
    uint8_t byte1 = 0x55, byte2 = 0xAA;
    uint16_t word1 = 0x5555, word2 = 0xAAAA; 
    uint32_t dword1 = 0x55555555, dword2 = 0xAAAAAAAA;
    uint64_t qword1 = 0x5555555555555555, qword2 = 0xAAAAAAAAAAAAAAAA;

    printf("Testing LOCK ADD with different operand sizes:\n");

    // Test byte size
    asm volatile("lock addb %1, %0" : "+m"(byte1) : "ri"(byte2));
    printf("BYTE: 0x%02X + 0x%02X = 0x%02X\n", 0x55, 0xAA, byte1);

    // Test word size
    asm volatile("lock addw %1, %0" : "+m"(word1) : "ri"(word2));
    printf("WORD: 0x%04X + 0x%04X = 0x%04X\n", 0x5555, 0xAAAA, word1);

    // Test dword size  
    asm volatile("lock addl %1, %0" : "+m"(dword1) : "ri"(dword2));
    printf("DWORD: 0x%08X + 0x%08X = 0x%08X\n", 0x55555555, 0xAAAAAAAA, dword1);

    // Test qword size
    asm volatile("lock addq %1, %0" : "+m"(qword1) : "ri"(qword2));
    printf("QWORD: 0x%016lX + 0x%016lX = 0x%016lX\n", 
           0x5555555555555555, 0xAAAAAAAAAAAAAAAA, qword1);
}

// Test lock add with different addressing modes
static void test_lock_add_addressing() {
    uint32_t mem = 0x12345678;
    uint32_t reg = 0x87654321;

    printf("\nTesting LOCK ADD with different addressing modes:\n");

    // Register to memory
    asm volatile("lock addl %1, %0" : "+m"(mem) : "r"(reg));
    printf("REG->MEM: 0x12345678 + 0x87654321 = 0x%08X\n", mem);

    // Immediate to memory
    mem = 0x12345678;
    asm volatile("lock addl $0x87654321, %0" : "+m"(mem));
    printf("IMM->MEM: 0x12345678 + 0x87654321 = 0x%08X\n", mem);
}

// Test lock add in multithreaded environment
static atomic_uint shared = 0;
static void* thread_func(void* arg) {
    (void)arg; // Silence unused parameter warning
    for (int i = 0; i < TEST_COUNT; ++i) {
        asm volatile("lock addl $1, %0" : "+m"(shared));
    }
    return NULL;
}

static void test_lock_add_threading() {
    pthread_t t1, t2;
    
    printf("\nTesting LOCK ADD in multithreaded environment:\n");
    printf("Initial value: 0\n");

    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final value: %u (expected: %d)\n", shared, TEST_COUNT*2);
}

int main() {
    printf("=== Testing LOCK ADD instruction ===\n\n");
    
    test_lock_add_size();
    test_lock_add_addressing();
    test_lock_add_threading();

    printf("\n=== LOCK ADD test complete ===\n");
    return 0;
}
