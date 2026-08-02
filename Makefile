# Tessera

# ':=' not '?=' -- make has a built-in default CC of 'cc', whose origin is
# 'default' rather than 'undefined', so '?=' would silently leave it alone.
# Command-line assignment still wins:  make CC=clang check
CC := gcc

BUILD_DIR := build
APP_MAIN  := src/main.c

# Production sources exclude the application's main(), so tests can supply
# their own without a duplicate-symbol link error.
LIBSRC  := $(shell find src -name '*.c' ! -path '$(APP_MAIN)' 2>/dev/null)
HEADERS := $(shell find src -name '*.h' 2>/dev/null)
TESTSRC := $(shell find tests -name '*.c' 2>/dev/null)

# _POSIX_C_SOURCE exposes POSIX APIs that strict -std=c11 hides (strdup,
# socket, bind, accept, shm_open...). _DEFAULT_SOURCE covers the widely-used
# extensions POSIX itself omits -- MAP_ANONYMOUS being the one week 4 needs.
CPPFLAGS := -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -Isrc

CFLAGS := -std=c11 -Wall -Wextra -Werror -Wpedantic -g3 -O0

# -pthread, not -lpthread: also defines _REENTRANT at compile time.
THREAD_FLAGS := -pthread

# ASan and TSan are incompatible at compile time -- separate binaries.
# UBSan combines cheaply with ASan, so the asan target carries both.
ASAN_FLAGS  := -fsanitize=address,undefined \
               -fno-sanitize-recover=all \
               -fno-omit-frame-pointer

UBSAN_FLAGS := -fsanitize=undefined \
               -fno-sanitize-recover=all \
               -fno-omit-frame-pointer

TSAN_FLAGS  := -fsanitize=thread \
               -fno-omit-frame-pointer

.PHONY: all test asan ubsan tsan valgrind check clean help

all: $(BUILD_DIR)/tessera

# --------------------------------------------------------------------
# Application
# --------------------------------------------------------------------

$(BUILD_DIR)/tessera: $(APP_MAIN) $(LIBSRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(THREAD_FLAGS) \
		$(APP_MAIN) $(LIBSRC) \
		-o $@

# --------------------------------------------------------------------
# Test commands
# --------------------------------------------------------------------

test: $(BUILD_DIR)/test_plain
	./$(BUILD_DIR)/test_plain

asan: $(BUILD_DIR)/test_asan
	./$(BUILD_DIR)/test_asan

ubsan: $(BUILD_DIR)/test_ubsan
	./$(BUILD_DIR)/test_ubsan

# GCC TSan conflicts with the VM's vm.mmap_rnd_bits=32 memory layout.
# Disable ASLR only for this child process; no system setting is changed.
tsan: $(BUILD_DIR)/test_tsan
	setarch "$$(uname -m)" -R \
		env TSAN_OPTIONS=halt_on_error=1 \
		./$(BUILD_DIR)/test_tsan

valgrind: $(BUILD_DIR)/test_plain
	valgrind \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--error-exitcode=1 \
		./$(BUILD_DIR)/test_plain

# --------------------------------------------------------------------
# Test binaries
# --------------------------------------------------------------------

$(BUILD_DIR)/test_plain: $(LIBSRC) $(TESTSRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(THREAD_FLAGS) \
		$(LIBSRC) $(TESTSRC) \
		-o $@

$(BUILD_DIR)/test_asan: $(LIBSRC) $(TESTSRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(ASAN_FLAGS) $(THREAD_FLAGS) \
		$(LIBSRC) $(TESTSRC) \
		-o $@

$(BUILD_DIR)/test_ubsan: $(LIBSRC) $(TESTSRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(UBSAN_FLAGS) $(THREAD_FLAGS) \
		$(LIBSRC) $(TESTSRC) \
		-o $@

$(BUILD_DIR)/test_tsan: $(LIBSRC) $(TESTSRC) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(TSAN_FLAGS) $(THREAD_FLAGS) \
		$(LIBSRC) $(TESTSRC) \
		-o $@

# --------------------------------------------------------------------
# Combined verification
# --------------------------------------------------------------------

# 'ubsan' is omitted: the asan target already includes UBSan.
check: test asan tsan valgrind
	@echo "All checks passed."

# --------------------------------------------------------------------
# Utilities
# --------------------------------------------------------------------

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Targets:"
	@echo "  make            Build build/tessera"
	@echo "  make test       Run tests, no instrumentation"
	@echo "  make asan       Run tests under AddressSanitizer + UBSan"
	@echo "  make ubsan      Run tests under UBSan alone"
	@echo "  make tsan       Run tests under ThreadSanitizer"
	@echo "  make valgrind   Run tests under Valgrind"
	@echo "  make check      test + asan + tsan + valgrind"
	@echo "  make clean      Remove build/"
	@echo ""
	@echo "  make clean && make CC=clang check"
