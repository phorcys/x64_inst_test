#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef enum {
    RNE,    // Round to Nearest Even
    RZ,     // Round towards Zero
    RD,     // Round Down (-inf)
    RU,     // Round Up (+inf)
    MODE_COUNT
} RoundingMode;

const char* mode_names[] = {"RNE", "RZ", "RD", "RU"};

#ifdef __x86_64__
/* x86_64 模式编码映射 */
static const unsigned int mode_encodings[] = {
    [RNE] = 0,  // 00b
    [RZ]  = 3,  // 11b
    [RD]  = 1,  // 01b
    [RU]  = 2   // 10b
};

void set_rounding_mode(RoundingMode mode) {
    uint32_t mxcsr;
    asm volatile("stmxcsr %0" : "=m"(mxcsr));
    mxcsr = (mxcsr & ~0x6000) | (mode_encodings[mode] << 13);
    asm volatile("ldmxcsr %0" : : "m"(mxcsr));
}

int32_t cvt_f32_i32(float f) {
    int32_t res;
    asm volatile("cvtss2si %1, %0" : "=r"(res) : "x"(f));
    return res;
}

int64_t cvt_f32_i64(float f) {
    int64_t res;
    asm volatile("cvtss2si %1, %0" : "=r"(res) : "x"(f));
    return res;
}

int32_t cvt_f64_i32(double d) {
    int32_t res;
    asm volatile("cvtsd2si %1, %0" : "=r"(res) : "x"(d));
    return res;
}

int64_t cvt_f64_i64(double d) {
    int64_t res;
    asm volatile("cvtsd2si %1, %0" : "=r"(res) : "x"(d));
    return res;
}

#elif defined(__loongarch64)
/* LoongArch64 模式编码 */
static const unsigned int mode_encodings[] = {
    [RNE] = 0,
    [RZ]  = 1,
    [RD]  = 2,
    [RU]  = 3
};

void set_rounding_mode(RoundingMode mode) {
    unsigned int fcsr;
    asm volatile("movfcsr2gr %0, $r0" : "=r"(fcsr));
    fcsr = (fcsr & ~(0x7 << 2)) | (mode_encodings[mode] << 2);
    asm volatile("movgr2fcsr $r0, %0" : : "r"(fcsr));
}

int32_t cvt_f32_i32(float f) {
    int32_t res;
    asm volatile("fcvt.w.s %0, %1" : "=r"(res) : "f"(f));
    return res;
}

int64_t cvt_f32_i64(float f) {
    int64_t res;
    asm volatile("fcvt.l.s %0, %1" : "=r"(res) : "f"(f));
    return res;
}

int32_t cvt_f64_i32(double d) {
    int32_t res;
    asm volatile("fcvt.w.d %0, %1" : "=r"(res) : "f"(d));
    return res;
}

int64_t cvt_f64_i64(double d) {
    int64_t res;
    asm volatile("fcvt.l.d %0, %1" : "=r"(res) : "f"(d));
    return res;
}
#else
#error "Unsupported architecture"
#endif

const char* get_special_name(float f) {
    if (isnan(f)) return signbit(f) ? "-NaN" : "+NaN";
    if (isinf(f)) return signbit(f) ? "-Inf" : "+Inf";
    return NULL;
}

const char* get_special_name_d(double d) {
    if (isnan(d)) return signbit(d) ? "-NaN" : "+NaN";
    if (isinf(d)) return signbit(d) ? "-Inf" : "+Inf";
    return NULL;
}

void print_result(const char* type, const char* name, RoundingMode mode, int64_t val) {
    printf("%-6s %-12s [%-3s] => ", type, name, mode_names[mode]);
    
    if (val == INT64_MIN) {  // 特殊值标记
        printf("0x%016lx\n", (uint64_t)val);
    } else {
        printf("%-20ld\n", val);
    }
}

void test_float(float f) {
    const char* name = get_special_name(f);
    char tmp[32];
    if (!name) {
        snprintf(tmp, sizeof(tmp), "%.9g", f);
        name = tmp;
    }

    for (RoundingMode m = RNE; m < MODE_COUNT; m++) {
        set_rounding_mode(m);
        print_result("F32", name, m, cvt_f32_i32(f));
        print_result("F32", name, m, cvt_f32_i64(f));
    }
}

void test_double(double d) {
    const char* name = get_special_name_d(d);
    char tmp[32];
    if (!name) {
        snprintf(tmp, sizeof(tmp), "%.17g", d);
        name = tmp;
    }

    for (RoundingMode m = RNE; m < MODE_COUNT; m++) {
        set_rounding_mode(m);
        print_result("F64", name, m, cvt_f64_i32(d));
        print_result("F64", name, m, cvt_f64_i64(d));
    }
}

int main() {
    float test_floats[] = {
        0.0f, -0.0f, 1.5f, -1.5f, 2.5f, -2.5f,
        2147483647.0f, -2147483648.0f, INFINITY, -INFINITY, NAN, -NAN
    };

    double test_doubles[] = {
        0.0, -0.0, 1.5, -1.5, 2.5, -2.5,
        2147483647.0, -2147483648.0, 9223372036854775807.0,
        -9223372036854775808.0, INFINITY, -INFINITY, NAN, -NAN
    };

    puts("Testing Float32 conversions:");
    for (size_t i = 0; i < sizeof(test_floats)/sizeof(test_floats[0]); i++) {
        test_float(test_floats[i]);
    }

    puts("\nTesting Float64 conversions:");
    for (size_t i = 0; i < sizeof(test_doubles)/sizeof(test_doubles[0]); i++) {
        test_double(test_doubles[i]);
    }

    return 0;
}