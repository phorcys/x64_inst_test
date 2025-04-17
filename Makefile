# Makefile for compiling and generating reference outputs

# Target directories 
DIRS := mmx lock sse sse2 sse3 avx crypt arith x87

# Default CFLAGS
CC := gcc-14
CFLAGS := -Wall -Wextra -O0 -march=core2 -pthread -D_GNU_SOURCE
MMX_CFLAGS := -Wall -Wextra -O0 -mmmx -pthread -D_GNU_SOURCE -lm
SSE_CFLAGS := -Wall -Wextra -O0 -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 \
             -pthread -D_GNU_SOURCE

# Get all .c files
SRCS := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
EXES := $(SRCS:.c=)

# Default target
all: $(EXES)
	@echo "Build completed for: $(EXES)"

# Pattern rule to build executables
%: %.c
	@echo "Building $@ from $<"
	@if echo "$@" | grep -q '^mmx/'; then \
		$(CC) $(MMX_CFLAGS) -o $@ $<; \
	elif echo "$@" | grep -q '^sse/'; then \
		$(CC) $(SSE_CFLAGS) -o $@ $<; \
	else \
		$(CC) $(CFLAGS) -o $@ $<; \
	fi
	@if [ ! -f $@ ]; then echo "Error: Failed to build $@"; exit 1; fi

# Target to generate reference outputs  
genref: all
	@for exe in $(EXES); do \
		echo "Generating reference for $$exe"; \
		./$$exe > $$exe.ref.txt; \
	done

# Clean target
clean:
	@for exe in $(EXES); do \
		echo "Removing $$exe and $$exe.ref.txt"; \
		rm -f $$exe $$exe.ref.txt  $$exe.o; \
	done

# Test target to run all tests with translator
test: $(if $(filter-out test,$(MAKECMDGOALS)),test-$(firstword $(filter mmx sse lock avx crypt arith x87,$(MAKECMDGOALS))),test-all)

test-all:
	@for exe in $(EXES); do \
		$$TRANSLATOR ./$$exe > test_res.txt; \
		if diff -q test_res.txt $$exe.ref.txt >/dev/null; then \
			echo "\033[32m$$exe: PASSED\033[0m"; \
		else \
			echo "\033[31m$$exe: FAILED\033[0m"; \
			diff -u $$exe.ref.txt test_res.txt; \
		fi; \
		rm -f test_res.txt; \
	done

test-%:
	@for exe in $(filter $*/%,$(EXES)); do \
		$$TRANSLATOR ./$$exe > test_res.txt; \
		if diff -q test_res.txt $$exe.ref.txt >/dev/null; then \
			echo "\033[32m$$exe: PASSED\033[0m"; \
		else \
			echo "\033[31m$$exe: FAILED\033[0m"; \
			diff -u $$exe.ref.txt test_res.txt; \
		fi; \
		rm -f test_res.txt; \
	done

%:
	@:

.PHONY: all genref clean test
