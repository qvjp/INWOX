ARCH ?= i686

TO_ROOT = ..

include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/libc

ARFLAGS := -rcs
CFLAGS ?= -O0 -g
CFLAGS += --sysroot=$(SYSROOT) -std=gnu11 -ffreestanding -Wall -Wextra
CFLAGS += -D__USE_INWOX
CXXFLAGS ?= -O2 -g
CXXFLAGS += --sysroot=$(SYSROOT) -std=gnu++14 -ffreestanding -Wall -Wextra
CPPFLAGS += -I include -I $(SYSROOT)/usr/include

LIBC_FLAGS += -D__is_inwox_libc
LIBK_FLAGS += -D__is_inwox_libk

COMMON_OBJ := \
	ctype/isspace \
	cxx/new \
	cxx/purevirtual \
	errno/errno \
	libgen/basename \
	libgen/dirname \
	stdio/vcbprintf \
	stdlib/calloc \
	stdlib/free \
	stdlib/malloc \
	stdlib/realloc \
	stdlib/strtol \
	stdlib/strtoul \
	string/stpcpy \
	string/memcmp \
	string/memcpy \
	string/memmove \
	string/memset \
	string/strcmp \
	string/strcspn \
	string/strdup \
	string/strlen \
	string/strnlen \
	string/strncmp \
	string/strrchr

CRT_OBJ := \
	$(BUILD)/arch/i686/crt0.o \
	$(BUILD)/arch/i686/crti.o \
	$(BUILD)/arch/i686/crtn.o

LIBC_OBJ := \
	$(COMMON_OBJ) \
	assert/assert \
	fcntl/open \
	fcntl/openat \
	stdio/dprintf \
	stdio/fclose \
	stdio/fdopen \
	stdio/fflush \
	stdio/fgetc \
	stdio/fgets \
	stdio/fprintf \
	stdio/fputc \
	stdio/fputs \
	stdio/fopen \
	stdio/getc \
	stdio/getchar \
	stdio/printf \
	stdio/putchar \
	stdio/putc \
	stdio/puts \
	stdio/sprintf \
	stdio/stderr \
	stdio/stdin \
	stdio/stdout \
	stdio/vdprintf \
	stdio/vfprintf \
	stdio/vprintf \
	stdio/vsprintf \
	stdlib/_Exit \
	stdlib/exit \
	sys/mman/mmap \
	sys/mman/munmap \
	sys/wait/waitpid \
	unistd/close \
	unistd/environ \
	unistd/execl \
	unistd/execv \
	unistd/execve \
	unistd/fork \
	unistd/_exit \
	unistd/read \
	unistd/write

LIBK_OBJ := \
	$(COMMON_OBJ)

LIBC_OBJ += \
	arch/i686/rfork \
	arch/i686/syscall

LIBC_OBJ := $(addprefix $(BUILD)/, $(addsuffix .o, $(LIBC_OBJ)))
LIBK_OBJ := $(addprefix $(BUILD)/, $(addsuffix .libk.o, $(LIBK_OBJ)))

all: build_info $(BUILD)/libc.a $(BUILD)/libk.a $(CRT_OBJ)
	@echo -n ${COLOR_RESET}

build_info:
	@echo ${COLOR_GREEN}Building libc...

install: install-headers install-libs

install-headers: $(SYSROOT)/usr/include

install-libs: $(BUILD)/libc.a $(BUILD)/libk.a $(CRT_OBJ)
	@mkdir -p $(LIB_DIR)
	cp -f $^ $(LIB_DIR)

$(SYSROOT)/usr/include: include $(TO_ROOT)/kernel/include
	@mkdir -p $@
	@touch $@
	cp -rf $^ $(SYSROOT)/usr

# 使用ar将*.libc.o创建成库libc.a
$(BUILD)/libc.a: $(LIBC_OBJ)
	$(AR) $(ARFLAGS) $@ $^

# 使用ar将*.libk.o创建成库libk.a
$(BUILD)/libk.a: $(LIBK_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LIBC_FLAGS) -c -o $@ $^

$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBC_FLAGS) -c -o $@ $^

$(BUILD)/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(CPPFLAGS) $(LIBC_FLAGS) -c -o $@ $^

$(BUILD)/%.libk.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LIBK_FLAGS) -c -o $@ $^

$(BUILD)/%.libk.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBK_FLAGS) -c -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all install install-headers install-libs clean
