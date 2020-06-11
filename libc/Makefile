ARCH ?= i686

TO_ROOT = ..

include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/libc

ARFLAGS := -rcs
CFLAGS ?= -O0 -g
CFLAGS += -std=gnu11 -ffreestanding -Wall -Wextra
CXXFLAGS ?= -O2 -g
CXXFLAGS += -std=gnu++14 -ffreestanding -Wall -Wextra
CPPFLAGS += -I include

LIBK_OBJ := \
	cxx/new \
	stdlib/free \
	stdlib/malloc \
	string/memcmp \
	string/memcpy \
	string/memmove \
	string/memset \
	string/strlen

LIBK_OBJ := $(addprefix $(BUILD)/, $(addsuffix .libk.o, $(LIBK_OBJ)))

all: $(BUILD)/libk.a

install: install-headers install-libk

install-headers: $(SYSROOT)/usr/include

install-libk: $(BUILD)/libk.a
	@mkdir -p $(SYSROOT)/usr/lib
	cp -f $^ $(SYSROOT)/usr/lib

$(SYSROOT)/usr/include: include
	@mkdir -p $@
	@touch $@
	cp -fr include $(SYSROOT)/usr

# 使用ar将*.libk.o创建成库libk.a
$(BUILD)/libk.a: $(LIBK_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/%.libk.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD)/%.libk.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all install install-headers install-libk clean
