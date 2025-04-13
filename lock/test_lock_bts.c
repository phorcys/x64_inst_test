#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

#define TEST_COUNT 10000

// Test lock bts with different operand sizes
static void test_lock_bts_size() {
    uint8_t byte = 0x55;
    uint16_t word = 0x5555;
    uint32_t dword = 0x55555555;
    uint64_t qword = 0x5555555555555555;

    printf("Testing LOCK BTS with different operand sizes:\n");

    // Test byte size
    asm volatile("lock bts $3, %0" : "+m"(byte));
    printf("BYTE: 0x55 with bit 3 set = 0x%02X\n", byte);

    // Test word size
    asm volatile("lock bts $7, %0" : "+m"(word));
    printf("WORD: 0x5555 with bit 7 set = 0x%04X\n", word);

    // Test dword size
    asm volatile("lock bts $15, %0" : "+m"(dword));
    printf("DWORD: 0x55555555 with bit 15 set = 0x%08X\n", dword);

    // Test qword size
    asm volatile("lock bts $31, %0" : "+m"(qword));
    printf("QWORD: 0x5555555555555555 with bit 31 set = 0x%016lX\n", qword);
}

// Test lock bts with different addressing modes
static void test_lock_bts_addressing() {
    uint32_t mem = 0x12345678;
    uint32_t reg = 7;

    printf("\nTesting LOCK BTS with different addressing modes:\n");

    // Register bit position
    asm volatile("lock bts %1, %0" : "+m"(mem) : "r"(reg));
    printf("REG->MEM: 0x12345678 with bit 7 set = 0x%08X\n", mem);

    // Immediate bit position
    mem = 0x12345678;
    asm volatile("lock bts $15, %0" : "+m"(mem));
    printf("IMM->MEM: 0x12345678 with bit 15 set = 0x%08X\n", mem);
}

// Test lock bts in multithreaded environment
static atomic_uint shared = 0;
static void* thread_func(void* arg) {
    (void)arg;
    for (int i = 0; i < TEST_COUNT; ++i) {
        asm volatile("lock bts $0, %0" : "+m"(shared));
    }
    return NULL;
}

static void test_lock_bts_threading() {
    pthread_t t1, t2;
    
    printf("\nTesting LOCK BTS in multithreaded environment:\n");
    printf("Initial value: %u\n", shared);

    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final value: %u (expected: 1)\n", shared);
}

int main() {
    printf("=== Testing LOCK BTS instruction ===\n\n");
    
    test_lock_bts_size();
    test_lock_bts_addressing();
    test_lock_bts_threading();

    printf("\n=== LOCK BTS test complete ===\n");
    return 0;
}
